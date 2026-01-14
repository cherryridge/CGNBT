#pragma once
#include <array>
#include <format>
#include <string>
#include <vector>
#include <boost/unordered/unordered_map.hpp>
#include <physfs.h>

#include "type.hpp"
#include "FileReader.hpp"
#include "auxiliary.hpp"
#include "error.hpp"

namespace NBT::IO {
    typedef char8_t c8;
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using namespace NBT::Type;
    using std::vector, std::array, std::span, std::string, std::move, std::bit_cast, std::to_string, std::format, boost::unordered_flat_map, NBT::Aux::readVarText, NBT::Aux::readIVarInt, NBT::Aux::readUVarInt, NBT::Error::clearErrors, NBT::Error::pushError;

    [[nodiscard]] inline bool readObject     (FileReader&, TagObject&     , bool topLevel = false) noexcept;
                  inline void readIVarInt    (FileReader&, TagIVarInt&    )                        noexcept;
                  inline void readUVarInt    (FileReader&, TagUVarInt&    )                        noexcept;
                  inline void readBool       (FileReader&, TagBool&       , u8)                    noexcept;
                  inline void readHex        (FileReader&, TagHex&        , u8)                    noexcept;
                  inline void readFloat      (FileReader&, TagFloat&      )                        noexcept;
                  inline void readDouble     (FileReader&, TagDouble&     )                        noexcept;
    [[nodiscard]] inline bool readArray      (FileReader&, TagArray&      , const Types)           noexcept;
    [[nodiscard]] inline bool readString     (FileReader&, TagString&     )                        noexcept;
                  inline void readRaw        (FileReader&, TagRaw&        )                        noexcept;
    [[nodiscard]] inline bool readArrayBool  (FileReader&, TagArrayBool&  )                        noexcept;
    [[nodiscard]] inline bool readArrayHex   (FileReader&, TagArrayHex&   )                        noexcept;
    [[nodiscard]] inline bool readArrayFloat (FileReader&, TagArrayFloat& )                        noexcept;
    [[nodiscard]] inline bool readArrayDouble(FileReader&, TagArrayDouble&)                        noexcept;
    [[nodiscard]] inline bool readArrayRaw   (FileReader&, TagArrayRaw&   )                        noexcept;

    struct NBTFileInfo {
        u64 fileSize{ 0 };
        bool validFile{ false }, compressed{ false };
    };

    [[nodiscard]] inline bool read(const char* path, unordered_flat_map<string, Tag>& result) noexcept {
        clearErrors();
        FileReader cursor(path);
        if (!cursor) {
            pushError(string("File failed to open: ") + path);
            return false;
        }
        if (cursor.empty()) {
            //Empty file is valid, but contains no data.
            result.clear();
            if (!cursor.close()) pushError(format("Failed to close file: {}!", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
            return true;
        }
        //Caveat: We can actually treat the top level tags as they are in a embedded `Object` tag.
        TagObject topLevel;
        if (readObject(cursor, topLevel, true)) {
            result = move(topLevel.payload);
            if (!cursor.close()) pushError(format("Failed to close file: {}!", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
            return true;
        }
        else if (!cursor.close()) pushError(format("Failed to close file: {}!", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
        return false;
    }

    [[nodiscard]] inline NBTFileInfo getFileInfo(const char* path) noexcept {
        clearErrors();
        NBTFileInfo result;
        FileReader cursor(path);
        result.validFile = !!cursor;
        result.compressed = cursor.compressed();
        result.fileSize = cursor.getFileSize();
        if (!cursor.close()) pushError(format("Failed to close file: {}!", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
        return result;
    }

    [[nodiscard]] inline bool readObject(FileReader& cursor, TagObject& result, bool topLevel) noexcept {
        auto type = getType(*cursor);
        while (topLevel ? !!cursor : type != Types::ObjectEnd) {
            switch (type) {
                case Types::Object: {
                    ++cursor;
                    TagObject temp;
                    string name = readVarText(cursor);
                    if (readObject(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::IVarInt: {
                    ++cursor;
                    TagIVarInt temp;
                    string name = readVarText(cursor);
                    readIVarInt(cursor, temp);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::UVarInt: {
                    ++cursor;
                    TagUVarInt temp;
                    string name = readVarText(cursor);
                    readUVarInt(cursor, temp);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::Bool: {
                    auto cv = *cursor;
                    ++cursor;
                    TagBool temp;
                    string name = readVarText(cursor);
                    readBool(cursor, temp, cv);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::Hex: {
                    auto cv = *cursor;
                    ++cursor;
                    TagHex temp;
                    string name = readVarText(cursor);
                    readHex(cursor, temp, cv);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::Float: {
                    ++cursor;
                    TagFloat temp;
                    string name = readVarText(cursor);
                    readFloat(cursor, temp);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::Double: {
                    ++cursor;
                    TagDouble temp;
                    string name = readVarText(cursor);
                    readDouble(cursor, temp);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::Array: {
                    auto secType = getSecondType(*cursor);
                    ++cursor;
                    TagArray temp;
                    string name = readVarText(cursor);
                    if (readArray(cursor, temp, secType)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::String: {
                    ++cursor;
                    TagString temp;
                    string name = readVarText(cursor);
                    if (readString(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::Raw: {
                    ++cursor;
                    TagRaw temp;
                    string name = readVarText(cursor);
                    readRaw(cursor, temp);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::ArrayBool: {
                    ++cursor;
                    TagArrayBool temp;
                    string name = readVarText(cursor);
                    if (readArrayBool(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayHex: {
                    ++cursor;
                    TagArrayHex temp;
                    string name = readVarText(cursor);
                    if (readArrayHex(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayFloat: {
                    ++cursor;
                    TagArrayFloat temp;
                    string name = readVarText(cursor);
                    if (readArrayFloat(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayDouble: {
                    ++cursor;
                    TagArrayDouble temp;
                    string name = readVarText(cursor);
                    if (readArrayDouble(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayRaw: {
                    ++cursor;
                    TagArrayRaw temp;
                    string name = readVarText(cursor);
                    if (readArrayRaw(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                default: {
                    pushError(format("Invalid type ID {} in object at pos {}!", static_cast<u8>(type), cursor.currentOffset()));
                    return false;
                }
            }
            if (!cursor) break;
            type = getType(*cursor);
        }
        if (!topLevel) ++cursor;
        return true;
    }

    inline void readIVarInt(FileReader& cursor, TagIVarInt& result) noexcept { result.payload = readIVarInt(cursor); }

    inline void readUVarInt(FileReader& cursor, TagUVarInt& result) noexcept { result.payload = readUVarInt(cursor); }

    inline void readBool(FileReader& cursor, TagBool& result, u8 cv) noexcept { result.payload = cv & 0x01; }

    inline void readHex(FileReader& cursor, TagHex& result, u8 cv) noexcept { result.payload = cv & 0x0F; }

    inline void readFloat(FileReader& cursor, TagFloat& result) noexcept {
        u32 temp = 0;
        for (u8 i = 0; i < sizeof(float); i++) {
            auto byte = *cursor;
            temp |= static_cast<u32>(byte) << i * 8;
            ++cursor;
        }
        result.payload = bit_cast<float>(temp);
    }

    inline void readDouble(FileReader& cursor, TagDouble& result) noexcept {
        u64 temp = 0;
        for (u8 i = 0; i < sizeof(double); i++) {
            auto byte = *cursor;
            temp |= static_cast<u64>(byte) << i * 8;
            ++cursor;
        }
        result.payload = bit_cast<double>(temp);
    }

    [[nodiscard]] inline bool readArray(FileReader& cursor, TagArray& result, const Types type) noexcept {
        auto count = readUVarInt(cursor);
        result.payload.resize(count);
        switch (type) {
            case Types::Object: {
                for (u64 i = 0; i < count; i++) {
                    new (&result.payload[i].tagObject) TagObject;
                    result.payload[i].type = Types::Object;
                    if (!readObject(cursor, result.payload[i].tagObject)) return false;
                }
                break;
            }
            case Types::IVarInt: {
                for (u64 i = 0; i < count; i++) {
                    new (&result.payload[i].tagIVarInt) TagIVarInt;
                    result.payload[i].type = Types::IVarInt;
                    readIVarInt(cursor, result.payload[i].tagIVarInt);
                }
                break;
            }
            case Types::UVarInt: {
                for (u64 i = 0; i < count; i++) {
                    new (&result.payload[i].tagUVarInt) TagUVarInt;
                    result.payload[i].type = Types::UVarInt;
                    readUVarInt(cursor, result.payload[i].tagUVarInt);
                }
                break;
            }
            case Types::Array: {
                auto type = getSecondType(*cursor);
                switch (type) {
                    case Types::Object:
                    case Types::IVarInt:
                    case Types::UVarInt:
                    case Types::Array:
                    case Types::String: {
                        for (u64 i = 0; i < count; i++) {
                            new (&result.payload[i].tagArray) TagArray;
                            result.payload[i].type = Types::Array;
                            //Pass head block of each array.
                            ++cursor;
                            if (!readArray(cursor, result.payload[i].tagArray, type)) return false;
                        }
                        break;
                    }
                    case Types::Bool: {
                        for(u64 i = 0; i < count; i++) {
                            new (&result.payload[i].tagArrayBool) TagArrayBool;
                            result.payload[i].type = Types::ArrayBool;
                            //Pass head block of each array.
                            ++cursor;
                            if (!readArrayBool(cursor, result.payload[i].tagArrayBool)) return false;
                        }
                        break;
                    }
                    case Types::Hex: {
                        for (u64 i = 0; i < count; i++) {
                            new (&result.payload[i].tagArrayHex) TagArrayHex;
                            result.payload[i].type = Types::ArrayHex;
                            //Pass head block of each array.
                            ++cursor;
                            if (!readArrayHex(cursor, result.payload[i].tagArrayHex)) return false;
                        }
                        break;
                    }
                    case Types::Float: {
                        for (u64 i = 0; i < count; i++) {
                            new (&result.payload[i].tagArrayFloat) TagArrayFloat;
                            result.payload[i].type = Types::ArrayFloat;
                            //Pass head block of each array.
                            ++cursor;
                            if (!readArrayFloat(cursor, result.payload[i].tagArrayFloat)) return false;
                        }
                        break;
                    }
                    case Types::Double: {
                        for (u64 i = 0; i < count; i++) {
                            new (&result.payload[i].tagArrayDouble) TagArrayDouble;
                            result.payload[i].type = Types::ArrayDouble;
                            //Pass head block of each array.
                            ++cursor;
                            if (!readArrayDouble(cursor, result.payload[i].tagArrayDouble)) return false;
                        }
                        break;
                    }
                    case Types::Raw: {
                        for (u64 i = 0; i < count; i++) {
                            new (&result.payload[i].tagArrayRaw) TagArrayRaw;
                            result.payload[i].type = Types::ArrayRaw;
                            //Pass head block of each array.
                            ++cursor;
                            if (!readArrayRaw(cursor, result.payload[i].tagArrayRaw)) return false;
                        }
                        break;
                    }
                    default: {
                        pushError(format("Invalid second type {} at pos {}!", static_cast<u8>(type), cursor.currentOffset() - 1));
                        return false;
                    }
                }
                break;
            }
            case Types::String: {
                for (u64 i = 0; i < count; i++) {
                    new (&result.payload[i].tagString) TagString;
                    result.payload[i].type = Types::String;
                    if (!readString(cursor, result.payload[i].tagString)) return false;
                }
                break;
            }
            default: { //ObjectEnd, etc.
                pushError(format("Invalid second type {} at pos {}!", static_cast<u8>(type), cursor.currentOffset() - 1));
                return false;
            }
        }
        return true;
    }

    inline constexpr const char* EOF_ERROR = "Failed to read data, EOF reached!";

    [[nodiscard]] inline bool readString(FileReader& cursor, TagString& result) noexcept {
        auto byteLength = readUVarInt(cursor);
        vector<c8> temp(byteLength);
        u64 actualByteLength = cursor.getContent(reinterpret_cast<u8*>(temp.data()), byteLength);
        if (actualByteLength < byteLength) {
            pushError(EOF_ERROR);
            return false;
        }
        result.payload = u8string(temp.begin(), temp.end());
        return true;
    }

    inline void readRaw(FileReader& cursor, TagRaw& result) noexcept {
        result.payload = *cursor;
        ++cursor;
    }

    [[nodiscard]] inline bool readArrayBool(FileReader& cursor, TagArrayBool& result) noexcept {
        auto count = readUVarInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(result.payload.data(), count);
        if (actualLength < count) {
            pushError(EOF_ERROR);
            return false;
        }
        //Modern compilers will auto vectorize this.
        for (u64 i = 0; i < count; i++) result.payload[i] &= 0x01;
        return true;
    }

    [[nodiscard]] inline bool readArrayHex(FileReader& cursor, TagArrayHex& result) noexcept {
        auto count = readUVarInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(result.payload.data(), count);
        if (actualLength < count) {
            pushError(EOF_ERROR);
            return false;
        }
        //Modern compilers will auto vectorize this.
        for (u64 i = 0; i < count; i++) result.payload[i] &= 0x0F;
        return true;
    }

    [[nodiscard]] inline bool readArrayFloat(FileReader& cursor, TagArrayFloat& result) noexcept {
        auto count = readUVarInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(reinterpret_cast<u8*>(result.payload.data()), count * 4);
        if (actualLength < count * 4) {
            pushError(EOF_ERROR);
            return false;
        }
        return true;
    }

    [[nodiscard]] inline bool readArrayDouble(FileReader& cursor, TagArrayDouble& result) noexcept {
        auto count = readUVarInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(reinterpret_cast<u8*>(result.payload.data()), count * 8);
        if (actualLength < count * 8) {
            pushError(EOF_ERROR);
            return false;
        }
        return true;
    }

    [[nodiscard]] inline bool readArrayRaw(FileReader& cursor, TagArrayRaw& result) noexcept {
        auto count = readUVarInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(result.payload.data(), count);
        if (actualLength < count) {
            pushError(EOF_ERROR);
            return false;
        }
        return true;
    }
}
#pragma once
#include <array>
#include <format>
#include <string>
#include <variant>
#include <vector>

#include "type.hpp"
#include "iotype.hpp"

namespace NBT::IO {
    typedef char8_t c8;
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using namespace NBT::TypeNS;
    using std::vector, std::array, std::string, std::variant, std::bit_cast, std::to_string, std::format, NBT::VarTextNS::readStr, NBT::VarIntNS::readUInt, NBT::TypeNS::SupportedContainers;

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

    inline thread_local vector<string> errors;

    //Error vector copied on-purpose.
    [[nodiscard]] inline vector<string> getErrors() noexcept { return vector(errors); }

    template<SupportedContainers T>
    [[nodiscard]] inline bool read(const char* path, T& result) noexcept {
        errors.clear();
        FileReader cursor(path);
        if (!cursor) {
            errors.push_back(string("File failed to open: ") + path);
            return false;
        }
        //Caveat: We can actually treat the top level tags as they are in a embedded `Object` tag.
        TagObject topLevel(12914);
        if (readObject(cursor, topLevel, true)) {
            result = topLevel.payload;
            return true;
        }
        else return false;
    }

    [[nodiscard]] inline NBTFileInfo getFileInfo(const char* path) noexcept {
        NBTFileInfo result;
        FileReader cursor(path);
        result.validFile = !!cursor;
        result.compressed = cursor.compressed();
        result.fileSize = cursor.getFileSize();
        return result;
    }

    [[nodiscard]] inline bool readObject(FileReader& cursor, TagObject& result, bool topLevel) noexcept {
        auto type = getType(*cursor);
        while (topLevel ? !!cursor : type != Types::ObjectEnd) {
            switch (type) {
                case Types::Object: {
                    ++cursor;
                    TagObject temp(12914);
                    string name = readStr(cursor);
                    if (readObject(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::IVarInt: {
                    ++cursor;
                    TagIVarInt temp(12914);
                    string name = readStr(cursor);
                    readIVarInt(cursor, temp);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::UVarInt: {
                    ++cursor;
                    TagUVarInt temp(12914);
                    string name = readStr(cursor);
                    readUVarInt(cursor, temp);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::Bool: {
                    auto cv = *cursor;
                    ++cursor;
                    TagBool temp(12914);
                    string name = readStr(cursor);
                    readBool(cursor, temp, cv);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::Hex: {
                    auto cv = *cursor;
                    ++cursor;
                    TagHex temp(12914);
                    string name = readStr(cursor);
                    readHex(cursor, temp, cv);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::Float: {
                    ++cursor;
                    TagFloat temp(12914);
                    string name = readStr(cursor);
                    readFloat(cursor, temp);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::Double: {
                    ++cursor;
                    TagDouble temp(12914);
                    string name = readStr(cursor);
                    readDouble(cursor, temp);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::Array: {
                    auto secType = getSecondType(*cursor);
                    ++cursor;
                    TagArray temp(12914);
                    string name = readStr(cursor);
                    if (readArray(cursor, temp, secType)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::String: {
                    ++cursor;
                    TagString temp(12914);
                    string name = readStr(cursor);
                    if (readString(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::Raw: {
                    ++cursor;
                    TagRaw temp(12914);
                    string name = readStr(cursor);
                    readRaw(cursor, temp);
                    result.payload.emplace(name, move(temp));
                    break;
                }
                case Types::ArrayBool: {
                    ++cursor;
                    TagArrayBool temp(12914);
                    string name = readStr(cursor);
                    if (readArrayBool(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayHex: {
                    ++cursor;
                    TagArrayHex temp(12914);
                    string name = readStr(cursor);
                    if (readArrayHex(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayFloat: {
                    ++cursor;
                    TagArrayFloat temp(12914);
                    string name = readStr(cursor);
                    if (readArrayFloat(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayDouble: {
                    ++cursor;
                    TagArrayDouble temp(12914);
                    string name = readStr(cursor);
                    if (readArrayDouble(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayRaw: {
                    ++cursor;
                    TagArrayRaw temp(12914);
                    string name = readStr(cursor);
                    if (readArrayRaw(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                default: {
                    errors.push_back(format("Invalid type ID {} in object at pos {}", static_cast<u8>(type), cursor.currentOffset()));
                    return false;
                }
            }
            if (!cursor) break;
            type = getType(*cursor);
        }
        if (!topLevel) ++cursor;
        return true;
    }

    inline void readIVarInt(FileReader& cursor, TagIVarInt& result) noexcept { result.payload = VarIntNS::readIVarInt(cursor); }

    inline void readUVarInt(FileReader& cursor, TagUVarInt& result) noexcept { result.payload = VarIntNS::readUVarInt(cursor); }

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
        auto count = readUInt(cursor);
        result.payload.resize(count);
        switch (type) {
            case Types::Object: {
                for (u64 i = 0; i < count; i++) {
                    if (readObject(cursor, result.payload[i].tagObject)) result.payload[i].type = Types::Object;
                    else return false;
                }
                break;
            }
            case Types::IVarInt: {
                for (u64 i = 0; i < count; i++) {
                    readIVarInt(cursor, result.payload[i].tagIVarInt);
                    result.payload[i].type = Types::IVarInt;
                }
                break;
            }
            case Types::UVarInt: {
                for (u64 i = 0; i < count; i++) {
                    readUVarInt(cursor, result.payload[i].tagUVarInt);
                    result.payload[i].type = Types::UVarInt;
                }
                break;
            }
            case Types::Array: {
                for (u64 i = 0; i < count; i++) {
                    auto type = getSecondType(*cursor);
                    ++cursor;
                    if (readArray(cursor, result.payload[i].tagArray, type)) result.payload[i].type = Types::Array;
                    else return false;
                }
                break;
            }
            default: { //ObjectEnd, etc.
                errors.push_back(format("Invalid second type {} at pos {}", static_cast<u8>(type), cursor.currentOffset() - 1));
                return false;
            }
        }
        return true;
    }

    inline constexpr const char* EOF_ERROR = "Failed to read string, EOF reached!";

    [[nodiscard]] inline bool readString(FileReader& cursor, TagString& result) noexcept {
        auto byteLength = readUInt(cursor);
        vector<c8> temp(byteLength);
        u64 actualByteLength = cursor.getContent(reinterpret_cast<u8*>(temp.data()), byteLength);
        if (actualByteLength < byteLength) {
            errors.push_back(EOF_ERROR);
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
        auto count = readUInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(result.payload.data(), count);
        if (actualLength < count) {
            errors.push_back(EOF_ERROR);
            return false;
        }
        //Modern compilers will auto vectorize this.
        for (u64 i = 0; i < count; i++) result.payload[i] &= 0x01;
        return true;
    }

    [[nodiscard]] inline bool readArrayHex(FileReader& cursor, TagArrayHex& result) noexcept {
        auto count = readUInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(result.payload.data(), count);
        if (actualLength < count) {
            errors.push_back(EOF_ERROR);
            return false;
        }
        //Modern compilers will auto vectorize this.
        for (u64 i = 0; i < count; i++) result.payload[i] &= 0x0F;
        return true;
    }

    [[nodiscard]] inline bool readArrayFloat(FileReader& cursor, TagArrayFloat& result) noexcept {
        auto count = readUInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(reinterpret_cast<u8*>(result.payload.data()), count * 4);
        if (actualLength < count * 4) {
            errors.push_back(EOF_ERROR);
            return false;
        }
        return true;
    }

    [[nodiscard]] inline bool readArrayDouble(FileReader& cursor, TagArrayDouble& result) noexcept {
        auto count = readUInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(reinterpret_cast<u8*>(result.payload.data()), count * 8);
        if (actualLength < count * 8) {
            errors.push_back(EOF_ERROR);
            return false;
        }
        return true;
    }

    [[nodiscard]] inline bool readArrayRaw(FileReader& cursor, TagArrayRaw& result) noexcept {
        auto count = readUInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(result.payload.data(), count);
        if (actualLength < count) {
            errors.push_back(EOF_ERROR);
            return false;
        }
        return true;
    }
}
#pragma once
#include <array>
#include <format>
#include <istream>
#include <span>
#include <string>
#include <vector>

#include "adapters.hpp"
#include "auxiliary.hpp"
#include "error.hpp"
#include "FileReader.hpp"
#include "mapLike.hpp"
#include "types.hpp"

namespace NBT::IO {
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using namespace NBT::Type;
    using std::vector, std::array, std::span, std::string, std::istream, std::move, std::bit_cast, std::to_string, std::format, NBT::Aux::readVarText, NBT::Aux::readIVarInt, NBT::Aux::readUVarInt, NBT::Error::clearErrors, NBT::Error::pushError, NBT::MapLike::MapLike;

    template<Readable S, typename P> requires MapLike<P>
    [[nodiscard]] inline bool readObject     (FileReader<S>&, TagObject<P>&  , bool topLevel = false) noexcept;
    template<Readable S>
                  inline void readIVarInt    (FileReader<S>&, TagIVarInt&      )                        noexcept;
    template<Readable S>
                  inline void readUVarInt    (FileReader<S>&, TagUVarInt&      )                        noexcept;
    template<Readable S>
                  inline void readBool       (FileReader<S>&, TagBool&         , u8)                    noexcept;
    template<Readable S>
                  inline void readHex        (FileReader<S>&, TagHex&          , u8)                    noexcept;
    template<Readable S>
                  inline void readFloat      (FileReader<S>&, TagFloat&        )                        noexcept;
    template<Readable S>
                  inline void readDouble     (FileReader<S>&, TagDouble&       )                        noexcept;
    template<Readable S, typename P> requires MapLike<P>
    [[nodiscard]] inline bool readArray      (FileReader<S>&, TagArray<P>&   , const Types)           noexcept;
    template<Readable S>
    [[nodiscard]] inline bool readString     (FileReader<S>&, TagString&       )                        noexcept;
    template<Readable S>
                  inline void readRaw        (FileReader<S>&, TagRaw&          )                        noexcept;
    template<Readable S>
    [[nodiscard]] inline bool readArrayBool  (FileReader<S>&, TagArrayBool&    )                        noexcept;
    template<Readable S>
    [[nodiscard]] inline bool readArrayHex   (FileReader<S>&, TagArrayHex&     )                        noexcept;
    template<Readable S>
    [[nodiscard]] inline bool readArrayFloat (FileReader<S>&, TagArrayFloat&   )                        noexcept;
    template<Readable S>
    [[nodiscard]] inline bool readArrayDouble(FileReader<S>&, TagArrayDouble&  )                        noexcept;
    template<Readable S>
    [[nodiscard]] inline bool readArrayRaw   (FileReader<S>&, TagArrayRaw&     )                        noexcept;

    struct NBTFileInfo {
        u64 fileSize{0};
        bool validFile{false}, compressed{false};
    };

    template<typename P, Readable S> requires MapLike<P>
    [[nodiscard]] inline bool readStream(S& source, typename P::template map<string, Tag<P>>& result) noexcept {
        clearErrors();
        FileReader<S> cursor(source);
        result.clear();
        if (!cursor) return false;
        if (cursor.empty()) {
            cursor.close();
            return true;
        }
        TagObject<P> topLevel;
        if (readObject(cursor, topLevel, true)) {
            result = move(topLevel.payload);
            cursor.close();
            return true;
        }
        cursor.close();
        return false;
    }

    template <typename P> requires MapLike<P>
    [[nodiscard]] inline bool readStream(istream& s, typename P::template map<string, Tag<P>>& result) noexcept {
        StdIn adapter(s);
        return readStream<P>(adapter, result);
    }

    template <typename P> requires MapLike<P>
    [[nodiscard]] inline bool readData(const span<const u8> data, typename P::template map<string, Tag<P>>& result) noexcept {
        SpanIn adapter(data);
        return readStream<P>(adapter, result);
    }

    template<Readable S>
    [[nodiscard]] inline NBTFileInfo getFileInfo(S& source) noexcept {
        clearErrors();
        FileReader<S> cursor(source);
        return {
            .fileSize = cursor.getFileSize(),
            .validFile = !!cursor,
            .compressed = cursor.compressed()
        };
    }

    template<Readable S, typename P> requires MapLike<P>
    [[nodiscard]] inline bool readObject(FileReader<S>& cursor, TagObject<P>& result, bool topLevel) noexcept {
        auto type = getType(*cursor);
        while (topLevel ? !!cursor : type != Types::ObjectEnd) {
            switch (type) {
                case Types::Object: {
                    ++cursor;
                    TagObject<P> temp;
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
                    TagArray<P> temp;
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

    template<Readable S> inline void readIVarInt(FileReader<S>& cursor, TagIVarInt& result) noexcept { result.payload = readIVarInt(cursor); }

    template<Readable S> inline void readUVarInt(FileReader<S>& cursor, TagUVarInt& result) noexcept { result.payload = readUVarInt(cursor); }

    template<Readable S> inline void readBool(FileReader<S>&, TagBool& result, u8 cv) noexcept { result.payload = cv & 0x01; }

    template<Readable S> inline void readHex(FileReader<S>&, TagHex& result, u8 cv) noexcept { result.payload = cv & 0x0F; }

    template<Readable S>
    inline void readFloat(FileReader<S>& cursor, TagFloat& result) noexcept {
        u32 temp = 0;
        for (u8 i = 0; i < sizeof(float); i++) {
            auto byte = *cursor;
            temp |= static_cast<u32>(byte) << i * 8;
            ++cursor;
        }
        result.payload = bit_cast<float>(temp);
    }

    template<Readable S>
    inline void readDouble(FileReader<S>& cursor, TagDouble& result) noexcept {
        u64 temp = 0;
        for (u8 i = 0; i < sizeof(double); i++) {
            auto byte = *cursor;
            temp |= static_cast<u64>(byte) << i * 8;
            ++cursor;
        }
        result.payload = bit_cast<double>(temp);
    }

    template<Readable S, typename P> requires MapLike<P>
    [[nodiscard]] inline bool readArray(FileReader<S>& cursor, TagArray<P>& result, const Types type) noexcept {
        auto count = readUVarInt(cursor);
        result.payload.resize(count);
        switch (type) {
            case Types::Object: {
                for (u64 i = 0; i < count; i++) {
                    new (&result.payload[i].tagObject) TagObject<P>;
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
                            new (&result.payload[i].tagArray) TagArray<P>;
                            result.payload[i].type = Types::Array;
                            ++cursor;
                            if (!readArray(cursor, result.payload[i].tagArray, type)) return false;
                        }
                        break;
                    }
                    case Types::Bool: {
                        for(u64 i = 0; i < count; i++) {
                            new (&result.payload[i].tagArrayBool) TagArrayBool;
                            result.payload[i].type = Types::ArrayBool;
                            ++cursor;
                            if (!readArrayBool(cursor, result.payload[i].tagArrayBool)) return false;
                        }
                        break;
                    }
                    case Types::Hex: {
                        for (u64 i = 0; i < count; i++) {
                            new (&result.payload[i].tagArrayHex) TagArrayHex;
                            result.payload[i].type = Types::ArrayHex;
                            ++cursor;
                            if (!readArrayHex(cursor, result.payload[i].tagArrayHex)) return false;
                        }
                        break;
                    }
                    case Types::Float: {
                        for (u64 i = 0; i < count; i++) {
                            new (&result.payload[i].tagArrayFloat) TagArrayFloat;
                            result.payload[i].type = Types::ArrayFloat;
                            ++cursor;
                            if (!readArrayFloat(cursor, result.payload[i].tagArrayFloat)) return false;
                        }
                        break;
                    }
                    case Types::Double: {
                        for (u64 i = 0; i < count; i++) {
                            new (&result.payload[i].tagArrayDouble) TagArrayDouble;
                            result.payload[i].type = Types::ArrayDouble;
                            ++cursor;
                            if (!readArrayDouble(cursor, result.payload[i].tagArrayDouble)) return false;
                        }
                        break;
                    }
                    case Types::Raw: {
                        for (u64 i = 0; i < count; i++) {
                            new (&result.payload[i].tagArrayRaw) TagArrayRaw;
                            result.payload[i].type = Types::ArrayRaw;
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
            default: {
                pushError(format("Invalid second type {} at pos {}!", static_cast<u8>(type), cursor.currentOffset() - 1));
                return false;
            }
        }
        return true;
    }

    inline constexpr const char* EOF_ERROR = "Failed to read data, EOF reached!";

    template<Readable S>
    [[nodiscard]] inline bool readString(FileReader<S>& cursor, TagString& result) noexcept {
        auto byteLength = readUVarInt(cursor);
        vector<u8> temp(byteLength);
        u64 actualByteLength = cursor.getContent(reinterpret_cast<u8*>(temp.data()), byteLength);
        if (actualByteLength < byteLength) { pushError(EOF_ERROR); return false; }
        result.payload = string(temp.begin(), temp.end());
        return true;
    }

    template<Readable S>
    inline void readRaw(FileReader<S>& cursor, TagRaw& result) noexcept { result.payload = *cursor; ++cursor; }

    template<Readable S>
    [[nodiscard]] inline bool readArrayBool(FileReader<S>& cursor, TagArrayBool& result) noexcept {
        auto count = readUVarInt(cursor);
        result.payload.resize(count);
        if (cursor.getContent(result.payload.data(), count) < count) { pushError(EOF_ERROR); return false; }
        for (u64 i = 0; i < count; i++) result.payload[i] &= 0x01;
        return true;
    }

    template<Readable S>
    [[nodiscard]] inline bool readArrayHex(FileReader<S>& cursor, TagArrayHex& result) noexcept {
        auto count = readUVarInt(cursor);
        result.payload.resize(count);
        if (cursor.getContent(result.payload.data(), count) < count) { pushError(EOF_ERROR); return false; }
        for (u64 i = 0; i < count; i++) result.payload[i] &= 0x0F;
        return true;
    }

    template<Readable S>
    [[nodiscard]] inline bool readArrayFloat(FileReader<S>& cursor, TagArrayFloat& result) noexcept {
        auto count = readUVarInt(cursor);
        result.payload.resize(count);
        if (cursor.getContent(reinterpret_cast<u8*>(result.payload.data()), count * 4) < count * 4) { pushError(EOF_ERROR); return false; }
        return true;
    }

    template<Readable S>
    [[nodiscard]] inline bool readArrayDouble(FileReader<S>& cursor, TagArrayDouble& result) noexcept {
        auto count = readUVarInt(cursor);
        result.payload.resize(count);
        if (cursor.getContent(reinterpret_cast<u8*>(result.payload.data()), count * 8) < count * 8) { pushError(EOF_ERROR); return false; }
        return true;
    }

    template<Readable S>
    [[nodiscard]] inline bool readArrayRaw(FileReader<S>& cursor, TagArrayRaw& result) noexcept {
        auto count = readUVarInt(cursor);
        result.payload.resize(count);
        if (cursor.getContent(result.payload.data(), count) < count) { pushError(EOF_ERROR); return false; }
        return true;
    }
}
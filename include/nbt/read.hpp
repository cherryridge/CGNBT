#pragma once
#include <vector>
#include <string>
#include <array>
#include <filesystem>
#include <variant>

#include "type/type.hpp"
#include "type/iotype.hpp"
#include "readGZip.hpp" // IWYU pragma: export

namespace NBT::IO {
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using namespace NBT::TypeNS;
    using std::vector, std::array, std::string, std::filesystem::path, std::variant, std::bit_cast, NBT::VarTextNS::readStr, std::to_string, NBT::VarIntNS::UReadInt;

    //Doesn't move the cursor!
    inline constexpr Types getType(u8 head) noexcept {
        if ((head & 0xF0) == static_cast<u8>(Types::Array) << 4) switch (static_cast<TypesN>(head & 0x0F)) {
            case TypesN::Bool:   return Types::ArrayBool;
            case TypesN::Hex:    return Types::ArrayHex;
            case TypesN::Float:  return Types::ArrayFloat;
            case TypesN::Double: return Types::ArrayDouble;
            case TypesN::Utf8:   return Types::ArrayUtf8;
            case TypesN::Raw:    return Types::ArrayRaw;
            default:             return Types::Array;
        }
        return static_cast<Types>((head & 0xF0) >> 4);
    }
    //Doesn't move the cursor!
    inline constexpr TypesN getTypeN(u8 head) noexcept {
        if ((head & 0xF0) == static_cast<u8>(Types::Array) << 4) switch (static_cast<TypesN>(head & 0x0F)) {
            case TypesN::Bool:   return TypesN::ArrayBool;
            case TypesN::Hex:    return TypesN::ArrayHex;
            case TypesN::Float:  return TypesN::ArrayFloat;
            case TypesN::Double: return TypesN::ArrayDouble;
            case TypesN::Utf8:   return TypesN::ArrayUtf8;
            case TypesN::Raw:    return TypesN::ArrayRaw;
            default:             return TypesN::Array;
        }
        return static_cast<TypesN>((head & 0xF0) >> 4);
    }
    //Doesn't move the cursor!
    inline constexpr TypesN getSecondType(u8 head) noexcept { return static_cast<TypesN>(head & 0x0F); }


    inline bool readObject(FileCursor& cursor, TagObject& result, bool topLevel = false) noexcept;
    inline bool readIVarInt(FileCursor& cursor, TagIVarInt& result) noexcept;
    inline bool readUVarInt(FileCursor& cursor, TagUVarInt& result) noexcept;
    inline bool readBool(FileCursor& cursor, TagBool& result) noexcept;
    inline bool readHex(FileCursor& cursor, TagHex& result) noexcept;
    inline bool readFloat(FileCursor& cursor, TagFloat& result) noexcept;
    inline bool readDouble(FileCursor& cursor, TagDouble& result) noexcept;
    inline bool readArray(FileCursor& cursor, const TypesN type, TagArray& result) noexcept;
    inline bool readUtf8(FileCursor& cursor, TagUtf8& result) noexcept;
    inline bool readRaw(FileCursor& cursor, TagRaw& result) noexcept;
    inline bool readArrayBool(FileCursor& cursor, TagArrayBool& result) noexcept;
    inline bool readArrayHex(FileCursor& cursor, TagArrayHex& result) noexcept;
    inline bool readArrayFloat(FileCursor& cursor, TagArrayFloat& result) noexcept;
    inline bool readArrayDouble(FileCursor& cursor, TagArrayDouble& result) noexcept;
    inline bool readArrayUtf8(FileCursor& cursor, TagArrayUtf8& result) noexcept;
    inline bool readArrayRaw(FileCursor& cursor, TagArrayRaw& result) noexcept;

    inline vector<string> readErrors;

    inline variant<unordered_map<string, Tag>, vector<string>> read(const path& _path) noexcept {
        FileCursor cursor(_path);
        if (!cursor.active) {
            vector<string> error;
            error.push_back("File failed to open: " + _path.string());
            return error;
        }
        array<u8, 2> header{};
        cursor.getContent(0, 2, header.data());
        //Not GZipped.
        if (header[0] == 'c' && header[1] == 'g') cursor += 2;
        else { //GZipped
            //todo
        }
        //Caveat: We can actually treat the top level tags as they are in a embedded `Object` tag.
        TagObject topLevel(12914);
        if (readObject(cursor, topLevel, true)) return topLevel.payload;
        else {
            vector<string> errors = readErrors;
            readErrors.clear();
            return errors;
        }
    }

    inline bool readObject(FileCursor& cursor, TagObject& result, bool topLevel) noexcept {
        auto type = getType(*cursor);
        while (topLevel ? !cursor.eof() : type != Types::ObjectEnd) {
            //Warning: We assume that every reading operation will put cursor at the start of next entry (which is heading block in `Object`s), so we will only skip one byte (which is this entry's heading block).
            ++cursor;
            switch (type) {
                case Types::Object: {
                    TagObject temp(12914);
                    string name = readStr(cursor);
                    if (readObject(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::IVarInt: {
                    TagIVarInt temp(12914);
                    string name = readStr(cursor);
                    if (readIVarInt(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::UVarInt: {
                    TagUVarInt temp(12914);
                    string name = readStr(cursor);
                    if (readUVarInt(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::Bool: {
                    TagBool temp(12914);
                    if (readBool(cursor, temp)) {
                        string name = readStr(cursor);
                        result.payload.emplace(name, move(temp));
                    }
                    else return false;
                    break;
                }
                case Types::Hex: {
                    TagHex temp(12914);
                    if (readHex(cursor, temp)) {
                        string name = readStr(cursor);
                        result.payload.emplace(name, move(temp));
                    }
                    else return false;
                    break;
                }
                case Types::Float: {
                    TagFloat temp(12914);
                    string name = readStr(cursor);
                    if (readFloat(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::Double: {
                    TagDouble temp(12914);
                    string name = readStr(cursor);
                    if (readDouble(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::Array: {
                    TagArray temp(12914);
                    --cursor;
                    auto secType = getSecondType(*cursor);
                    ++cursor;
                    string name = readStr(cursor);
                    if (readArray(cursor, secType, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::Utf8: {
                    TagUtf8 temp(12914);
                    string name = readStr(cursor);
                    if (readUtf8(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::Raw: {
                    TagRaw temp(12914);
                    string name = readStr(cursor);
                    if (readRaw(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayBool: {
                    TagArrayBool temp(12914);
                    string name = readStr(cursor);
                    if (readArrayBool(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayHex: {
                    TagArrayHex temp(12914);
                    string name = readStr(cursor);
                    if (readArrayHex(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayFloat: {
                    TagArrayFloat temp(12914);
                    string name = readStr(cursor);
                    if (readArrayFloat(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayDouble: {
                    TagArrayDouble temp(12914);
                    string name = readStr(cursor);
                    if (readArrayDouble(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayUtf8: {
                    TagArrayUtf8 temp(12914);
                    string name = readStr(cursor);
                    if (readArrayUtf8(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                case Types::ArrayRaw: {
                    TagArrayRaw temp(12914);
                    string name = readStr(cursor);
                    if (readArrayRaw(cursor, temp)) result.payload.emplace(name, move(temp));
                    else return false;
                    break;
                }
                default: {
                    string temp("Invalid type ID in object at pos ");
                    temp += to_string(cursor.current() - 1);
                    temp += ": ";
                    temp += to_string(static_cast<u8>(type));
                    readErrors.push_back(temp);
                    return false;
                }
            }
            if (cursor.eof()) break;
            type = getType(*cursor);
        }
        if (!topLevel) ++cursor;
        return true;
    }

    inline bool readIVarInt(FileCursor& cursor, TagIVarInt& result) noexcept {
        result.payload = VarIntNS::IRead(cursor);
        return true;
    }

    inline bool readUVarInt(FileCursor& cursor, TagUVarInt& result) noexcept {
        result.payload = VarIntNS::URead(cursor);
        return true;
    }

    inline bool readBool(FileCursor& cursor, TagBool& result) noexcept {
        //Move cursor backwards to get convenient payload
        --cursor;
        result.payload = *cursor & 0x0F;
        ++cursor;
        return true;
    }

    inline bool readHex(FileCursor& cursor, TagHex& result) noexcept {
        //Move cursor backwards to get convenient payload
        --cursor;
        result.payload = *cursor & 0x0F;
        ++cursor;
        return true;
    }

    inline bool readFloat(FileCursor& cursor, TagFloat& result) noexcept {
        u32 temp = 0;
        for (u8 i = 0; i < sizeof(float); i++) {
            auto byte = *cursor;
            temp |= static_cast<u32>(byte) << i * 8;
            ++cursor;
        }
        result.payload = bit_cast<float>(temp);
        return true;
    }

    inline bool readDouble(FileCursor& cursor, TagDouble& result) noexcept {
        u64 temp = 0;
        for (u8 i = 0; i < sizeof(double); i++) {
            auto byte = *cursor;
            temp |= static_cast<u64>(byte) << i * 8;
            ++cursor;
        }
        result.payload = bit_cast<double>(temp);
        return true;
    }

    inline bool readArray(FileCursor& cursor, const TypesN type, TagArray& result) noexcept {
        auto count = UReadInt(cursor);
        result.payload.resize(count);
        switch (type) {
            case TypesN::Object: {
                for (u64 i = 0; i < count; i++) {
                    readObject(cursor, result.payload[i].tagObject);
                    result.payload[i].type = TypesN::Object;
                }
                break;
            }
            case TypesN::IVarInt: {
                for (u64 i = 0; i < count; i++) {
                    readIVarInt(cursor, result.payload[i].tagIVarInt);
                    result.payload[i].type = TypesN::IVarInt;
                }
                break;
            }
            case TypesN::UVarInt: {
                for (u64 i = 0; i < count; i++) {
                    readUVarInt(cursor, result.payload[i].tagUVarInt);
                    result.payload[i].type = TypesN::UVarInt;
                }
                break;
            }
            case TypesN::Array: {
                for (u64 i = 0; i < count; i++) {
                    auto type = getSecondType(*cursor);
                    ++cursor;
                    readArray(cursor, type, result.payload[i].tagArray);
                    result.payload[i].type = TypesN::Array;
                }
                break;
            }
            default: { //ObjectEnd, etc.
                string temp("Invalid second type ");
                temp += to_string(static_cast<u8>(type));
                temp += " at pos ";
                temp += to_string(cursor.current() - 1);
                readErrors.push_back(temp);
                return false;
            }
        }
        return true;
    }

    inline bool readUtf8(FileCursor& cursor, TagUtf8& result) noexcept {
        array<u8, 4> p{};
        cursor.getContent(cursor.current(), 4, p.data());
        result.payload.fill(0);
        if ((p[0] & 0x80) == 0x00) {
            memcpy(result.payload.data(), p.data(), 1);
            result.length = 1;
            cursor += 1;
        }
        else if ((p[0] & 0xE0) == 0xC0) {
            memcpy(result.payload.data(), p.data(), 2);
            result.length = 2;
            cursor += 2;
        }
        else if ((p[0] & 0xF0) == 0xE0) {
            memcpy(result.payload.data(), p.data(), 3);
            result.length = 3;
            cursor += 3;
        }
        else if ((p[0] & 0xF8) == 0xF0) {
            memcpy(result.payload.data(), p.data(), 4);
            result.length = 4;
            cursor += 4;
        }
        //Invalid UTF-8 sequence
        else return false;
        return true;
    }

    inline bool readRaw(FileCursor& cursor, TagRaw& result) noexcept {
        result.payload = *cursor;
        ++cursor;
        return true;
    }

    inline bool readArrayBool(FileCursor& cursor, TagArrayBool& result) noexcept {
        auto count = UReadInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(cursor.current(), count, result.payload.data());
        if (actualLength < count) return false;
        //We need to get rid of the head anyways, so why not getting rid of the rest as well?
        //todo: SIMD?
        for (u64 i = 0; i < count; i++) result.payload[i] &= 0x01;
        cursor += count;
        return true;
    }

    inline bool readArrayHex(FileCursor& cursor, TagArrayHex& result) noexcept {
        auto count = UReadInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(cursor.current(), count, result.payload.data());
        if (actualLength < count) return false;
        //todo: SIMD?
        for (u64 i = 0; i < count; i++) result.payload[i] &= 0x0F;
        cursor += count;
        return true;
    }

    inline bool readArrayFloat(FileCursor& cursor, TagArrayFloat& result) noexcept {
        auto count = UReadInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(cursor.current(), count * 4, bit_cast<u8*>(result.payload.data()));
        if (actualLength < count * 4) return false;
        cursor += count * 4;
        return true;
    }

    inline bool readArrayDouble(FileCursor& cursor, TagArrayDouble& result) noexcept {
        auto count = UReadInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(cursor.current(), count * 8, bit_cast<u8*>(result.payload.data()));
        if (actualLength < count * 8) return false;
        cursor += count * 8;
        return true;
    }

    inline constexpr u8 ARRAY_UTF8_BUFFER_LENGTH = 16;

    inline bool readArrayUtf8(FileCursor& cursor, TagArrayUtf8& result) noexcept {
        auto count = UReadInt(cursor);
        //todo: Use sliding window to not explode the heap memory
        auto* const p = new c8[count * 4];
        //*CodePoint
        u64 cpByteLength = 0, cpCount = 0, actualLength = cursor.getContent(cursor.current(), count * 4, bit_cast<u8*>(p));
        while (cpByteLength < count * 4) {
            if ((p[cpByteLength] & 0x80) == 0x00) cpByteLength++;
            else if ((p[cpByteLength] & 0xE0) == 0xC0) cpByteLength += 2;
            else if ((p[cpByteLength] & 0xF0) == 0xE0) cpByteLength += 3;
            else if ((p[cpByteLength] & 0xF8) == 0xF0) cpByteLength += 4;
            //Invalid UTF-8 sequence
            else {
                delete[] p;
                return false;
            }
            cpCount++;
            if (cpCount == count) break;
        }
        //It's not possible for `cpCount` to be bigger than `count`.
        if (cpCount < count) {
            delete[] p;
            return false;
        }
        result.payload.resize(cpByteLength);
        memcpy(result.payload.data(), p, cpByteLength);
        result.count = cpCount;
        delete[] p;
        cursor += cpByteLength;
        return true;
    }

    inline bool readArrayRaw(FileCursor& cursor, TagArrayRaw& result) noexcept {
        auto count = UReadInt(cursor);
        result.payload.resize(count);
        u64 actualLength = cursor.getContent(cursor.current(), count, result.payload.data());
        if (actualLength < count) return false;
        cursor += count;
        return true;
    }
}
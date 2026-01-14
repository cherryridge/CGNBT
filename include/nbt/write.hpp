#pragma once
#include <array>
#include <format>
#include <vector>
#include <boost/unordered/unordered_map.hpp>
#include <physfs.h>
#include <zstd.h>

#include "type.hpp"
#include "auxiliary.hpp"
#include "error.hpp"

namespace NBT::IO {
    typedef uint8_t u8;
    typedef uint64_t u64;
    using namespace NBT::Type;
    using std::array, std::vector, std::format, boost::unordered_flat_map, NBT::Aux::writeVarText, NBT::Aux::writeIVarInt, NBT::Aux::writeUVarInt, NBT::Error::clearErrors, NBT::Error::pushError;

    [[nodiscard]] inline bool writeObject(const unordered_flat_map<string, Tag>&, vector<u8>&) noexcept;

    inline void writeIVarInt    (const TagIVarInt&    , vector<u8>&) noexcept;
    inline void writeUVarInt    (const TagUVarInt&    , vector<u8>&) noexcept;
    inline void writeBool       (const TagBool&       , vector<u8>&) noexcept;
    inline void writeHex        (const TagHex&        , vector<u8>&) noexcept;
    inline void writeFloat      (const TagFloat&      , vector<u8>&) noexcept;
    inline void writeDouble     (const TagDouble&     , vector<u8>&) noexcept;

    [[nodiscard]] inline bool writeArray(const TagArray&, vector<u8>&) noexcept;

    inline void writeString     (const TagString&     , vector<u8>&) noexcept;
    inline void writeRaw        (const TagRaw&        , vector<u8>&) noexcept;
    inline void writeArrayBool  (const TagArrayBool&  , vector<u8>&) noexcept;
    inline void writeArrayHex   (const TagArrayHex&   , vector<u8>&) noexcept;
    inline void writeArrayFloat (const TagArrayFloat& , vector<u8>&) noexcept;
    inline void writeArrayDouble(const TagArrayDouble&, vector<u8>&) noexcept;
    inline void writeArrayRaw   (const TagArrayRaw&   , vector<u8>&) noexcept;

    inline static constexpr array<u8, 5> MAGIC = { 'c', 'G', 'n', 'b', 'T' };

    [[nodiscard]] inline bool writeRawData(const unordered_flat_map<string, Tag>& data, vector<u8>& result, bool addMagic = false) noexcept {
        clearErrors();
        if (addMagic) result.insert(result.end(), MAGIC.begin(), MAGIC.end());
        if (writeObject(data, result)) return true;
        return false;
    }

    //If exists, setting `_override` to true will override the file, or it will fail.
    [[nodiscard]] inline bool write(const char* path, const unordered_flat_map<string, Tag>& data, bool _override = false, bool zstd = false, u8 compressionLevel = 3) noexcept {
        if(!_override && PHYSFS_exists(path)) {
            pushError(format("File {} already exists! Use `_override=true` to override it.", path));
            return false;
        }
        vector<u8> result;
        if (writeRawData(data, result, !zstd)) {
            auto* file = PHYSFS_openWrite(path);
            if (file == nullptr) goto fail;
            if (zstd) {
                vector<u8> compressed(ZSTD_compressBound(result.size()));
                const auto sizeOrError = ZSTD_compress(compressed.data(), compressed.size(), result.data(), result.size(), compressionLevel > 22 ? 22 : compressionLevel == 0 ? 1 : compressionLevel);
                if(ZSTD_isError(sizeOrError)) {
                    pushError(format("ZSTD compression error: {}", ZSTD_getErrorName(sizeOrError)));
                    goto fail;
                }
                compressed.resize(sizeOrError);
                if (PHYSFS_writeBytes(file, compressed.data(), compressed.size()) != compressed.size()) {
                    pushError(format("Failed to write all bytes to file {}: {}", path, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
                    goto fail;
                }
            }
            else if (PHYSFS_writeBytes(file, result.data(), result.size()) != result.size()) {
                pushError(format("Failed to write all bytes to file {}: {}", path, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
                goto fail;
            }
            if (!PHYSFS_close(file)) pushError(format("Failed to close file {}: {}", path, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
            return true;
        fail:
            if (!PHYSFS_close(file)) pushError(format("Failed to close file {}: {}", path, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
        }
        return false;
    }

    [[nodiscard]] inline bool writeObject(const unordered_flat_map<string, Tag>& payload, vector<u8>& result) noexcept {
        for(const auto& [key, value] : payload) switch(value.type) {
            case Types::Object: {
                result.push_back(static_cast<u8>(Types::Object) << 4);
                writeVarText(key, result);
                if (!writeObject(value.tagObject.payload, result)) return false;
                result.push_back(static_cast<u8>(Types::ObjectEnd));
                break;
            }
            case Types::IVarInt: {
                result.push_back(static_cast<u8>(Types::IVarInt) << 4);
                writeVarText(key, result);
                writeIVarInt(value.tagIVarInt, result);
                break;
            }
            case Types::UVarInt: {
                result.push_back(static_cast<u8>(Types::UVarInt) << 4);
                writeVarText(key, result);
                writeUVarInt(value.tagUVarInt, result);
                break;
            }
            case Types::Bool: {
                writeBool(value.tagBool, result);
                writeVarText(key, result);
                break;
            }
            case Types::Hex: {
                writeHex(value.tagHex, result);
                writeVarText(key, result);
                break;
            }
            case Types::Float: {
                result.push_back(static_cast<u8>(Types::Float) << 4);
                writeVarText(key, result);
                writeFloat(value.tagFloat, result);
                break;
            }
            case Types::Double: {
                result.push_back(static_cast<u8>(Types::Double) << 4);
                writeVarText(key, result);
                writeDouble(value.tagDouble, result);
                break;
            }
            case Types::Array: {
                result.push_back((static_cast<u8>(Types::Array) << 4) | static_cast<u8>(getOriginalType(value.tagArray.payload[0].type)));
                writeVarText(key, result);
                if (!writeArray(value.tagArray, result)) return false;
                break;
            }
            case Types::String: {
                result.push_back(static_cast<u8>(Types::String) << 4);
                writeVarText(key, result);
                writeString(value.tagString, result);
                break;
            }
            case Types::Raw: {
                result.push_back(static_cast<u8>(Types::Raw) << 4);
                writeVarText(key, result);
                writeRaw(value.tagRaw, result);
                break;
            }
            case Types::ArrayBool: {
                result.push_back(static_cast<u8>(Types::Array) << 4 | static_cast<u8>(Types::Bool));
                writeVarText(key, result);
                writeArrayBool(value.tagArrayBool, result);
                break;
            }
            case Types::ArrayHex: {
                result.push_back(static_cast<u8>(Types::Array) << 4 | static_cast<u8>(Types::Hex));
                writeVarText(key, result);
                writeArrayHex(value.tagArrayHex, result);
                break;
            }
            case Types::ArrayFloat: {
                result.push_back(static_cast<u8>(Types::Array) << 4 | static_cast<u8>(Types::Float));
                writeVarText(key, result);
                writeArrayFloat(value.tagArrayFloat, result);
                break;
            }
            case Types::ArrayDouble: {
                result.push_back(static_cast<u8>(Types::Array) << 4 | static_cast<u8>(Types::Double));
                writeVarText(key, result);
                writeArrayDouble(value.tagArrayDouble, result);
                break;
            }
            case Types::ArrayRaw: {
                result.push_back(static_cast<u8>(Types::Array) << 4 | static_cast<u8>(Types::Raw));
                writeVarText(key, result);
                writeArrayRaw(value.tagArrayRaw, result);
                break;
            }
            default: {
                pushError(format("Invalid type ID {} for key {}!", static_cast<u8>(value.type), key));
                return false;
            }
        }
        return true;
    }

    inline void writeIVarInt(const TagIVarInt& data, vector<u8>& result) noexcept { writeIVarInt(data.payload, result); }

    inline void writeUVarInt(const TagUVarInt& data, vector<u8>& result) noexcept { writeUVarInt(data.payload, result); }

    inline void writeBool(const TagBool& data, vector<u8>& result) noexcept { result.push_back((data.payload ? 0x01 : 0x00) | (static_cast<u8>(Types::Bool) << 4)); }

    inline void writeHex(const TagHex& data, vector<u8>& result) noexcept { result.push_back(data.payload & 0x0F | (static_cast<u8>(Types::Hex) << 4)); }

    inline void writeFloat(const TagFloat& data, vector<u8>& result) noexcept {
        const auto* const bytes = reinterpret_cast<const u8*>(&data.payload);
        result.insert(result.end(), bytes, bytes + sizeof(float));
    }

    inline void writeDouble(const TagDouble& data, vector<u8>& result) noexcept {
        const auto* const bytes = reinterpret_cast<const u8*>(&data.payload);
        result.insert(result.end(), bytes, bytes + sizeof(double));
    }

    [[nodiscard]] inline bool writeArray(const TagArray& data, vector<u8>& result) noexcept {
        writeUVarInt(data.payload.size(), result);
        if(!data.payload.empty()) switch (data.payload[0].type) {
            case Types::Object: {
                for(u64 i = 0; i < data.payload.size(); i++) {
                    if (!writeObject(data.payload[i].tagObject.payload, result)) return false;
                    result.push_back(static_cast<u8>(Types::ObjectEnd));
                }
                break;
            }
            case Types::IVarInt: {
                for(u64 i = 0; i < data.payload.size(); i++) writeIVarInt(data.payload[i].tagIVarInt, result);
                break;
            }
            case Types::UVarInt: {
                for(u64 i = 0; i < data.payload.size(); i++) writeUVarInt(data.payload[i].tagUVarInt, result);
                break;
            }
            case Types::Array: {
                for (u64 i = 0; i < data.payload.size(); i++) {
                    result.push_back(static_cast<u8>(Types::Array) << 4 | static_cast<u8>(data.payload[i].tagArray.payload[0].type));
                    if (!writeArray(data.payload[i].tagArray, result)) return false;
                }
                break;
            }
            case Types::String: {
                for(u64 i = 0; i < data.payload.size(); i++) writeString(data.payload[i].tagString, result);
                break;
            }
            case Types::ArrayBool: {
                for (u64 i = 0; i < data.payload.size(); i++) {
                    result.push_back(static_cast<u8>(Types::Array) << 4 | static_cast<u8>(Types::Bool));
                    writeArrayBool(data.payload[i].tagArrayBool, result);
                }
                break;
            }
            case Types::ArrayHex: {
                for (u64 i = 0; i < data.payload.size(); i++) {
                    result.push_back(static_cast<u8>(Types::Array) << 4 | static_cast<u8>(Types::Hex));
                    writeArrayHex(data.payload[i].tagArrayHex, result);
                }
                break;
            }
            case Types::ArrayFloat: {
                for (u64 i = 0; i < data.payload.size(); i++) {
                    result.push_back(static_cast<u8>(Types::Array) << 4 | static_cast<u8>(Types::Float));
                    writeArrayFloat(data.payload[i].tagArrayFloat, result);
                }
                break;
            }
            case Types::ArrayDouble: {
                for (u64 i = 0; i < data.payload.size(); i++) {
                    result.push_back(static_cast<u8>(Types::Array) << 4 | static_cast<u8>(Types::Double));
                    writeArrayDouble(data.payload[i].tagArrayDouble, result);
                }
                break;
            }
            case Types::ArrayRaw: {
                for (u64 i = 0; i < data.payload.size(); i++) {
                    result.push_back(static_cast<u8>(Types::Array) << 4 | static_cast<u8>(Types::Raw));
                    writeArrayRaw(data.payload[i].tagArrayRaw, result);
                }
                break;
            }
            default: {
                pushError(format("Invalid second type {} in array! For fixed-size types (`bool 4`, `hex 5`, `float 6`, `double 7`, `raw 10`), please use dedicated array types.", static_cast<u8>(data.payload[0].type)));
                return false;
            }
        }
        return true;
    }

    inline void writeString(const TagString& data, vector<u8>& result) noexcept {
        writeUVarInt(data.payload.size(), result);
        result.insert(result.end(), data.payload.begin(), data.payload.end());
    }

    inline void writeRaw(const TagRaw& data, vector<u8>& result) noexcept { result.push_back(data.payload); }

    inline void writeArrayBool(const TagArrayBool& data, vector<u8>& result) noexcept {
        writeUVarInt(data.payload.size(), result);
        if (!data.payload.empty()) result.insert(result.end(), data.payload.begin(), data.payload.end());
    }

    inline void writeArrayHex(const TagArrayHex& data, vector<u8>& result) noexcept {
        writeUVarInt(data.payload.size(), result);
        if (!data.payload.empty()) result.insert(result.end(), data.payload.begin(), data.payload.end());
    }

    inline void writeArrayFloat(const TagArrayFloat& data, vector<u8>& result) noexcept {
        writeUVarInt(data.payload.size(), result);
        if (!data.payload.empty()) result.insert(result.end(), reinterpret_cast<const u8*>(data.payload.data()), reinterpret_cast<const u8*>(data.payload.data()) + sizeof(float) * data.payload.size());
    }

    inline void writeArrayDouble(const TagArrayDouble& data, vector<u8>& result) noexcept {
        writeUVarInt(data.payload.size(), result);
        if (!data.payload.empty()) result.insert(result.end(), reinterpret_cast<const u8*>(data.payload.data()), reinterpret_cast<const u8*>(data.payload.data()) + sizeof(double) * data.payload.size());
    }

    inline void writeArrayRaw(const TagArrayRaw& data, vector<u8>& result) noexcept {
        writeUVarInt(data.payload.size(), result);
        if (!data.payload.empty()) result.insert(result.end(), data.payload.begin(), data.payload.end());
    }
}
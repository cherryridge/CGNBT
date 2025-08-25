#pragma once
#include <format>
#include <span>
#include <string>
#include <boost/unordered_map.hpp>

#include "type.hpp"
#include "auxiliary.hpp"
#include "error.hpp"

namespace NBT::IO {
    typedef uint8_t u8;
    using namespace NBT::Type;
    using std::span, std::string, std::format, boost::unordered_flat_map, NBT::Aux::readVarText, NBT::Aux::readIVarInt, NBT::Aux::readUVarInt, NBT::Error::clearErrors, NBT::Error::pushError;

    enum struct RawDataStatus : u8 {
        NoMagic_Raw,
        NoMagic_Zstd,
        Magic_Raw,
        Unknown
    };

    [[nodiscard]] inline bool readRealRawData(span<u8>, unordered_flat_map<string, Tag>&) noexcept;

    [[nodiscard]] inline bool readRawData(span<u8> data, unordered_flat_map<string, Tag>& result, RawDataStatus dataStatus = RawDataStatus::Unknown) noexcept {
        return false; //Not implemented yet because we need completely separate logics
        clearErrors();
        switch (dataStatus) {
            case RawDataStatus::NoMagic_Raw: return readRealRawData(data, result);
            case RawDataStatus::NoMagic_Zstd: {
                //todo: Decompress
                break;
            }
            case RawDataStatus::Magic_Raw: return readRealRawData(data.subspan(5), result);
            case RawDataStatus::Unknown: {
                //todo: Determine the data status by checking the magic and trying to decompress if no magic
                break;
            }
            default: return false;
        }
    }

    [[nodiscard]] inline bool readRealRawData(span<u8> data, unordered_flat_map<string, Tag>& result) noexcept {
        return true;
    }
}
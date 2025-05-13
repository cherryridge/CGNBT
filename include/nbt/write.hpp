#pragma once
#include <fstream>
#include <optional>
#include <filesystem>

#include "type/type.hpp"
#include "writeGZip.hpp" // IWYU pragma: export

namespace NBT::IO {
    typedef uint8_t u8;
    typedef uint64_t u64;
    using namespace NBT::TypeNS;
    using std::ofstream, std::ifstream, std::optional, std::nullopt, std::filesystem::is_regular_file, std::filesystem::file_size, std::filesystem::path;

    inline constexpr u8 NBT_WRITE_GZIP_DATA = 1 << 0;

    //todo
    inline bool write(const path& _path, const TagObject& data, u8 flags = 0) noexcept {
        if (flags & NBT_WRITE_GZIP_DATA) {
            //GZip
        }
        else {

        }
        return true;
    }
}
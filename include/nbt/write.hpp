#pragma once
#include <fstream>
#include <type_traits>

#include "type.hpp"

namespace NBT::IO {
    typedef uint8_t u8;
    typedef uint64_t u64;
    using std::ofstream, std::ifstream, std::streamsize, NBT::TypeNS::Tag, std::ios, std::decay_t, std::enable_if_t, std::is_same_v, NBT::TypeNS::SupportedContainers;

    //If exists, setting `_override` to true will override the file, or it will fail.
    template<SupportedContainers T>
    inline bool write(const T& data, const char* path, bool _override = false, bool zstd = false, u8 compressionLevel = 0) noexcept {
        //Can't override, error
        //if (!_override && ) return false;
        //for (const auto& [key, value] : data) {
        //
        //}
        return true;
    }

    //If exists, detect the variation and append content. If not, create the file.
    template<SupportedContainers T>
    inline bool append(const T& data, const char* _path, bool zstd = false, u8 compressLevel = 0) noexcept {
        //Doesn't exist, will create file
        //if (!exists(_path) || !is_regular_file(_path)) return write(data, _path, false, zstd, compressLevel);
        ifstream stream(_path, ios::in | ios::binary);
        array<u8, 5> header{ 0 };
        stream.read(reinterpret_cast<char*>(header.data()), 5);
        if (header[0] == 'c' && header[1] == 'G' && header[2] == 'n' && header[3] == 'b' && header[4] == 'T') {
            stream.seekg(0, ios::end);

        }
        else if (ZSTD_isFrame(header.data(), 4) || ZSTD_isSkippableFrame(header.data(), 4)) {
            //todo: append to zstd file
            ZSTD_CStream* zstdStream = ZSTD_createCStream();
            //ZSTD_initCStream(zstdStream);
        }
        //Malformed.
        else return false;
    }

    //Get encoded bytes.
    template<SupportedContainers T>
    inline bool encode(const T& data, const vector<u8>& result) noexcept {

        return true;
    }
}
#pragma once
#include <array>
#include <string>
#include <vector>

#include "FileReader.hpp"

namespace NBT::Aux {
    typedef uint8_t u8;
    typedef int64_t i64;
    typedef uint64_t u64;
    using std::array, std::string, std::vector, NBT::IO::FileReader;

    inline static constexpr u8 MSB = 0x80;

    [[nodiscard]] inline string readVarText(FileReader& cursor) noexcept {
        vector<u8> buffer;
        while (!(*cursor & MSB)) {
            buffer.push_back(*cursor);
            ++cursor;
        }
        buffer.push_back(*cursor);
        ++cursor;
        buffer[buffer.size() - 1] -= MSB;
        buffer.push_back('\0');
        return string(reinterpret_cast<const char*>(buffer.data()));
    }

    inline void writeVarText(const string& text, vector<u8>& result) noexcept {
        result.insert(result.end(), text.begin(), text.end());
        result[result.size() - 1] += MSB;
    }

    //Sets cursor to the start of the next byte.
    [[nodiscard]] inline u64 readUVarInt(FileReader& cursor) noexcept {
        u8 length = 1, buffer[10]{ 0 };
        buffer[0] = *cursor;
        while (!(*cursor & MSB)) {
            ++cursor;
            buffer[length] = *cursor;
            length++;
        }
        ++cursor;
        u64 result = 0;
        for (u8 i = 0; i < length; i++) result |= static_cast<u64>(buffer[i] & (MSB - 1)) << (7 * i);
        return result;
    }

    //Sets cursor to the start of the next byte.
    [[nodiscard]] inline i64 readIVarInt(FileReader& cursor) noexcept {
        const auto _unsigned = readUVarInt(cursor);
        return (_unsigned >> 1) ^ -(_unsigned & 1);
    }

    inline void writeUVarInt(u64 data, vector<u8>& result) noexcept {
        array<u8, 10> buffer{ 0 };
        u8 cursor = 0;
        while (data > 0) {
            buffer[cursor] = static_cast<u8>(data & static_cast<i64>(MSB - 1));
            cursor++;
            data >>= 7;
        }
        #pragma warning(suppress: 28020)
        buffer[cursor - 1] += MSB;
        result.insert(result.end(), buffer.begin(), buffer.begin() + cursor);
    }

    inline void writeIVarInt(i64 data, vector<u8>& result) noexcept {
        writeUVarInt((data << 1) ^ (data >> 63), result);
    }
}
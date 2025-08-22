#pragma once
#include <string>
#include <vector>

#include "iotype.hpp"

//Note: This implementation converts `"\0"` to `""` implicitly.
//VarText is not for program storage. Don't use it like normal `string`.

namespace NBT::VarTextNS {
    typedef uint8_t u8;
    typedef uint64_t u64;
    using std::string, std::vector, std::ostream, std::move, NBT::IO::FileReader;

    inline constexpr u8 MSB = 0x80, THRESHOLD = 16;

    struct VarText {
        union {
            u8 local[THRESHOLD];
            const u8* heapStart;
        };
        u64 length;

        explicit VarText() noexcept {
            length = 1;
            local[0] = MSB;
        };

        VarText& operator=(const VarText& copy) noexcept {
            if (this == &copy) goto same;
            length = copy.length;
            if (length <= THRESHOLD) memcpy(local, copy.local, length);
            else {
                heapStart = new u8[length];
                memcpy(const_cast<u8*>(heapStart), copy.heapStart, length);
            }
            same: return *this;
        }

        VarText& operator=(VarText&& _move) noexcept {
            if (this == &_move) goto same;
            length = _move.length;
            if (length <= THRESHOLD) memcpy(local, _move.local, length);
            else {
                heapStart = _move.heapStart;
                _move.heapStart = nullptr;
                //Don't set `move.length` to `0`! We need the indication to check if `heapStart` is `nullptr` in the first place.
            }
            same: return *this;
        }

        VarText(const VarText& copy) noexcept { operator=(copy); }
        VarText(VarText&& _move) noexcept { operator=(move(_move)); }

        //For not initializing the object at all. You should absolutely know what you're doing.
        #pragma warning(suppress: 26495)
        VarText(bool) noexcept {}

        //For pushing in data directly. You should know what you're doing.
        VarText(const u8* input, u64 _length) noexcept {
            length = _length;
            if (length <= THRESHOLD) memcpy(local, input, length);
            else {
                heapStart = new u8[length];
                memcpy(const_cast<u8*>(heapStart), input, length);
            }
        }
        
        //For std::string.
        VarText(const string& str) noexcept {
            u64 _length = str.size();
            length = _length > 0 ? _length : 1;
            if (_length == 0) local[0] = MSB;
            else if (length <= THRESHOLD) {
                memcpy(local, str.c_str(), length);
                local[length - 1] += MSB;
            }
            else {
                heapStart = new u8[length];
                memcpy(const_cast<u8*>(heapStart), str.c_str(), length);
                const_cast<u8*>(heapStart)[length - 1] += MSB;
            }
        }

        //For string literals.
        VarText(const char* input) noexcept {
            //strlen
            u64 _length = 0;
            const char* pc = input;
            while (*pc != NULL) {
                pc++;
                _length++;
            }

            length = _length > 0 ? _length : 1;
            if (_length == 0) local[0] = MSB;
            else if (length <= THRESHOLD) {
                memcpy(local, input, length);
                local[length - 1] += MSB;
            }
            else {
                heapStart = new u8[length];
                memcpy(const_cast<u8*>(heapStart), input, length);
                const_cast<u8*>(heapStart)[length - 1] += MSB;
            }
        }

        //For string literals with known length.
        VarText(const char* input, u64 _length) noexcept {
            length = _length > 0 ? _length : 1;
            if (_length == 0) local[0] = MSB;
            else if (length <= THRESHOLD) {
                memcpy(local, input, length);
                local[length - 1] += MSB;
            }
            else {
                heapStart = new u8[length];
                memcpy(const_cast<u8*>(heapStart), input, length);
                const_cast<u8*>(heapStart)[length - 1] += MSB;
            }
        }

        operator string() const noexcept {
            //Invalid data/Empty string
            if ((length == 1 && local[0] == MSB) || (length > THRESHOLD && heapStart == nullptr)) return string();
            const_cast<u8*>(length > THRESHOLD ? heapStart : local)[length - 1] -= MSB;
            string result(reinterpret_cast<const char*>(length > THRESHOLD ? heapStart : local), length);
            const_cast<u8*>(length > THRESHOLD ? heapStart : local)[length - 1] += MSB;
            return result;
        }

        VarText operator+(const VarText& other) const noexcept {
            bool isThisEmpty = length == 1 && local[0] == MSB, isOtherEmpty = other.length == 1 && other.local[0] == MSB;
            //Invalid data/Both empty string
            if ((length > THRESHOLD && heapStart == nullptr) || (other.length > THRESHOLD && other.heapStart == nullptr) || (isThisEmpty && isOtherEmpty)) return VarText();
            //Empty string
            if (isThisEmpty) return VarText(other);
            if (isOtherEmpty) return VarText(*this);
            u64 _length = length + other.length;
            u8* const p = new u8[_length];
            #pragma warning(suppress: 6386)
            memcpy(p, length > THRESHOLD ? heapStart : local, length);
            p[length - 1] -= MSB;
            memcpy(p + length, other.length > THRESHOLD ? other.heapStart : other.local, other.length);
            VarText result(p, _length);
            delete[] p;
            return result;
        }

        bool operator==(const VarText& other) const noexcept {
            if (length != other.length) return false;
            return memcmp(length > THRESHOLD ? heapStart : local, length > THRESHOLD ? other.heapStart : other.local, length) == 0;
        }

        ~VarText() { if (length > THRESHOLD && heapStart != nullptr) delete[] heapStart; }

        friend ostream& operator<<(ostream& os, const VarText& data) noexcept {
            //Invalid data/Empty string
            if ((data.length == 1 && data.local[0] == MSB) || (data.length > THRESHOLD && data.heapStart == nullptr)) return os;
            char* const p = new char[data.length + 1];
            memcpy(p, data.length > THRESHOLD ? data.heapStart : data.local, data.length);
            p[data.length - 1] -= MSB;
            p[data.length] = NULL;
            os << p;
            delete[] p;
            return os;
        }
    };

    //Sets cursor to the start of the next byte.
    inline VarText read(FileReader& cursor) noexcept {
        vector<u8> buffer;
        do {
            buffer.push_back(*cursor);
            ++cursor;
        } while (!(*cursor & MSB));
        VarText result(buffer.data(), buffer.size());
        return result;
    }

    inline string readStr(FileReader& cursor) noexcept {
        vector<u8> buffer;
        while (!(*cursor & MSB)) {
            buffer.push_back(*cursor);
            ++cursor;
        }
        buffer.push_back(*cursor);
        ++cursor;
        buffer[buffer.size() - 1] -= MSB;
        buffer.push_back('\0');
        return string(reinterpret_cast<char*>(buffer.data()));
    }

    //Sets the pointer to the start of the next byte.
    inline VarText readRaw(u8* input) noexcept {
        if (input == nullptr) return VarText();
        u64 length = 1;
        auto p = input;
        while (!(*input & MSB)) {
            input++;
            length++;
        }
        input++;
        return VarText(p, length);
    }
}

//namespace std {
//    using namespace NBT::VarTextNS;
//    typedef uint64_t u64;
//
//    inline constexpr u64 FNV_OFFSET_BASIS = 14695981039346656037, FNV_PRIME = 1099511628211;
//
//    template<>
//    struct hash<VarText> {
//        u64 operator()(const VarText& input) const noexcept {
//            u64 result = FNV_OFFSET_BASIS;
//            const auto* data = input.length > THRESHOLD ? input.heapStart : input.local;
//            for (u64 i = 0; i < input.length; i++) {
//                result ^= data[i];
//                result *= FNV_PRIME;
//            }
//            result ^= input.length;
//            result *= FNV_PRIME;
//            return result;
//        }
//    };
//}

inline NBT::VarTextNS::VarText operator""_vartext(const char* input, uint64_t length) noexcept {
    return NBT::VarTextNS::VarText(input, length);
}
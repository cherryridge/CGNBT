#pragma once
#include <array>
#include <cassert>
#include <iostream>
#include <limits>
#include <type_traits>

#include "iotype.hpp"

namespace NBT::VarIntNS {
    typedef int8_t i8;
    typedef uint8_t u8;
    typedef int16_t i16;
    typedef uint16_t u16;
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef int64_t i64;
    typedef uint64_t u64;
    using std::ostream, std::array, std::enable_if_t, std::is_same_v, std::move;

    inline constexpr u8 MSB = 0x80;

    template<typename T>
    concept Uint = is_same_v<T, u8> || is_same_v<T, u16> || is_same_v<T, u32> || is_same_v<T, u64>;
    template<typename T>
    concept Sint = is_same_v<T, i8> || is_same_v<T, i16> || is_same_v<T, i32> || is_same_v<T, i64>;
    template<typename T>
    concept Int = Sint<T> || Uint<T>;

    struct UVarInt {
        u8 data[10];
        u8 length;

        UVarInt() noexcept {
            length = 1;
            data[0] = MSB;
        }

        UVarInt& operator=(const UVarInt& copy) noexcept {
            if (this == &copy) goto same;
            length = copy.length;
            memcpy(data, copy.data, length);
            same: return *this;
        }

        UVarInt& operator=(UVarInt&& move) noexcept {
            if (this == &move) goto same;
            length = move.length;
            memcpy(data, move.data, length);
            same: return *this;
        }

        UVarInt(const UVarInt& copy) noexcept { operator=(copy); }
        UVarInt(UVarInt&& _move) noexcept { operator=(move(_move)); }

        //For not initializing the object at all. You should know what you're doing.
        #pragma warning(suppress: 26495)
        UVarInt(bool) noexcept {}

        //For pushing in data directly. You should know what you're doing.
        UVarInt(const u8* input, u64 _length) noexcept {
            length = _length;
            memcpy(data, input, length);
        }

        UVarInt(u8 input) noexcept : UVarInt(static_cast<u64>(input)) {}
        UVarInt(u16 input) noexcept : UVarInt(static_cast<u64>(input)) {}
        UVarInt(u32 input) noexcept : UVarInt(static_cast<u64>(input)) {}
        UVarInt(u64 input) noexcept { encode(input); }

        template<Uint T = u64>
        T get() const noexcept {
            u64 result = 0;
            for (u8 i = 0; i < length; i++) result |= static_cast<u64>(data[i] & (MSB - 1)) << (7 * i);
            return static_cast<T>(result);
        }
        explicit operator u8() const noexcept { return get<u8>(); }
        explicit operator u16() const noexcept { return get<u16>(); }
        explicit operator u32() const noexcept { return get<u32>(); }
        operator u64() const noexcept { return get<u64>(); }

        //Note: We don't encourage arithmetic operations to be done directly on any of the NBT types.
        //todo: Use bitwise approaches
        UVarInt& operator++() noexcept {
            encode(get() + 1);
            return *this;
        }
        UVarInt operator++(int) noexcept {
            UVarInt old = *this;
            encode(get() + 1);
            return old;
        }
        UVarInt& operator--() noexcept {
            encode(get() - 1);
            return *this;
        }
        UVarInt operator--(int) noexcept {
            UVarInt old = *this;
            encode(get() - 1);
            return old;
        }
        template<Int T>
        UVarInt& operator+=(const T& other) noexcept {
            encode(get() + other);
            return *this;
        }
        template<Int T>
        UVarInt& operator-=(const T& other) noexcept {
            encode(get() - other);
            return *this;
        }
        template<Int T>
        UVarInt& operator*=(const T& other) noexcept {
            encode(get() * other);
            return *this;
        }
        template<Int T>
        UVarInt& operator/=(const T& other) noexcept {
            encode(get() / other);
            return *this;
        }
        template<Int T>
        UVarInt& operator%=(const T& other) noexcept {
            encode(get() % other);
            return *this;
        }

        friend ostream& operator<<(ostream& os, const UVarInt& data) noexcept {
            os << data.get();
            return os;
        }

    private:
        void encode(u64 input) {
            if (input == 0) {
                length = 1;
                data[0] = MSB;
            }
            else {
                array<u8, 10> buffer{ 0 };
                u8 cursor = 0;
                while (input > 0) {
                    buffer[cursor] = static_cast<u8>(input & (MSB - 1));
                    cursor++;
                    input >>= 7;
                }
                #pragma warning(suppress: 28020)
                buffer[cursor - 1] += MSB;
                length = cursor;
                memcpy(data, &buffer, length);
            }
        }
    };

    struct IVarInt {
        u8 data[10];
        u8 length;

        IVarInt() noexcept {
            length = 1;
            data[0] = MSB;
        }

        IVarInt& operator=(const IVarInt& copy) noexcept {
            if (this == &copy) goto same;
            length = copy.length;
            memcpy(data, copy.data, length);
            same: return *this;
        }

        IVarInt& operator=(IVarInt&& move) noexcept{
            if (this == &move) goto same;
            length = move.length;
            memcpy(data, move.data, length);
            same: return *this;
        }

        IVarInt(const IVarInt& copy) noexcept { operator=(copy); }
        IVarInt(IVarInt&& _move) noexcept { operator=(move(_move)); }

        //For not initializing the object at all. You should know what you're doing.
        #pragma warning(suppress: 26495)
        IVarInt(bool) noexcept {}

        //For pushing in data directly. You should know what you're doing.
        IVarInt(const u8* input, u64 _length) noexcept {
            length = _length;
            memcpy(data, input, length);
        }

        IVarInt(i8 input) noexcept : IVarInt(static_cast<i64>(input)) {}
        IVarInt(i16 input) noexcept : IVarInt(static_cast<i64>(input)) {}
        IVarInt(i32 input) noexcept : IVarInt(static_cast<i64>(input)) {}
        IVarInt(i64 input) noexcept { encode(input); }

        template<Sint T = i64>
        T get() const noexcept {
            u64 result = 0;
            for (u8 i = 0; i < length; i++) result |= static_cast<u64>(data[i] & (MSB - 1)) << (7 * i);
            return static_cast<T>((result >> 1) ^ -(result & 1));
        }
        explicit operator i8() const noexcept { return get<i8>(); }
        explicit operator i16() const noexcept { return get<i16>(); }
        explicit operator i32() const noexcept { return get<i32>(); }
        operator i64() const noexcept { return get<i64>(); }

        //Note: We don't encourage arithmetic operations to be done directly on any of the NBT types.
        //todo: Use bitwise approaches
        IVarInt& operator++() noexcept {
            encode(get() + 1);
            return *this;
        }
        IVarInt operator++(int) noexcept {
            IVarInt old = *this;
            encode(get() + 1);
            return old;
        }
        IVarInt& operator--() noexcept {
            encode(get() - 1);
            return *this;
        }
        IVarInt operator--(int) noexcept {
            IVarInt old = *this;
            encode(get() - 1);
            return old;
        }
        IVarInt operator-() const noexcept { return IVarInt(-get()); }
        template<Int T>
        IVarInt& operator+=(const T& other) noexcept {
            encode(get() + other);
            return *this;
        }
        template<Int T>
        IVarInt& operator-=(const T& other) noexcept {
            encode(get() - other);
            return *this;
        }
        template<Int T>
        IVarInt& operator*=(const T& other) noexcept {
            encode(get() * other);
            return *this;
        }
        template<Int T>
        IVarInt& operator/=(const T& other) noexcept {
            encode(get() / other);
            return *this;
        }
        template<Int T>
        IVarInt& operator%=(const T& other) noexcept {
            encode(get() % other);
            return *this;
        }

        friend ostream& operator<<(ostream& os, const IVarInt& data) noexcept {
            os << data.get();
            return os;
        }

        bool isNegative() const noexcept { return data[0] & 1; }

    private:
        void encode(i64 input) {
            if (input == 0) {
                length = 1;
                data[0] = MSB;
            }
            else {
                //Zigzag encoding
                input = (input << 1) ^ (input >> 63);
                array<u8, 10> buffer{ 0 };
                u8 cursor = 0;
                while (input > 0) {
                    buffer[cursor] = static_cast<u8>(input & static_cast<i64>(MSB - 1));
                    cursor++;
                    input >>= 7;
                }
                #pragma warning(suppress: 28020)
                buffer[cursor - 1] += MSB;
                length = cursor;
                memcpy(data, &buffer, length);
            }
        }
    };

    //Sets the pointer to the start of the next byte.
    inline IVarInt IReadRaw(u8* input) noexcept {
        if (input == nullptr) return IVarInt();
        IVarInt result(true);
        result.data[0] = *input;
        result.length = 1;
        while (!(*input & MSB)) {
            input++;
            result.data[result.length] = *input;
            result.length++;
        }
        input++;
        return result;
    }

    //Sets the pointer to the start of the next byte.
    inline UVarInt UReadRaw(u8* input) noexcept {
        if (input == nullptr) return UVarInt();
        UVarInt result(true);
        result.data[0] = *input;
        result.length = 1;
        while (!(*input & MSB)) {
            input++;
            result.data[result.length] = *input;
            result.length++;
        }
        input++;
        return result;
    }

    //Sets `cursor.current` to the start of the next byte.
    inline IVarInt IRead(NBT::IO::FileCursor& cursor) noexcept {
        IVarInt result(true);
        result.data[0] = *cursor;
        result.length = 1;
        while (!(*cursor & MSB)) {
            ++cursor;
            result.data[result.length] = *cursor;
            result.length++;
        }
        ++cursor;
        return result;
    }

    //Sets `cursor.current` to the start of the next byte.
    inline UVarInt URead(NBT::IO::FileCursor& cursor) noexcept {
        UVarInt result(true);
        result.data[0] = *cursor;
        result.length = 1;
        while (!(*cursor & MSB)) {
            ++cursor;
            result.data[result.length] = *cursor;
            result.length++;
        }
        ++cursor;
        return result;
    }

    //Sets `cursor.current` to the start of the next byte.
    inline u64 UReadInt(NBT::IO::FileCursor& cursor) noexcept {
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

    //Sets `cursor.current` to the start of the next byte.
    inline i64 IReadInt(NBT::IO::FileCursor& cursor) noexcept {
        auto _unsigned = UReadInt(cursor);
        return (_unsigned >> 1) ^ -(_unsigned & 1);
    }
}

inline NBT::VarIntNS::IVarInt operator""_ivarint(uint64_t input) noexcept {
    assert(input <= std::numeric_limits<int64_t>::max());
    return NBT::VarIntNS::IVarInt(static_cast<int64_t>(input));
}

inline NBT::VarIntNS::UVarInt operator""_uvarint(uint64_t input) noexcept {
    return NBT::VarIntNS::UVarInt(input);
}
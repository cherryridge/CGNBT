#pragma once
#include <cstring>
#include <istream>
#include <ostream>
#include <span>

namespace NBT::IO {
    typedef uint8_t u8;
    typedef int64_t i64;
    using std::istream, std::ostream, std::span, std::streamsize, std::streamoff, std::ios, std::memcpy, std::min;

    // std::istream adapter
    struct StdIn {
        explicit StdIn(istream& s) noexcept : s_(s) {
            s_.seekg(0, ios::end);
            size_ = static_cast<i64>(static_cast<streamoff>(s_.tellg()));
            s_.seekg(0, ios::beg);
        }
        [[nodiscard]] size_t readBlock(u8* buf, size_t n) noexcept {
            s_.read(reinterpret_cast<char*>(buf), static_cast<streamsize>(n));
            return static_cast<size_t>(s_.gcount());
        }
        void incrementBy(size_t n) noexcept { s_.seekg(static_cast<streamoff>(n), ios::cur); }
        [[nodiscard]] i64 getOffset() noexcept { return static_cast<i64>(static_cast<streamoff>(s_.tellg())); }
        [[nodiscard]] i64 getSize() noexcept { return size_; }
    private:
        istream& s_;
        i64 size_{-1};
    };

    // span<const u8> adapter — no I/O, pure in-memory
    struct SpanIn {
        explicit SpanIn(span<const u8> s) noexcept : s_(s) {}
        [[nodiscard]] size_t readBlock(u8* buf, size_t n) noexcept {
            const size_t available = s_.size() - pos_, count = min(n, available);
            memcpy(buf, s_.data() + pos_, count);
            pos_ += count;
            return count;
        }
        void incrementBy(size_t n) noexcept { pos_ = min(pos_ + n, s_.size()); }
        [[nodiscard]] i64 getOffset() noexcept { return static_cast<i64>(pos_); }
        [[nodiscard]] i64 getSize() noexcept { return static_cast<i64>(s_.size()); }
    private:
        span<const u8> s_;
        size_t pos_{0};
    };

    // std::ostream adapter
    struct StdOut {
        explicit StdOut(ostream& s) noexcept : s_(s) {}
        bool writeBlock(const u8* buf, size_t n) noexcept {
            s_.write(reinterpret_cast<const char*>(buf), static_cast<streamsize>(n));
            return !!s_;
        }
    private:
        ostream& s_;
    };
}
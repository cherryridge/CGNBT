#pragma once
#include <array>
#include <concepts>
#include <cstring>
#include <stdexcept>
#include <vector>
#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>

#include "error.hpp"

namespace NBT::IO {
    typedef uint8_t  u8;
    typedef uint16_t u16;
    typedef int64_t  i64;
    typedef uint64_t u64;
    using std::array, std::min, std::memcpy, std::vector, std::same_as, std::out_of_range, NBT::Error::pushError;

    inline constexpr u16 BUFFER_SIZE = 4096;

    template<typename S>
    concept Readable = requires(S& s, u8* buf, size_t n) {
        { s.readBlock(buf, n) } -> same_as<size_t>;
        { s.incrementBy(n) };
        { s.getOffset() } -> same_as<i64>;
        // -1 if unknown
        { s.getSize() } -> same_as<i64>;
    };

    template<typename S>
    concept Writable = requires(S& s, const u8* buf, size_t n) {
        { s.writeBlock(buf, n) } -> same_as<bool>;
    };

    template<Readable S>
    struct FileReader {
        [[nodiscard]] explicit FileReader(S& source) noexcept : src_(&source), fileSize_(source.getSize()) {
            array<u8, 5> hdr{};
            if (source.readBlock(hdr.data(), 5) < 5) {
                pushError("Stream too short to be a valid CGNBT file!");
                return;
            }
            buffer_.resize(BUFFER_SIZE);
            if (hdr[0]=='c' && hdr[1]=='G' && hdr[2]=='n' && hdr[3]=='b' && hdr[4]=='T') {
                status_ = Status::Plain;
                fetchBlock(true);
            }
            else if (ZSTD_isFrame(hdr.data(), 4) || ZSTD_isSkippableFrame(hdr.data(), 4)) {
                status_ = Status::Zstd;
                zstdStream_ = ZSTD_createDStream();
                ZSTD_initDStream(zstdStream_);
                inBuffer_.resize(ZSTD_DStreamInSize());
                // Pre-fill inBuffer with the already-read 5 bytes — they are part of the zstd frame.
                memcpy(inBuffer_.data(), hdr.data(), 5);
                zsrc_ = {inBuffer_.data(), 5, 0};
                fetchBlock(true);
            }
            else pushError("Stream does not contain a valid CGNBT file!");
        }

        FileReader(const FileReader&) = delete;
        FileReader& operator=(const FileReader&) = delete;
        [[nodiscard]] FileReader(FileReader&&) noexcept = default;
        [[nodiscard]] FileReader& operator=(FileReader&&) noexcept = default;

        [[nodiscard]] explicit operator bool() const noexcept { return status_ != Status::End; }
        [[nodiscard]] bool empty() const noexcept { return status_ == Status::Empty; }

        [[nodiscard]] u8 operator*() const {
            if (status_ == Status::End) throw out_of_range("FileReader: cursor at EOF or initialization failed!");
            return buffer_[bufPos_];
        }

        // Reads up to `length` decoded bytes into `dst`; returns bytes actually written.
        // Faster than repeated operator++/operator*.
        [[nodiscard]] u64 getContent(u8* dst, u64 length) noexcept {
            u64 progress = 0;
            while (progress < length && status_ != Status::End) {
                if (bufPos_ == bufSize_) {
                    fetchBlock();
                    if (status_ == Status::End) break;
                }
                const u64 available = bufSize_ - bufPos_, delta = min(available, length - progress);
                memcpy(dst + progress, buffer_.data() + bufPos_, delta);
                bufPos_ += delta;
                progress += delta;
                decoded_ += delta;
            }
            return progress;
        }

        FileReader& operator++() noexcept {
            if (status_ != Status::End) {
                ++bufPos_;
                ++decoded_;
                if (bufPos_ == bufSize_) fetchBlock();
            }
            return *this;
        }

        // Decoded bytes consumed so far (useful for error reporting).
        [[nodiscard]] u64 currentOffset() const noexcept { return decoded_; }
        [[nodiscard]] bool compressed() const noexcept { return status_ == Status::Zstd; }
        // Raw source size in bytes; 0 if the source reported unknown (-1).
        [[nodiscard]] u64 getFileSize() const noexcept { return fileSize_ > 0 ? static_cast<u64>(fileSize_) : 0; }

        bool close() noexcept {
            status_ = Status::End;
            if (zstdStream_ != nullptr) { ZSTD_freeDStream(zstdStream_); zstdStream_ = nullptr; }
            src_ = nullptr;
            return true;
        }

        ~FileReader() { if (zstdStream_ != nullptr) ZSTD_freeDStream(zstdStream_); }

    private:
        S* src_{nullptr};
        i64 fileSize_{-1};
        u64 bufPos_{0}, bufSize_{0}, decoded_{0};
        vector<u8> buffer_, inBuffer_;
        ZSTD_DStream* zstdStream_{nullptr};
        ZSTD_inBuffer zsrc_{nullptr, 0, 0};
        enum struct Status : u8 { Plain, Zstd, End, Empty } status_{Status::End};

        // isFirstFetch=true: treat an empty first block as an empty (but valid) file.
        void fetchBlock(bool isFirstFetch = false) noexcept {
            bufPos_ = 0;
            bufSize_ = 0;
            switch (status_) {
                case Status::Plain: {
                    bufSize_ = src_->readBlock(buffer_.data(), BUFFER_SIZE);
                    if (bufSize_ == 0) status_ = isFirstFetch ? Status::Empty : Status::End;
                    break;
                }
                case Status::Zstd: {
                    ZSTD_outBuffer dst{buffer_.data(), BUFFER_SIZE, 0};
                    while (dst.pos < dst.size) {
                        if (zsrc_.pos == zsrc_.size) {
                            zsrc_.size = src_->readBlock(inBuffer_.data(), inBuffer_.size());
                            zsrc_.pos = 0;
                            if (zsrc_.size == 0) break;
                        }
                        const auto r = ZSTD_decompressStream(zstdStream_, &dst, &zsrc_);
                        if (ZSTD_isError(r) || (r == 0 && dst.pos == 0)) break;
                    }
                    bufSize_ = dst.pos;
                    if (bufSize_ == 0) status_ = isFirstFetch ? Status::Empty : Status::End;
                    break;
                }
                default: break;
            }
        }
    };
}
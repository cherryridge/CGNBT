#pragma once
#include <array>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>
#include <physfs.h>
#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>

namespace NBT::IO {
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef int64_t i64;
    typedef uint64_t u64;
    using std::array, std::move, std::swap, std::min, std::vector, std::string, std::streampos, std::streamoff, std::logic_error, std::span;

    //note: I currently don't want to get target's filesystem's alignment size dynamically. This part is too platform-specific and require a lot of experiments and tests.
    inline constexpr u16 BUFFER_SIZE = 4096;

    struct FileReader {
        [[nodiscard]] explicit FileReader(const char* path) noexcept : file(PHYSFS_openRead(path)) {
            if (file && PHYSFS_fileLength(file) > 5) {
                array<u8, 5> header{ 0 };
                PHYSFS_readBytes(file, header.data(), 5);
                PHYSFS_seek(file, 0);
                buffer.resize(BUFFER_SIZE);
                if (header[0] == 'c' && header[1] == 'G' && header[2] == 'n' && header[3] == 'b' && header[4] == 'T') {
                    status = Status::Plain;
                    PHYSFS_seek(file, 5);
                    fetchBlock();
                }
                else if (ZSTD_isFrame(header.data(), 4) || ZSTD_isSkippableFrame(header.data(), 4)) {
                    status = Status::Zstd;
                    zstdStream = ZSTD_createDStream();
                    ZSTD_initDStream(zstdStream);
                    inBuffer.resize(ZSTD_DStreamInSize());
                    src = { inBuffer.data(), 0, 0 };
                    fetchBlock();
                }
                //todo: show error
                else return;
            }
        }

        [[nodiscard]] FileReader(const FileReader&) = delete;
        [[nodiscard]] FileReader& operator=(const FileReader&) = delete;
        [[nodiscard]] FileReader(FileReader&&) noexcept = default;
        [[nodiscard]] FileReader& operator=(FileReader&&) noexcept = default;

        [[nodiscard]] explicit operator bool() const noexcept { return status != Status::End; }
        [[nodiscard]] bool eof() const noexcept { return status == Status::End; }

        [[nodiscard]] u8 operator*() {
            if (status == Status::End) throw logic_error("FileReader initialization failed or cursor is at EOF!");
            return buffer[bufPos];
        }

        //Advances the cursor to the next byte and returns the actual bytes written.
        //Faster than `operator++` and `operator*`.
        [[nodiscard]] u64 getContent(u8* dst, u64 length) noexcept {
            u64 progress = 0;
            while (progress < length && status != Status::End) {
                if (bufPos == bufSize) {
                    fetchBlock();
                    if (status == Status::End) break;
                }
                const u64 available = bufSize - bufPos, delta = min(available, length - progress);
                memcpy(dst + progress, buffer.data() + bufPos, delta);
                bufPos += delta;
                progress += delta;
            }
            return progress;
        }

        FileReader& operator++() noexcept {
            if (status != Status::End) {
                bufPos++;
                if (bufPos == bufSize) fetchBlock();
            }
            return *this;
        }

        //Ensure validation before calling.
        [[nodiscard]] u64 currentOffset() const noexcept {
            i64 off = PHYSFS_tell(file);
            if (off == -1) return 0;
            if (status == Status::Plain) return off - bufSize + bufPos;
            else if (status == Status::Zstd) return off - src.size + src.pos;
            else return 0;
        }

        //Ensure validation before calling.
        [[nodiscard]] bool compressed() const noexcept { return status == Status::Zstd; }

        //Ensure validation before calling.
        [[nodiscard]] u64 getFileSize() const noexcept {
            if (file != nullptr) {
                auto size = PHYSFS_fileLength(file);
                if (size == -1) return 0;
                return static_cast<u64>(size);
            }
            return 0;
        }

        ~FileReader() {
            if (zstdStream != nullptr) ZSTD_freeDStream(zstdStream);
            if (file != nullptr) PHYSFS_close(file);
        }

    private:
        PHYSFS_File* file{ nullptr };
        u64 bufPos{ 0 }, bufSize{ 0 };
        vector<u8> buffer;
        vector<u8> inBuffer;
        ZSTD_DStream* zstdStream{ nullptr };
        ZSTD_inBuffer src{ nullptr, 0, 0 };
        enum struct Status : u8 {
            Plain, Zstd, End
        } status{ Status::End };

        void fetchBlock() noexcept {
            bufPos = 0;
            bufSize = 0;
            switch (status) {
                case Status::Plain: {
                    bufSize = PHYSFS_readBytes(file, buffer.data(), BUFFER_SIZE);
                    if (bufSize == 0) status = Status::End;
                    break;
                }
                case Status::Zstd: {
                    ZSTD_outBuffer dst{ buffer.data(), BUFFER_SIZE, 0 };
                    while (dst.pos < dst.size) {
                        if (src.pos == src.size) {
                            src.size = PHYSFS_readBytes(file, inBuffer.data(), inBuffer.size());
                            src.pos = 0;
                            if (src.size == 0) break;
                        }
                        auto result = ZSTD_decompressStream(zstdStream, &dst, &src);
                        if (ZSTD_isError(result) || (result == 0 && dst.pos == 0)) break;
                    }
                    bufSize = dst.pos;
                    if (bufSize == 0) status = Status::End;
                    break;
                }
                default: break;
            }
        }
    };

    struct FileWriter {
        [[nodiscard]] explicit FileWriter(const char* path, u8 bufferMag = 10) noexcept : file(PHYSFS_openWrite(path)) { if (file && PHYSFS_setBuffer(file, 1ull << bufferMag)) active = true; }

        [[nodiscard]] FileWriter(const FileWriter&) = delete;
        [[nodiscard]] FileWriter& operator=(const FileWriter&) = delete;
        [[nodiscard]] FileWriter(FileWriter&& _move) noexcept : file(_move.file) { _move.file = nullptr; }
        [[nodiscard]] FileWriter& operator=(FileWriter&& _move) noexcept {
            swap(file, _move.file);
            return *this;
        }

        ~FileWriter() { if (file != nullptr) PHYSFS_close(file); }

        [[nodiscard]] explicit operator bool() const noexcept { return active; }

        bool write(span<u8> data) noexcept {
            auto wrote = PHYSFS_writeBytes(file, data.data(), data.size());
            if (wrote != data.size()) {
                //todo: failure
            }
            return true;
        }

    private:
        PHYSFS_File* file{ nullptr };
        bool active{ false };
    };
}
#pragma once
#include <fstream>
#include <vector>
#include <filesystem>
#include <string>
#include <type_traits>
#include <zlib-ng.h>

namespace NBT::IO {
    typedef int8_t i8;
    typedef uint8_t u8;
    typedef int16_t i16;
    typedef uint16_t u16;
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef int64_t i64;
    typedef uint64_t u64;
    using std::ifstream, std::vector, std::string, std::streampos, std::streamoff, std::filesystem::path, std::is_same_v, std::bit_cast, std::out_of_range, std::logic_error;

    template<typename T>
    concept Int = is_same_v<T, i8> || is_same_v<T, i16> || is_same_v<T, i32> || is_same_v<T, i64> || is_same_v<T, u8> || is_same_v<T, u16> || is_same_v<T, u32> || is_same_v<T, u64>;

    //note: I currently don't want to get target's filesystem's alignment size dynamically. This part is too platform-specific and require a lot of experiments and tests.
    inline constexpr u16 BUFFER_SIZE = 4096;

    struct FileCursor {
        ifstream stream;
        vector<u8> buffer;
        streamoff fileSize;
        //The position of the start of the buffer in the file.
        streampos bufferStart;
        //The position of the "cursor" inside the buffer, relative to buffer (0-4095).
        u16 posInBuf;
        bool active;

        FileCursor(const path& _path) noexcept : stream(_path, std::ios::in | std::ios::binary), buffer(BUFFER_SIZE), bufferStart(0), posInBuf(0), active(true) {
            if (!stream) goto fail;
            stream.seekg(0, std::ios::end);
            fileSize = stream.tellg();
            stream.seekg(0, std::ios::beg);
            fetchBlock();
            return;
            fail: active = false;
        }

        u8 operator*(){
            if (!active) throw logic_error("FileCursor initialization failed or cursor is at EOF!");
            return buffer[posInBuf];
        }

        FileCursor& operator++() {
            if (posInBuf < BUFFER_SIZE - 1) {
                posInBuf++;
                auto cur = current();
                if (cur > fileSize) goto eof_;
                //For cursor placing convenience.
                else if (cur == fileSize) active = false;
            }
            else {
                //Get the next block.
                bufferStart += 4096;
                posInBuf = 0;
                auto cur = current();
                if (cur > fileSize) goto eof_;
                //For cursor placing convenience.
                else if (cur == fileSize) active = false;
                else fetchBlock();
            }
            return *this;
            eof_: throw out_of_range("Increment past EOF!");
        }

        FileCursor& operator--() {
            if(bufferStart == 0 && posInBuf == 0) throw out_of_range("Decrement before BOF!");
            if (posInBuf > 0) posInBuf--;
            else {
                //Get the previous block.
                bufferStart -= 4096;
                posInBuf = 4095;
                fetchBlock();
            }
            return *this;
        }

        template<Int T>
        FileCursor& operator+=(const T& offset) {
            jump(current() + offset);
            return *this;
        }

        template<Int T>
        FileCursor& operator-=(const T& offset) {
            jump(current() - offset);
            return *this;
        }

        void fetchBlock() noexcept {
            //std::cout << "fetchBlock!" << std::endl;
            stream.seekg(bufferStart);
            stream.read(bit_cast<char*>(buffer.data()), BUFFER_SIZE);
        }

        streamoff current() const noexcept { return static_cast<streamoff>(bufferStart) + static_cast<streamoff>(posInBuf); }

        bool eof() const noexcept { return current() >= fileSize; }
        //For cursor placing convenience.
        bool looseEof() const noexcept { return current() > fileSize; }

        //Allows for reaching beyond EOF for 1 byte for cursor placing convenience. However accessing this byte will throw at operator*.
        //You must push in `readErrors` by yourself. `FileCursor` don't has access to it.
        void jump(streamoff to) {
            if (to < 0 || to > fileSize) {
                string temp("Offset ");
                temp += to;
                temp += " is out of bound (0-";
                temp += fileSize - 1;
                temp += ")!";
                throw out_of_range(temp);
            }
            auto oldBufferStart = bufferStart;
            //Biggest multiple of 4096 that is less than `to`.
            bufferStart = to & ~static_cast<streamoff>(BUFFER_SIZE - 1);
            posInBuf = to - bufferStart;
            //EOF reached, signal for operator* to be faster
            if (to == fileSize) active = false;
            //New position is not in the same block
            else if(oldBufferStart != bufferStart) fetchBlock();
        }

        //Returns the length actually read.
        //Reading beyond `EOF` is allowed but will return all `0`.
        u64 getContent(streamoff start, streamoff length, u8* result) {
            if (start < 0) throw out_of_range("Trying to get file content below index 0");
            auto _bufferStart = static_cast<streamoff>(bufferStart), end = start + length - 1, bufferEnd = _bufferStart + BUFFER_SIZE - 1;
            //Cache hit
            if (start <= bufferEnd && end >= _bufferStart) {
                auto leftIn = start >= _bufferStart, rightIn = end <= bufferEnd;
                //Complete hit
                if (leftIn && rightIn) memcpy(result, buffer.data() + start - _bufferStart, length);
                //Partial hit within left boundary
                else if(leftIn) {
                    auto cachedLength = _bufferStart + BUFFER_SIZE - start;
                    memcpy(result, buffer.data() + start - _bufferStart, cachedLength);
                    stream.seekg(_bufferStart + BUFFER_SIZE);
                    stream.read(bit_cast<char*>(result + cachedLength), start + length - _bufferStart - BUFFER_SIZE);
                    //std::cout << "miss!" << std::endl;
                }
                //Partial hit within right boundary
                else if (rightIn) {
                    auto missedLength = _bufferStart - start;
                    stream.seekg(start);
                    stream.read(bit_cast<char*>(result), missedLength);
                    //std::cout << "miss!" << std::endl;
                    memcpy(result + missedLength, buffer.data(), length - missedLength);
                }
                //Partial hit in the middle, just act like cache miss
                else {
                    stream.seekg(start);
                    stream.read(bit_cast<char*>(result), length);
                    //std::cout << "miss!" << std::endl;
                }
            }
            //Cache miss
            else {
                stream.seekg(start);
                stream.read(bit_cast<char*>(result), length);
                //std::cout << "miss!" << std::endl;
            }
            //Just to prevent some weird attacks
            if (start + length > fileSize) {
                memset(result + fileSize - start, 0, start + length - fileSize);
                return fileSize - start;
            }
            else return length;
        }

        ~FileCursor() { if (stream.is_open()) stream.close(); }
    };

    struct GZipFile {

        GZipFile(const path& _path) noexcept {

        }
    };
}
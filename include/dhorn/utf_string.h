/*
 * Duncan Horn
 *
 * utf_string.h
 *
 *
 */
#pragma once

#include <cassert>
#include <string>
#include <memory>

#include "algorithm.h"

namespace dhorn
{
    /*
     * UTF Encoding
     */
    enum class utf_encoding
    {
        utf_8 = 8,
        utf_16 = 16,
        utf_32 = 32,
    };



    /*
     * bad_utf_encoding Exception
     */
    class bad_utf_encoding :
        public std::exception
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        bad_utf_encoding(_In_ utf_encoding encoding, _In_ char32_t badValue) :
            _encoding(encoding),
            _badValue(badValue)
        {
        }



        /*
         * Exception data
         */
        utf_encoding encoding(void) const
        {
            return this->_encoding;
        }

        char32_t bad_value(void) const
        {
            return this->_badValue;
        }



    private:

        utf_encoding _encoding;
        char32_t _badValue;
    };



    namespace garbage
    {
        /*
         * Declare read functions outside of traits for cross consumption
         */
        inline void verify_character(_In_ utf_encoding encoding, _In_ char32_t val)
        {
            if (((val > 0x0000D7FF) && (val < 0x0000E000)) ||
                (val > 0x0010FFFF))
            {
                throw bad_utf_encoding(encoding, val);
            }
        }

#pragma region utf-8

        inline constexpr size_t size_utf8(_In_ char ch)
        {
            // UTF-8 character widths are defined as:
            // 0xxx xxxx    - 1 byte wide
            // 110x xxxx    - 2 bytes wide
            // 1110 xxxx    - 3 bytes wide
            // 1111 0xxx    - 4 bytes wide
            return
                ((ch & 0x80) == 0x00) ? 1 :
                ((ch & 0xE0) == 0xC0) ? 2 :
                ((ch & 0xF0) == 0xE0) ? 3 :
                ((ch & 0xF8) == 0xF0) ? 4 : 0;
        }

        inline constexpr size_t size_utf8(_In_ char32_t val)
        {
            // The format of a utf-8 character is (A is first byte, B is second, etc.):
            // 1 byte:  0000 0000 0000 0000 0000 0000 0AAA AAAA
            // 2 bytes: 0000 0000 0000 0000 0000 0AAA AABB BBBB
            // 3 bytes: 0000 0000 0000 0000 AAAA BBBB BBCC CCCC
            // 4 bytes: 0000 0000 000A AABB BBBB CCCC CCDD DDDD
            return
                (val & 0xFFE00000) ? 0 :
                (val & 0x001F0000) ? 4 :
                (val & 0x001FF800) ? 3 :
                (val & 0x001FFF80) ? 2 : 1;
        }

        inline char32_t read_utf8(_In_ const char *str, _Out_opt_ const char **result)
        {
            size_t size = size_utf8(*str);
            unsigned char mask = 0xFF >> size;
            char32_t val = 0;

            if (size == 0)
            {
                throw bad_utf_encoding(utf_encoding::utf_8, static_cast<unsigned char>(*str));
            }

            // TODO: Figure out how much of a performance benefit there is to hard-code this
            for (size_t i = 0; i < size; ++i)
            {
                if ((i > 0) && ((*str & 0xC0) != 0x80))
                {
                    throw bad_utf_encoding(utf_encoding::utf_8, *str);
                }

                val = (val << 6) | (*str & mask);
                ++str;
                mask = 0x3F;
            }

            if (result)
            {
                *result = str;
            }

            return val;
        }

#pragma endregion

#pragma region utf-16

        inline constexpr size_t size_utf16(_In_ char16_t ch)
        {
            // UTF-16 character widths are defined as:
            // 1101 10xx xxxx xxxx  - 2 characters (4 bytes)
            // 1101 11xx xxxx xxxx  - INVALID
            // Else: 1 character (2 bytes)
            return
                ((static_cast<uint16_t>(ch) & 0xFC00) == 0xD800) ? 2 :
                ((static_cast<uint16_t>(ch) & 0xFC00) == 0xDC00) ? 0 : 1;
        }

        inline constexpr size_t size_utf16(_In_ char32_t ch)
        {
            // 0x000000 to 0x00D7FF:    1 character (2 bytes)
            // 0x00D800 to 0x00DFFF:    INVALID
            // 0x00E000 to 0x00FFFF:    1 character (2 bytes)
            // 0x010000 to 0x10FFFF:    2 characters (4 bytes)
            // >= 0x110000:             INVALID
            // For simplicity - and the ability to use constexpr - we ignore the invalid cases here and let callers
            // ensure validity
            return (ch & 0xFFFF0000) ? 2 : 1;
        }

        inline char32_t read_utf16(_In_ const char16_t *str, _Out_opt_ const char16_t **result)
        {
            auto size = size_utf16(*str);

            if (size == 0)
            {
                throw bad_utf_encoding(utf_encoding::utf_16, *str);
            }

            char32_t val;
            if (size == 0)
            {
                throw bad_utf_encoding(utf_encoding::utf_16, *str);
            }
            else if (size == 1)
            {
                val = *str;
            }
            else // size == 2
            {
                val = (*str & 0x03FF) << 10;
                val |= (str[1] & 0x03FF);
                val += 0x00010000;

                if ((str[1] & 0xFC00) != 0xDC00)
                {
                    throw bad_utf_encoding(utf_encoding::utf_16, str[1]);
                }
            }

            if (result)
            {
                *result = str + size;
            }

            return val;
        }

#pragma endregion



        /*
         * UTF Traits
         */
#pragma region UTF Traits

        template <utf_encoding Encoding>
        struct utf_traits;



        template <>
        struct utf_traits<utf_encoding::utf_8>
        {
            static const utf_encoding encoding = utf_encoding::utf_8;
            using value_type = char;

            static inline constexpr size_t size(_In_ value_type val)
            {
                return size_utf8(val);
            }

            static inline char32_t next(_In_ const value_type *pos, _Out_opt_ const value_type **output)
            {
                return read_utf8(pos, output);
            }

            static inline value_type *write(_In_ char32_t val, /*_Out_*/ value_type *pos)
            {
                size_t bytes = size_utf8(val);
                verify_character(utf_encoding::utf_8, val);

                static const unsigned char masks[] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0 };
                auto mask = masks[bytes];

                // All bytes past the first one are six bits of data long
                size_t shift = (bytes - 1) * 6;

                for (size_t i = 0; i < bytes; ++i)
                {
                    // All masks start with all 1s followed by a single 0. We use that knowledge to our advantage
                    *pos = ((val >> shift) & (~mask >> 1)) | mask;

                    mask = 0x80;
                    ++pos;
                    shift -= 6;
                }

                return pos;
            }
        };



        template <>
        struct utf_traits<utf_encoding::utf_16>
        {
            static const utf_encoding encoding = utf_encoding::utf_16;
            using value_type = char16_t;

            static inline constexpr size_t size(_In_ value_type val)
            {
                return size_utf16(val);
            }

            static inline char32_t next(_In_ const value_type *pos, _Out_opt_ const value_type **output)
            {
                return read_utf16(pos, output);
            }

            static inline value_type *write(_In_ char32_t val, /*_Out_*/ value_type *pos)
            {
                size_t bytes = size_utf16(val);
                verify_character(utf_encoding::utf_16, val);

                if (bytes == 1)
                {
                    *pos = val & 0x0000FFFF;
                }
                else // bytes == 2
                {
                    // high bytes go first
                    val -= 0x00010000;
                    pos[0] = 0xD800 | ((val >> 10) & 0x03FF);
                    pos[1] = 0xDC00 | (val & 0x03FF);
                }

                return pos + bytes;
            }
        };



        template <>
        struct utf_traits<utf_encoding::utf_32>
        {
            static const utf_encoding encoding = utf_encoding::utf_32;
            using value_type = char32_t;

            static inline constexpr size_t size(_In_ value_type /*val*/)
            {
                return 1;
            }

            static inline char32_t next(_In_ const value_type *pos, _Out_opt_ const value_type **output)
            {
                // Need to read the value before writing to output since we don't know if output == &pos
                char32_t val = *pos;
                verify_character(utf_encoding::utf_32, val);

                if (output)
                {
                    *output = pos + 1;
                }

                return val;
            }

            static inline value_type *write(_In_ char32_t val, /*_Out_*/ value_type *pos)
            {
                verify_character(utf_encoding::utf_32, val);
                *pos = val;

                return pos + 1;
            }
        };



        /*
         * Traits Type Definitions
         */
        using utf8_traits = utf_traits<utf_encoding::utf_8>;
        using utf16_traits = utf_traits<utf_encoding::utf_16>;
        using utf32_traits = utf_traits<utf_encoding::utf_32>;

#pragma endregion



        /*
         * Convert character type -> encoding
         */
#pragma region utf_encoding_from_char

        template <typename CharT>
        struct utf_encoding_from_char;

        template <>
        struct utf_encoding_from_char<char>
        {
            static const utf_encoding value = utf_encoding::utf_8;
            using traits_type = utf_traits<value>;
        };

        template <>
        struct utf_encoding_from_char<char16_t>
        {
            static const utf_encoding value = utf_encoding::utf_16;
            using traits_type = utf_traits<value>;
        };

        template <>
        struct utf_encoding_from_char<char32_t>
        {
            static const utf_encoding value = utf_encoding::utf_32;
            using traits_type = utf_traits<value>;
        };

#pragma endregion



        /*
         * utf8_string const iterator
         */
        class utf_string_const_iterator
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            utf_string_const_iterator(
                _In_ const unsigned char *front,
                _In_ const unsigned char *back,
                _In_ const unsigned char *pos) :
                _front(front),
                _back(back),
                _pos(pos)
            {
            }

        private:

            const unsigned char *_front;
            const unsigned char *_back;
            const unsigned char *_pos;
        };
    }



    /*
     * utf_string Definition
     */
#pragma region utf_string Definition

    template <typename CharT>
    class utf_string
    {
        using Traits = typename garbage::utf_encoding_from_char<CharT>::traits_type;

        template <typename CharType>
        friend class utf_string;

    public:

        /*
         * Public Type Definitions
         */
        using value_type = typename Traits::value_type;



        /*
         * Constructor(s)/Destructor
         */
        utf_string(void) :
            _length(0),
            _front(nullptr),
            _back(nullptr),
            _bounds(nullptr)
        {
        }

        utf_string(_In_ const utf_string &other) :
            utf_string()
        {
            this->Create(other);
        }

        template <typename CharType>
        utf_string(_In_ const utf_string<CharType> &other) :
            utf_string()
        {
            this->Create(other);
        }

        utf_string(_Inout_ utf_string &&other) :
            utf_string()
        {
            this->swap(other);
        }

        template <typename CharType>
        utf_string(_In_ const CharType *str) :
            utf_string()
        {
            this->Create(str);
        }

        template <typename CharType>
        utf_string(_In_ const std::basic_string<CharType> &str) :
            utf_string()
        {
            // We don't use the iterator Create function since std::basic_string does not know about utf encodings and
            // dereferencing/incrementing may not give a full character
            this->Create(str);
        }

        template <typename Itr>
        utf_string(_In_ Itr front, _In_ Itr back) :
            utf_string()
        {
            this->Create(front, back);
        }

        ~utf_string(void)
        {
            this->Destroy();
        }



        /*
         * Operators
         */
        utf_string &operator=(_In_ const utf_string &other)
        {
            this->Assign(other);
            return *this;
        }

        template <typename CharType>
        utf_string &operator=(_In_ const utf_string<CharType> &other)
        {
            this->Assign(other);
            return *this;
        }

        utf_string &operator=(_Inout_ utf_string &&other)
        {
            assert(this != &other);
            this->swap(other);

            return *this;
        }

        template <typename CharType>
        utf_string &operator=(_In_ const CharType *str)
        {
            this->Assign(str);
            return *this;
        }

        template <typename CharType>
        utf_string &operator=(_In_ const std::basic_string<CharType> &other)
        {
            this->Assign(other);
            return *this;
        }

        template <typename CharType>
        utf_string &operator+=(_In_ const utf_string<CharType> &other)
        {
            this->Append(other);
            return *this;
        }

        template <typename CharType>
        utf_string &operator+=(_In_ const CharType *other)
        {
            this->Append(other);
            return *this;
        }

        template <typename CharType>
        utf_string &operator+=(_In_ const std::basic_string<CharType> &other)
        {
            this->Append(other);
            return *this;
        }

        operator CharT *(void)
        {
            return this->_front;
        }

        operator const CharT *(void) const
        {
            return this->_front;
        }



        /*
         * Public "string-like" functions
         */
        bool empty(void) const
        {
            return (this->_length == 0);
        }

        size_t length(void) const
        {
            // Length of the string
            return this->_length;
        }

        size_t size(void) const
        {
            // Size of the string buffer (minus the null character)
            return this->BufferSize();
        }

        size_t capacity(void) const
        {
            // Size of the complete string buffer available minus the null character
            return this->Capacity() - 1;
        }

        void reserve(_In_ size_t desiredSize)
        {
            this->Resize(desiredSize);
        }

        const value_type *c_str(void) const
        {
            return this->_front;
        }

        void push_back(_In_ char32_t ch)
        {
            this->InternalPushBack(ch);
            *this->_back = '\0';
        }



        /*
         * Public Functions
         */
        void swap(_Inout_ utf_string &other)
        {
            std::swap(this->_front, other._front);
            std::swap(this->_back, other._back);
            std::swap(this->_bounds, other._bounds);
            std::swap(this->_length, other._length);
        }



    private:

        static const size_t max_char_size = sizeof(char32_t) / sizeof(value_type);

        // Returns pair (length, buffer size)
        template <typename CharType>
        static std::pair<size_t, size_t> BufferSizeFromStringLiteral(_In_ const CharType *str)
        {
            using their_traits = typename garbage::utf_encoding_from_char<CharType>::traits_type;

            auto end = str;
            size_t length = 0;
            while (*end)
            {
                ++length;
                their_traits::next(end, &end);
            }

            return std::make_pair(length, end - str);
        }

        template <typename CharType>
        void AppendFromBuffer(_In_ const CharType *str)
        {
            using their_traits = typename garbage::utf_encoding_from_char<CharType>::traits_type;
            while (*str)
            {
                this->InternalPushBack(their_traits::next(str, &str));
            }

            assert(this->_back < this->_bounds);
            *this->_back = '\0';
        }

        inline bool Inside(_In_ const CharT *str) const
        {
            return (str >= this->_front) && (str < this->_bounds);
        }

        inline void FinishString()
        {
            assert(this->_back < this->_bounds);
            *this->_back = '\0';
        }

        inline void Copy(_In_ const CharT *str, _In_ size_t length, _In_ size_t bufferSize)
        {
            assert(this->_back + bufferSize < this->_bounds);
            memcpy(this->_back, str, bufferSize * sizeof(value_type));
            this->_back += bufferSize;
            this->_length += length;
        }



        //
        // Creating - assumes that this is uninitialized (i.e. this->_front == nullptr)
        //
        void Create(_In_ const utf_string &other)
        {
            assert(!this->_front);
            auto bufferSize = other.BufferSize();
            this->Resize(bufferSize);

            this->Copy(other._front, other._length, bufferSize);
            this->FinishString();
        }

        template <typename CharType>
        void Create(_In_ const utf_string<CharType> &other)
        {
            assert(!this->_front);
            this->Resize(other._length);
            this->AppendFromBuffer(other._front);
        }

        void Create(_In_ const CharT *str)
        {
            // For string literals that are the same type as us (CharT *), we can just use memcpy
            assert(!this->_front);
            auto otherSize = BufferSizeFromStringLiteral(str);
            this->Resize(otherSize.second);

            this->Copy(str, otherSize.first, otherSize.second);
            this->FinishString();
        }

        template <typename CharType>
        void Create(_In_ const CharType *str)
        {
            assert(!this->_front);
            this->Resize(BufferSizeFromStringLiteral(str).first);
            this->AppendFromBuffer(str);
        }

        void Create(_In_ const std::basic_string<CharT> &str)
        {
            // We need to calculate the buffer size anyway, so just go ahead and call the memcpy version
            this->Create(str.c_str());
        }

        template <typename CharType>
        void Create(_In_ const std::basic_string<CharType> &str)
        {
            assert(!this->_front);
            this->Resize(str.length());
            this->AppendFromBuffer(str.c_str());
        }

        template <typename Itr>
        void Create(_In_ Itr front, _In_ Itr back)
        {
            assert(!this->_front);
            this->Resize(std::distance(front, back));

            // It's assumed that dereferencing an iterator gives the full character, and that incrementing the iterator
            // moves to the next character
            for (; front != back; ++front)
            {
                this->InternalPushBack(*front);
            }

            this->FinishString();
        }



        //
        // Assignment - Has proper checks for when the this == target, or when the buffer is a part of our buffer
        //
        void Assign(_In_ const utf_string &other)
        {
            if (&other != this)
            {
                this->Destroy();
                this->Create(other);
            }
        }

        template <typename CharType>
        void Assign(_In_ const utf_string<CharType> &other)
        {
            this->Destroy();
            this->Create(other);
        }

        void Assign(_In_ const CharT *str)
        {
            if (str == this->_front)
            {
                // Nothing to do here
                return;
            }
            else if (this->Inside(str))
            {
                // If str is a part of our string, we can't destroy our string until the copy is done
                utf_string(str).swap(*this);
            }
            else
            {
                // No harm in destroying and re-creating
                this->Destroy();
                this->Create(str);
            }
        }

        template <typename CharType>
        void Assign(_In_ const CharType *str)
        {
            assert(!this->Inside(reinterpret_cast<const CharT *>(str))); // Guaranteed bad operation
            this->Destroy();
            this->Create(str);
        }

        template <typename CharType>
        void Assign(_In_ std::basic_string<CharType> &str)
        {
            this->Destroy();
            this->Create(str);
        }



        //
        // Append - has proper checks for when we append ourself or part of ourself
        //
        void Append(_In_ const utf_string &other)
        {
            auto otherSize = other.BufferSize();
            auto otherLength = other._length;

            // Okay to destroy our buffer if &other == this since we memcpy using the length later
            this->Resize(this->BufferSize() + otherSize);

            this->Copy(other._front, otherLength, otherSize);
            this->FinishString();
        }

        template <typename CharType>
        void Append(_In_ const utf_string<CharType> &other)
        {
            // Guess the resulting buffer size as one unit per character
            this->Resize(this->BufferSize() + other._length);
            this->AppendFromBuffer(other._front);
        }

        void Append(_In_ const CharT *str)
        {
            auto otherSize = BufferSizeFromStringLiteral(str);

            if (this->Inside(str))
            {
                // Since we might be destroying our buffer, we can't use str from that point on
                auto index = str - this->_front;
                this->Resize(this->BufferSize() + otherSize.second);
                str = this->_front + index;
            }
            else
            {
                this->Resize(this->BufferSize() + otherSize.second);
            }

            this->Copy(str, otherSize.first, otherSize.second);
            this->FinishString();
        }

        template <typename CharType>
        void Append(_In_ const CharType *other)
        {
            this->Resize(this->BufferSize() + this->BufferSizeFromStringLiteral(other).first);
            this->AppendFromBuffer(other);
        }

        void Append(_In_ const std::basic_string<CharT> &str)
        {
            auto otherSize = BufferSizeFromStringLiteral(str.c_str());
            this->Resize(this->BufferSize() + otherSize.second);

            this->Copy(str.c_str(), otherSize.first, otherSize.second);
            this->FinishString();
        }

        template <typename CharType>
        void Append(_In_ const std::basic_string<CharType> &other)
        {
            this->Resize(this->BufferSize() + other.length());
            this->AppendFromBuffer(other.c_str());
        }



        inline void Destroy(void)
        {
            delete[] this->_front;

            this->_front = nullptr;
            this->_back = nullptr;
            this->_bounds = nullptr;
            this->_length = 0;
        }

        inline void InternalPushBack(_In_ char32_t ch)
        {
            // Determine if we need to resize
            if ((this->_back + max_char_size) >= this->_bounds)
            {
                this->Resize(this->Capacity() * 2);
            }

            // Increase length after the write in case an exception is thrown
            // NOTE: Does not write the null character; that's the caller's responsibility
            this->_back = Traits::write(ch, this->_back);
            ++this->_length;
            assert(this->_back < this->_bounds);
        }

        inline void Resize(_In_ size_t desiredCapacity)
        {
            size_t currentCapacity = this->Capacity();
            size_t bufferSize = this->BufferSize();
            size_t capacity = dhorn::max(currentCapacity, max_char_size + 1, desiredCapacity + 1);
            assert(capacity >= (bufferSize + max_char_size + 1));

            // Don't resize if we don't need to
            if (capacity != currentCapacity)
            {
                // Don't copy the null character since this->_front is null on creation
                std::unique_ptr<value_type[]> buffer(new value_type[capacity]);
                memcpy(buffer.get(), this->_front, bufferSize * sizeof(value_type));
                buffer.get()[bufferSize] = '\0';

                delete[] this->_front;
                this->_front = buffer.release();
                this->_back = this->_front + bufferSize;
                this->_bounds = this->_front + capacity;
            }
        }

        inline size_t BufferSize(void) const
        {
            // Size (in units of value_type) of the string *NOT* inluding the null character. I.e. the size that we
            // need to copy on resize
            return (this->_back - this->_front);
        }

        inline size_t Capacity(void) const
        {
            // Size of our internal buffer (includes the null character)
            return (this->_bounds - this->_front);
        }

        size_t _length;
        value_type *_front;     // Always points at first character
        value_type *_back;      // Always points at the null character
        value_type *_bounds;    // Always points one past the last location we can write to
    };



    /*
     * Type Definitions
     */
    using utf8_string = utf_string<char>;
    using utf16_string = utf_string<char16_t>;
    using utf32_string = utf_string<char32_t>;

#pragma endregion
}

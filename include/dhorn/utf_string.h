/*
 * utf_string.h
 *
 * Duncan Horn
 *
 *
 */
#pragma once

#include <cassert>
#include <string>
#include <memory>

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
                !(ch & 0x80) ? 1 :
                !(ch & 0x20) ? 2 :
                !(ch & 0x10) ? 3 :
                !(ch & 0x08) ? 4 : 0;
        }

        inline constexpr size_t size_utf8(_In_ char32_t val)
        {
            return
                (val & 0x00FF0000) ? 4 :
                (val & 0x00FFF800) ? 3 :
                (val & 0x00FFFF80) ? 2 : 1;
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
            // 0x00D800 to 0x00DC00:    INVALID
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
            if (size == 1)
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
                unsigned char mask = masks[bytes];
                size_t shift = (bytes - 1) * 6;

                for (size_t i = 0; i < bytes; ++i)
                {
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
                    pos[0] = 0x0000D800 | ((val >> 10) & 0x03FF);
                    pos[1] = 0x0000DC00 | (val & 0x03FF);
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

    template <typename Traits>
    class utf_string
    {
    public:

        /*
         * Public Type Definitions
         */
        using value_type = typename Traits::value_type;



        /*
         * Constructor(s)/Destructor
         */
        utf_string(void) :
            _length(0)
            _front(nullptr),
            _back(nullptr),
            _bounds(nullptr)
        {
        }

        template <typename CharType>
        utf_string(_In_ const CharType *str)
        {
            this->Create(str);
        }

        template <typename Itr>
        utf_string(_In_ Itr front, _In_ Itr back)
        {
            this->Create(front, back);
        }

        ~utf_string(void)
        {
            this->Destroy();
        }



        /*
         * Public "string-like" functions
         */
        size_t length(void) const
        {
            return this->_length;
        }



    private:

        static const size_t max_char_size = sizeof(char32_t) / sizeof(value_type);

        template <typename CharT>
        static inline constexpr size_t MaxRequiredBufferSize(_In_ size_t bufferLen)
        {
            return bufferLen * ((sizeof(value_type) > sizeof(CharT)) ? 1 : (sizeof(CharT) / sizeof(value_type)));
        }

        //template <typename CharT>
        //static inline size_t StringBufferLen(_In_ const CharT *str)
        //{
        //    // Computes the buffer length. I.e. the maximum string size
        //    size_t len = 0;
        //    while (str[len])
        //    {
        //        ++len;
        //    }

        //    return len;
        //}

        template <typename CharT>
        void Create(_In_ const CharT *str)
        {
            using their_traits = typename garbage::utf_encoding_from_char<CharT>::traits_type;
        }

        template <typename Itr>
        void Create(_In_ Itr front, _In_ Itr back)
        {
            // TODO
            (void)front;
            (void)back;
        }

        inline void Destroy(void)
        {
            delete[] this->_front;

            this->_front = nullptr;
            this->_back = nullptr;
        }

        inline void PushBack(_In_ char32_t ch)
        {
            // Determine if we need to resize
            if ((this->_back + max_char_size) >= this->_bounds)
            {
                this->Resize();
            }

            // Increase length after the write in case an exception is thrown
            this->_back = Traits::write(ch, this->_back);
            ++this->_length;
            assert(this->_back < this->_bounds);
        }

        inline void Resize(void)
        {
            size_t capacity = Capacity();
            size_t bufferSize = BufferSize();
            capacity = capacity ? (capacity * 2) : 8;
            assert(capacity >= (bufferSize + max_char_size + 1));

            // Don't copy the null character since this->_front is null on creation
            std::unique_ptr<value_type[]> buffer(new value_type[capacity]);
            memcpy(buffer.get(), this->_front, bufferSize * sizeof(value_type));
            buffer.get()[bufferSize] = '\0';

            delete[] this->_front;
            this->_front = buffer.release();
            this->_back = this->_front + bufferSize;
            this->_bounds = this->_front + capacity;
        }

        inline size_t BufferSize(void)
        {
            // Size (in units of value_type) of the buffer *NOT* inluding the null character. I.e. the size that we
            // need to copy on resize
            return (this->_back - this->_front);
        }

        inline size_t Capacity(void)
        {
            // Size of our internal buffer
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
    using utf8_string = utf_string<garbage::utf8_traits>;
    using utf16_string = utf_string<garbage::utf16_traits>;
    using utf32_string = utf_string<garbage::utf32_traits>;

#pragma endregion
}

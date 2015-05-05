/*
 * utf_string.h
 *
 * Duncan Horn
 *
 *
 */
#pragma once

#include <cstdint>
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
                (val & 0xFF0000) ? 4 :
                (val & 0xFFF800) ? 3 :
                (val & 0xFFFF80) ? 2 : 1;
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

        inline char32_t read_utf16(_In_ const char16_t *str, _Out_opt_ const char16_t **result)
        {
            // TODO
            (void)str;
            (void)result;
            return 0;
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

            static inline char32_t next(_In_ const char *pos, _Out_opt_ const char **output)
            {
                return read_utf8(pos, output);
            }

            static inline unsigned char *write(_In_ char32_t val, _Out_ unsigned char *pos)
            {
                size_t bytes = size_utf8(val);

                if (val > 0x10FFFF)
                {
                    throw bad_utf_encoding(encoding, val);
                }

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

            static inline constexpr size_t size(_In_ char val)
            {
                return size_utf8(val);
            }

            static inline std::pair<value_type *, value_type *> create(_In_ const char *str)
            {
                // If the input is a char * string, then we assume either (1) it's a normal ANSI string, or (2) it's
                // already a utf-8 string. In either case, this is just a memcpy.
                auto len = strlen(str);
                std::unique_ptr<value_type[]> ptr(new value_type[len + 1]);

                memcpy(ptr.get(), str, len);
                ptr[len] = '\0';

                auto front = ptr.release();
                return std::make_pair(front, front + len);
            }
        };



        /*
         * Traits Type Definitions
         */
        using utf8_traits = utf_traits<utf_encoding::utf_8>;

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
            _front(nullptr),
            _back(nullptr)
        {
        }

        utf_string(_In_ const value_type *str)
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



    private:

        void Create(_In_ const value_type *str)
        {
            // TODO
            (void)str;
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

        value_type *_front;
        value_type *_back;
    };



    /*
     * Type Definitions
     */
    using utf8_string = utf_string<garbage::utf8_traits>;

#pragma endregion
}

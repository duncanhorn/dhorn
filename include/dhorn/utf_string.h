/*
 * utf_string.h
 *
 * Duncan Horn
 *
 *
 */
#pragma once

#include <string>

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
         * UTF Traits
         */
#pragma region UTF Traits

        template <utf_encoding Encoding>
        struct utf_traits;

        template <>
        struct utf_traits<utf_encoding::utf_8>
        {
            static const utf_encoding encoding = utf_encoding::utf_8;
            using value_type = unsigned char;

            static inline char32_t next(_In_ const unsigned char *pos, _Out_opt_ const unsigned char **output)
            {
                size_t bytes = size(*pos);

                if (bytes == 0)
                {
                    throw bad_utf_encoding(encoding, *pos);
                }

                unsigned char mask = 0xFF >> bytes;
                char32_t val = 0;

                for (size_t i = 0; i < bytes; ++i)
                {
                    val = (val << 6) | (pos[i] & mask);
                    mask = 0x3F;
                }

                if (output)
                {
                    *output = pos + bytes;
                }

                return val;
            }

            static inline unsigned char *write(_In_ char32_t val, _Out_ unsigned char *pos)
            {
                size_t bytes =
                    (val & 0xFF0000) ? 4 :
                    (val & 0xFFF800) ? 3 :
                    (val & 0xFFFF80) ? 2 : 1;

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

            static inline constexpr size_t size(_In_ unsigned char val)
            {
                // Ignore checking for 10xxxxxx since there's no point in doing it here
                return
                    ((val & 0x80) == 0x00) ? 1 :
                    ((val & 0xE0) == 0xC0) ? 2 :
                    ((val & 0xF0) == 0xE0) ? 3 :
                    ((val & 0xF8) == 0xF0) ? 4 : 0;
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

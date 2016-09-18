/*
 * Duncan Horn
 *
 * utf_iterator.h
 *
 * An iterator approach for converting between strings with different utf encodings. The goal is to achieve something
 * like the following:
 *
 *      std::string u8str = u8"some text here";
 *      std::u16string u16str(utf8_iterator(std::begin(u8str)), utf8_iterator(std::end(u8str)));
 *
 * 
 */
#pragma once

#include <iterator>

#include "type_traits.h"

namespace dhorn
{
    namespace experimental
    {
        /*
         * bad_encoding
         */
        class bad_encoding final :
            public std::exception
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            template <typename CharT>
            bad_encoding(CharT value) :
                _value(value),
                _typeSize(sizeof(value))
            {
                sprintf_s(this->_msg, "bad_encoding: [value: U+06X] [type: %s]", this->_value, type_name());
            }



            /*
             * std::exception
             */
            const char *what(void) const override
            {
                return this->_msg;
            }



            /*
             * Properties
             */
            char32_t value(void) const
            {
                return this->_value;
            }

            const char *type_name(void) const
            {
                return
                    (this->_typeSize == 1) ? "char" :
                    (this->_typeSize == 2) ? "char16_t" :
                    (this->_typeSize == 4) ? "char32_t" : "unknown";
            }



        private:

            char32_t _value;
            size_t _typeSize;
            char _msg[50];
        };



        /*
         * exception_encoding_error_policy
         */
        template <typename CharT>
        class exception_encoding_error_policy
        {
        public:

            /*
             * Operators
             */
            void operator()(CharT value) const
            {
                throw bad_encoding(value);
            }
        };



        /*
         * max_code_point_size
         */
#pragma region max_code_point_size

        template <typename CharT>
        struct max_code_point_size;

        template <>
        struct max_code_point_size<char>
        {
            static const size_t value = 4;
        };

        template <>
        struct max_code_point_size<char16_t>
        {
            static const size_t value = 2;
        };

        template <>
        struct max_code_point_size<char32_t>
        {
            static const size_t value = 1;
        };

        template <typename CharT>
        constexpr size_t max_code_point_size_v = max_code_point_size<CharT>::value;

#pragma endregion



        /*
         * code_point_size
         */
#pragma region code_point_size

        inline constexpr size_t code_point_size(char value)
        {
            return ((value & 0x80) == 0) ? 1 :
                ((value & 0xE0) == 0xC0) ? 2 :
                ((value & 0xF0) == 0xE0) ? 3 :
                ((value & 0xF8) == 0xF0) ? 4 : 0;
        }

        inline constexpr size_t code_point_size(char16_t value)
        {
            return ((value & 0xFC00) == 0xD800) ? 2 : 1;
        }

        inline constexpr size_t code_point_size(char32_t /*value*/)
        {
            return 1;
        }

#pragma endregion



        /*
         * utf_iterator
         */
        template <typename CharTo, typename FromItr, typename ErrPolicy = exception_encoding_error_policy<CharTo>>
        class utf_iterator :
            public std::iterator<std::forward_iterator_tag, CharTo, std::ptrdiff_t, const CharTo *, const CharTo &>
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            utf_iterator(const FromItr &itr) :
                _itr(itr)
            {
            }



        private:

            /*
             * State.
             */
            FromItr _itr;
            CharTo _next[max_code_point_size_v<CharTo>];
            size_t _nextIndex = max_code_point_size_v<CharTo>;

            ErrPolicy _errPolicy;
        };



        /*
         * utf_iterator Creation Helpers
         */
#pragma region utf_iterator Creation Helpers



#pragma endregion
    }
}

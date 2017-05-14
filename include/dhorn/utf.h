/*
 * Duncan Horn
 *
 * utf.h
 *
 * 
 */
#pragma once

#include <cstddef>
#include <utility>

namespace dhorn
{
    /*
     * utf_traits
     */
#pragma region utf_traits

    template <typename CharTy>
    struct utf_traits;

    template <>
    struct utf_traits<char>
    {
        using char_type = char;
        using pointer = char_type*;
        using reference = char_type&;

        static const std::size_t max_code_point_size = 4;

        static constexpr size_t code_point_size(char_type ch)
        {
            return
                ((ch & 0x80) == 0x00) ? 1 :
                ((ch & 0xE0) == 0xC0) ? 2 :
                ((ch & 0xF0) == 0xE0) ? 3 : 4;
        }

        static constexpr size_t code_point_size(char32_t ch)
        {
            return
                (ch <= 0x00007F) ? 1 :
                (ch <= 0x0007FF) ? 2 :
                (ch <= 0x00FFFF) ? 3 : 4;
        }

        static constexpr bool is_initial_code_unit(char_type ch)
        {
            return (ch & 0xC0) != 0x80;
        }

        template <typename Itr>
        static constexpr std::pair<char32_t, Itr> read(Itr pos)
        {
            const auto size = code_point_size(*pos);

            // The first mask has '0's in the highest 'size' bits
            char32_t result = *pos & (0xFF >> size);
            ++pos;

            for (size_t i = 1; i < size; ++i)
            {
                result = (result << 6) | (*pos & 0x3F);
                ++pos;
            }

            return std::make_pair(result, pos);
        }

        //template <typename Itr>
        //static constexpr Itr write(Itr pos, char32_t ch)
        //{

        //}
    };

    template <>
    struct utf_traits<char16_t>
    {
        using char_type = char16_t;
        using pointer = char_type*;
        using reference = char_type&;

        static const std::size_t max_code_point_size = 2;

        static constexpr size_t code_point_size(char_type ch)
        {
            return ((ch & 0xF800) != 0xD800) ? 1 : 2;
        }

        static constexpr size_t code_point_size(char32_t ch)
        {
            // Code points U+D800 through U+DFFF are reserved and unassigned, meaning that we don't need to worry about
            // representation of surrogates
            return (ch <= 0x00FFFF) ? 1 : 2;
        }

        static constexpr bool is_initial_code_unit(char_type ch)
        {
            return (ch & 0xFC00) != 0xDC00;
        }

        template <typename Itr>
        static constexpr std::pair<char32_t, Itr> read(Itr pos)
        {
            if (code_point_size(*pos) == 2)
            {
                // The two code units are:
                // 1101 10XX XXXX XXXX
                // 1101 11XX XXXX XXXX
                char32_t result = (*pos & 0x03FF) << 10;
                ++pos;
                result = result | (*pos & 0x03FF);
                ++pos;
                result += 0x0001'0000;
                return std::make_pair(result, pos);
            }
            else
            {
                char32_t result = *pos;
                ++pos;
                return std::make_pair(result, pos);
            }
        }

        //template <typename Itr>
        //static constexpr Itr write(Itr pos, char32_t ch)
        //{

        //}
    };

    template <>
    struct utf_traits<char32_t>
    {
        using char_type = char32_t;
        using pointer = char_type*;
        using reference = char_type&;

        static const std::size_t max_code_point_size = 1;

        static constexpr size_t code_point_size(char_type /*ch*/)
        {
            return 1;
        }

        static constexpr bool is_initial_code_unit(char_type /*ch*/)
        {
            return true;
        }

        template <typename Itr>
        static constexpr std::pair<char32_t, Itr> read(Itr pos)
        {
            char32_t result = *pos;
            ++pos;
            return std::make_pair(result, pos);
        }

        template <typename Itr>
        static constexpr Itr write(Itr pos, char32_t ch)
        {
            *pos = ch;
            ++pos;
            return pos;
        }
    };

#pragma endregion
}

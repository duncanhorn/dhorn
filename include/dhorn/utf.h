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

            switch (size)
            {
            case 4:
                result = (result << 6) | (*pos & 0x3F);
                ++pos;
                [[fallthrough]];

            case 3:
                result = (result << 6) | (*pos & 0x3F);
                ++pos;
                [[fallthrough]];

            case 2:
                result = (result << 6) | (*pos & 0x3F);
                ++pos;
            }

            return std::make_pair(result, pos);
        }

        template <typename Itr>
        static constexpr Itr write(Itr pos, char32_t ch)
        {
            // The only thing "special" that we need to worry about is the initial character since the masks and bit
            // count are different. After that we can optimize more
            static constexpr unsigned char prefixes[] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0 };
            static constexpr size_t bitCounts[] = { 0, 7, 5, 4, 3 };
            static constexpr unsigned char masks[] = { 0x00, 0x7F, 0x1F, 0x0F, 0x07 };

            const auto size = code_point_size(ch);
            auto prefix = prefixes[size];
            auto mask = masks[size];
            size_t shift = 6 * (size - 1);

            *pos = static_cast<char_type>(((ch >> shift) & mask) | prefix);
            ++pos;

            // From here out, the mask and prefix are constant
            prefix = 0x80;
            mask = 0x3F;
            switch (size)
            {
            case 4:
                shift -= 6;
                *pos = static_cast<char_type>(((ch >> shift) & mask) | prefix);
                ++pos;
                [[fallthrough]];

            case 3:
                shift -= 6;
                *pos = static_cast<char_type>(((ch >> shift) & mask) | prefix);
                ++pos;
                [[fallthrough]];

            case 2:
                shift -= 6;
                *pos = static_cast<char_type>(((ch >> shift) & mask) | prefix);
                ++pos;
            }

            return pos;
        }
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

        template <typename itr>
        static constexpr itr write(itr pos, char32_t ch)
        {
            if (code_point_size(ch) == 1)
            {
                *pos = static_cast<char_type>(ch);
                ++pos;
            }
            else
            {
                ch -= 0x0001'0000;
                *pos = static_cast<char_type>(0xD800 | ((ch >> 10) & 0x03FF));
                ++pos;
                *pos = static_cast<char_type>(0xDC00 | (ch & 0x03FF));
                ++pos;
            }

            return pos;
        }
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

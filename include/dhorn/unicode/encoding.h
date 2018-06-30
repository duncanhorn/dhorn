/*
 * Duncan Horn
 *
 * encoding.h
 *
 *
 */
#pragma once

#include <cassert>

namespace dhorn::unicode
{
    // TODO: std::endian when available
    namespace details
    {
        enum class endian
        {
            little,
            big,

#if _M_IX86 || _M_AMD64
            native = little,
#else // TODO. For now, assume ARM
            native = big,
#endif
        };
    }



    // TODO: Make name something like invalid_char?
    constexpr char32_t eof = static_cast<char32_t>(-1);
    constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();



    /*
     * encoding
     *
     * Only includes the encodings that are part of the Unicode standard (i.e. excludes UTF-1, UTF-7, UTF-EBCDIC, etc.),
     * with endianness
     */
    enum class encoding
    {
        utf_8 = 0,       // UTF-8

        utf_16le = 1,   // UTF-16, little endian
        utf_16be = 2,   // UTF-16, big endian
        utf_16 = (details::endian::native == details::endian::little) ? utf_16le : utf_16be,

        utf_32le = 3,   // UTF-32, little endian
        utf_32be = 4,   // UTF-32, big endian
        utf_32 = (details::endian::native == details::endian::little) ? utf_32le : utf_32be,
    };



    /*
     * character_encoding
     */
#pragma region character_encoding

    template <typename CharTy>
    struct character_encoding;

    template <typename CharTy>
    constexpr encoding character_encoding_v = character_encoding<CharTy>::value;

    template <>
    struct character_encoding<char> :
        std::integral_constant<encoding, encoding::utf_8>
    {
    };

    template <>
    struct character_encoding<char16_t> :
        std::integral_constant<encoding, encoding::utf_16>
    {
    };

    template <>
    struct character_encoding<char32_t> :
        std::integral_constant<encoding, encoding::utf_32>
    {
    };

#pragma endregion



    /*
     * Code Point Validation
     */
#pragma region Code Point Validation

    inline constexpr std::size_t plane(char32_t ch) noexcept
    {
        return (ch >> 16);
    }

    inline constexpr bool is_high_surrogate(char32_t ch) noexcept
    {
        return (ch & 0xFC00) == 0xD800;
    }

    inline constexpr bool is_low_surrogate(char32_t ch) noexcept
    {
        return (ch & 0xFC00) == 0xDC00;
    }

    inline constexpr bool is_noncharacter(char32_t ch) noexcept
    {
        // Unused code points, mostly for BOM identification
        return ((ch & 0xFFFE) == 0xFFFE) || ((ch >= 0xFDD0) && (ch <= 0xFDEF));
    }

    inline constexpr bool is_private_use(char32_t ch) noexcept
    {
        return ((ch >= 0x00'E000) && (ch <= 0x00'F8FF)) ||
               ((ch >= 0x0F'0000) && (ch <= 0x0F'FFFD)) ||
               ((ch >= 0x10'0000) && (ch <= 0x10'FFFD));
    }

    inline constexpr bool is_control_code(char32_t ch) noexcept
    {
        return (ch <= 0x001F) || ((ch >= 0x007F) && (ch <= 0x009F));
    }

    inline constexpr bool is_valid_code_point(char32_t ch) noexcept
    {
        // Not a surrogate and in plane 0-16
        return ((ch & 0xFFFF'F800) != 0xD800) && (ch < 0x11'0000);
    }

#pragma endregion



    /*
     * string_length
     *
     * Used in places that calculate/return a string length, differentiating the number of code points from the number
     * of code units that are in the string
     */
    struct string_length
    {
        std::size_t code_points;
        std::size_t code_units;
    };

    namespace details
    {
        template <typename Traits, typename Itr>
        inline constexpr string_length length(Itr str) noexcept
        {
            string_length result = {};
            while (*str)
            {
                auto units = Traits::code_point_size(*str);
                if constexpr (Traits::is_checked)
                {
                    if (!units)
                    {
                        // TODO? What's the best way to indicate error? For now, we'll keep code_units unmodified so
                        // that the caller knows where the error occurred, but give "-1" back for code_points
                        result.code_points = npos;
                        break;
                    }
                }

                result.code_units += units;
                ++result.code_points;

                std::advance(str, units);
            }

            return result;
        }
    }



    /*
     * encoding_traits
     */
#pragma region encoding_traits

    template <encoding Encoding, bool Validate = false>
    struct encoding_traits;

    template <bool Validate>
    struct encoding_traits<encoding::utf_8, Validate>
    {
        using value_type = char;
        using pointer = char*;
        using const_pointer = const char*;

        static constexpr bool is_checked = Validate;
        static constexpr int max_code_point_size = 4;
        static constexpr char byte_order_mark[] = { '\xEF', '\xBB', '\xBF' };

        static constexpr std::size_t code_point_size(value_type ch) noexcept
        {
            // One byte:    0xxx xxxx
            // Two bytes:   110x xxxx
            // Three bytes: 1110 xxxx
            // Four bytes:  1111 0xxx
            return ((ch & 0x80) == 0x00) ? 1 :
                   ((ch & 0xE0) == 0xC0) ? 2 :
                   ((ch & 0xF0) == 0xE0) ? 3 :
                   !Validate ? 4 :
                   ((ch & 0xF8) == 0xF0) ? 4 : 0;
        }

        static constexpr std::size_t code_point_size(char32_t ch) noexcept
        {
            if constexpr (Validate)
            {
                if (!is_valid_code_point(ch))
                {
                    return 0;
                }
            }

            // One byte: 7 bits (000000-00007F)
            // Two bytes: 11 bits (000080-0007FF)
            // Three bytes: 16 bits (000800-00FFFF), with a hole from 00D800-00DFFF
            // Four bytes: 21 bits, but 10FFFF is the max (010000-10FFFF)
            return (ch < 0x00'0080) ? 1 :
                   (ch < 0x00'0800) ? 2 :
                   (ch < 0x01'0000) ? 3 : 4;
        }

        static constexpr bool is_initial_code_unit(value_type ch) noexcept
        {
            return (ch & 0xC0) != 0x80;
        }

        template <typename InputItr>
        static constexpr InputItr next(InputItr pos) noexcept
        {
            std::advance(pos, code_point_size(*pos));
            return pos;
        }

        template <typename InputItr>
        static constexpr std::pair<char32_t, InputItr> read(InputItr pos) noexcept
        {
            const auto initialValue = *pos;
            const auto size = code_point_size(static_cast<value_type>(initialValue));

            if constexpr (Validate)
            {
                if (size == 0)
                {
                    return { eof, pos };
                }
            }

            // First byte is either 0xxx'xxxx, 110x'xxxx, 1110'xxxx, or 1111'0xxx. In all cases, we only need to mask
            // away the first 'size' bits, since all others are either data or assumed to be zero
            char32_t result = initialValue & (0xFF >> size);
            switch (size)
            {
            case 4:
                result = (result << 6) | (*++pos & 0x3F);
                if constexpr (Validate)
                {
                    // TODO: Early return ensures that we return an iterator to the bad character, but this is likely
                    // less efficient than keeping a running "has error occurred" that we later make sense of. This is
                    // likely the most useful to consumers, but impact should be understood
                    if ((*pos & 0xC0) != 0x80)
                    {
                        return { eof, pos };
                    }
                }
                [[fallthrough]];
            case 3:
                result = (result << 6) | (*++pos & 0x3F);
                if constexpr (Validate)
                {
                    if ((*pos & 0xC0) != 0x80)
                    {
                        return { eof, pos };
                    }
                }
                [[fallthrough]];
            case 2:
                result = (result << 6) | (*++pos & 0x3F);
                if constexpr (Validate)
                {
                    if ((*pos & 0xC0) != 0x80)
                    {
                        return { eof, pos };
                    }
                }
            }

            if constexpr (Validate)
            {
                if (!is_valid_code_point(result))
                {
                    result = eof;
                }
            }

            return { result, ++pos };
        }

        template <typename Itr>
        static constexpr Itr write(Itr pos, char32_t ch) noexcept
        {
            // The only thing "special" that we need to worry about is the initial character since the masks and bit
            // count are different. After that we can optimize more
            constexpr unsigned char prefixes[] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0 };
            constexpr unsigned char masks[] = { 0x00, 0x7F, 0x1F, 0x0F, 0x07 };
            constexpr std::size_t shifts[] = { 0, 0, 6, 12, 18 };

            const auto size = code_point_size(ch);
            if constexpr (Validate)
            {
                if (!size)
                {
                    return pos;
                }
            }

            auto shift = shifts[size];
            *pos++ = static_cast<value_type>(((ch >> shift) & masks[size]) | prefixes[size]);

            switch (size)
            {
            case 4:
                shift -= 6;
                *pos++ = static_cast<value_type>(((ch >> shift) & 0x3F) | 0x80);
                [[fallthrough]];
            case 3:
                shift -= 6;
                *pos++ = static_cast<value_type>(((ch >> shift) & 0x3F) | 0x80);
                [[fallthrough]];
            case 2:
                shift -= 6;
                *pos++ = static_cast<value_type>(((ch >> shift) & 0x3F) | 0x80);
            }

            assert(shift == 0);
            return pos;
        }

        template <typename Itr>
        static constexpr string_length length(Itr str) noexcept
        {
            return details::length<encoding_traits>(str);
        }
    };



    namespace details
    {
        template <endian Endianness, bool Validate>
        struct utf16_traits_impl
        {
            using value_type = char16_t;
            using pointer = char16_t*;
            using const_pointer = const char16_t*;

            static constexpr bool is_checked = Validate;
            static constexpr int max_code_point_size = 2;
            static constexpr char byte_order_mark[] =
            {
                (Endianness == endian::little) ? '\xFF' : '\xFE',
                (Endianness == endian::little) ? '\xFE' : '\xFF'
            };

            // Converts from source endianness to host endianness
            static constexpr value_type normalize(value_type value) noexcept
            {
                if constexpr (Endianness != endian::native)
                {
                    // Swap the bytes
                    value = ((value & 0x00FF) << 8) | ((value >> 8) & 0x00FF);
                }

                return value;
            }

            static constexpr std::size_t code_point_size(value_type value) noexcept
            {
                // One character: 0000-D7FF and E000-FFFF
                // Two characters: 1101 10xx xxxx xxxx
                if constexpr (Validate)
                {
                    if (!is_initial_code_unit(value))
                    {
                        // Low surrogate
                        return 0;
                    }
                }

                return ((value & normalize(0xFC00)) == normalize(0xD800)) ? 2 : 1;
            }

            // NOTE: Input is in the host's endian-ness, so no converion is necessary
            static constexpr std::size_t code_point_size(char32_t ch) noexcept
            {
                if constexpr (Validate)
                {
                    if (!is_valid_code_point(ch))
                    {
                        return 0;
                    }
                }

                // One character: 0000-D7FF and E000-FFFF
                // Two characters: 01'0000-10'FFFF
                return (ch & 0xFFFF'0000) ? 2 : 1;
            }

            static constexpr bool is_initial_code_unit(value_type ch) noexcept
            {
                return (ch & normalize(0xFC00)) != normalize(0xDC00);
            }

            template <typename InputItr>
            static constexpr InputItr next(InputItr pos) noexcept
            {
                std::advance(pos, code_point_size(*pos));
                return pos;
            }

            template <typename InputItr>
            static constexpr std::pair<char32_t, InputItr> read(InputItr pos) noexcept
            {
                const auto initialValue = *pos;
                const auto size = code_point_size(static_cast<value_type>(initialValue));

                if constexpr (Validate)
                {
                    if (!size)
                    {
                        return { eof, pos };
                    }
                }

                char32_t result = normalize(initialValue);
                if (size == 2)
                {
                    result = (result & 0x03FF) << 10;
                    result |= normalize(*++pos) & 0x03FF;
                    result += 0x0001'0000;

                    if constexpr (Validate)
                    {
                        if (((*pos & normalize(0xFC00)) != normalize(0xDC00)) || !is_valid_code_point(result))
                        {
                            return { eof, pos };
                        }
                    }
                }

                return { result, ++pos };
            }

            template <typename OutputItr>
            static constexpr OutputItr write(OutputItr pos, char32_t ch) noexcept
            {
                const auto size = code_point_size(ch);

                if constexpr (Validate)
                {
                    if (!size)
                    {
                        return pos;
                    }
                }

                if (size == 1)
                {
                    *pos++ = normalize(static_cast<value_type>(ch));
                }
                else
                {
                    assert(size == 2);
                    ch -= 0x0001'0000;
                    *pos++ = normalize(static_cast<value_type>(0xD800 | ((ch >> 10) & 0x03FF)));
                    *pos++ = normalize(static_cast<value_type>(0xDC00 | (ch & 0x03FF)));
                }

                return pos;
            }

            template <typename Itr>
            static constexpr string_length length(Itr str) noexcept
            {
                return details::length<utf16_traits_impl>(str);
            }
        };
    }

    // NOTE: One of the following will account for encoding::utf_16
    template <bool Validate>
    struct encoding_traits<encoding::utf_16le, Validate> :
        public details::utf16_traits_impl<details::endian::little, Validate>
    {
    };

    template <bool Validate>
    struct encoding_traits<encoding::utf_16be, Validate> :
        public details::utf16_traits_impl<details::endian::big, Validate>
    {
    };



    namespace details
    {
        template <endian Endianness, bool Validate>
        struct utf32_traits_impl
        {
            using value_type = char32_t;
            using pointer = char32_t*;
            using const_pointer = const char32_t*;

            static constexpr bool is_checked = Validate;
            static constexpr int max_code_point_size = 1;
            static constexpr char byte_order_mark[] =
            {
                (Endianness == endian::little) ? '\xFF' : '\x00',
                (Endianness == endian::little) ? '\xFE' : '\x00',
                (Endianness == endian::little) ? '\x00' : '\xFE',
                (Endianness == endian::little) ? '\x00' : '\xFF'
            };

            // Converts from source endianness to host endianness
            static constexpr value_type normalize(value_type value) noexcept
            {
                if constexpr (Endianness != endian::native)
                {
                    value = ((value & 0x000000FF) << 24) |
                            ((value & 0x0000FF00) << 8) |
                            ((value >> 8) & 0x0000FF00) |
                            ((value >> 24) & 0x000000FF);
                }

                return value;
            }

            static constexpr std::size_t code_point_size([[maybe_unused]] value_type value) noexcept
            {
                if constexpr (Validate)
                {
                    if (!is_valid_code_point(normalize(value)))
                    {
                        return 0;
                    }
                }

                return 1;
            }

            static constexpr bool is_initial_code_unit(value_type) noexcept
            {
                return true;
            }

            template <typename InputItr>
            static constexpr InputItr next(InputItr pos) noexcept
            {
                return ++pos;
            }

            template <typename InputItr>
            static constexpr std::pair<char32_t, InputItr> read(InputItr pos) noexcept
            {
                auto result = normalize(*pos);
                if constexpr (Validate)
                {
                    if (!is_valid_code_point(result))
                    {
                        return { eof, pos };
                    }
                }

                return { result, ++pos };
            }

            template <typename OutputItr>
            static constexpr OutputItr write(OutputItr pos, char32_t ch) noexcept
            {
                if constexpr (Validate)
                {
                    if (!is_valid_code_point(ch))
                    {
                        return pos;
                    }
                }

                *pos++ = normalize(ch);
                return pos;
            }

            template <typename Itr>
            static constexpr string_length length(Itr str) noexcept
            {
                return details::length<utf32_traits_impl>(str);
            }
        };
    }

    // NOTE: One of the following will account for encoding::utf_32
    template <bool Validate>
    struct encoding_traits<encoding::utf_32le, Validate> :
        public details::utf32_traits_impl<details::endian::little, Validate>
    {
    };

    template <bool Validate>
    struct encoding_traits<encoding::utf_32be, Validate> :
        public details::utf32_traits_impl<details::endian::big, Validate>
    {
    };

#pragma endregion



    /*
     * char_traits
     *
     * Maps a character type to encoding(s)
     */
#pragma region char_traits

    template <typename CharTy>
    struct char_traits;

    template <>
    struct char_traits<char>
    {
        static constexpr encoding native_encoding = encoding::utf_8;
        static constexpr encoding little_endian_encoding = encoding::utf_8;
        static constexpr encoding big_endian_encoding = encoding::utf_8;
    };

    template <>
    struct char_traits<char16_t>
    {
        static constexpr encoding native_encoding = encoding::utf_16;
        static constexpr encoding little_endian_encoding = encoding::utf_16le;
        static constexpr encoding big_endian_encoding = encoding::utf_16be;
    };

    template <>
    struct char_traits<char32_t>
    {
        static constexpr encoding native_encoding = encoding::utf_32;
        static constexpr encoding little_endian_encoding = encoding::utf_32le;
        static constexpr encoding big_endian_encoding = encoding::utf_32be;
    };

#pragma endregion
}

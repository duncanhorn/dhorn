/*
 * Duncan Horn
 *
 * numeric.h
 *
 * Various functions that deal with numeric values (either integral or not). The input of each function is expected to
 * be of the form:
 *
 *  [+/-][digits...][.][digits...][e/E][+/-][digits...]
 */
#pragma once

#include <string>

#include "utf_string.h"

namespace dhorn
{
    namespace garbage
    {
        template <
            typename NumericType,
            typename = std::enable_if<std::is_signed<NumericType>::value>::type>
        inline constexpr void negate(_In_ NumericType val)
        {
            return -val;
        }

        template <
            typename NumericType,
            typename = std::enable_if<std::is_unsigned<NumericType>::value>::type>
        inline constexpr void negate(_In_ NumericType val)
        {
            throw std::invalid_argument("Cannot negate an unsigned type");
        }

        template <typename CharT>
        inline constexpr bool is_digit(_In_ CharT ch) noexcept
        {
            return (ch >= '0') && (ch <= '9');
        }

        template <typename NumericType, typename CharT>
        inline constexpr NumericType to_digit(_In_ CharT ch) noexcept
        {
            return static_cast<NumericType>(ch - '0');
        }

        template <typename CharT>
        inline constexpr bool is_sign(_In_ CharT ch) noexcept
        {
            return (ch == '+') || (ch == '-');
        }

        template <typename CharT>
        inline constexpr bool is_decimal(_In_ CharT ch) noexcept
        {
            return ch == '.';
        }

        template <typename CharT>
        inline constexpr bool is_exponential(_In_ CharT ch) noexcept
        {
            return (ch == 'e') || (ch == 'E');
        }

        template <typename NumericType, typename CharT>
        NumericType read_single_number(_Inout_ const CharT **str)
        {
            NumericType value = {};
            for (; is_digit(**str); ++(*str))
            {
                value = (value * 10) + to_digit<NumericType>(**str);
            }

            return value;
        }
    }



    /*
     * String literal conversion
     */
#pragma region String Literal Conversion

    template <typename NumericType, typename CharT>
    NumericType numeric_cast(_In_ const CharT *str)
    {
        NumericType integer_part;
        NumericType fractional_part = {};
        intmax_t exponent_part = 0;

        // Check for negative value. If the type is unsigned, we'll throw later
        bool negative = *str == static_cast<CharT>('-');

        // Skip past any sign that might be present
        if (garbage::is_sign(*str))
        {
            ++str;
        }

        // Read the digits for the integer part
        integer_part = garbage::read_single_number(&str);

        // Negate if necessary
        if (negative)
        {
            integer_part = garbage::negate(integer_part);
        }

        if (garbage::is_decimal(*str))
        {
            ++str;

            // Read the digits for the fractional part
            fractional_part = garbage::read_single_number(&str);
        }

        if (garbage::is_exponential(*str))
        {
            ++str;


        }
    }

#pragma endregion
}

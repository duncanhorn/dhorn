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
        template <typename NumericType>
        inline constexpr typename std::enable_if<std::is_signed<NumericType>::value, NumericType>::type
        negate(_In_ NumericType val)
        {
            return -val;
        }

        template <typename NumericType>
        inline constexpr typename std::enable_if<std::is_unsigned<NumericType>::value, NumericType>::type
        negate(_In_ NumericType)
        {
            throw std::invalid_argument("Cannot negate an unsigned type");
        }

        template <typename CharT>
        inline constexpr bool is_digit(_In_ CharT ch) noexcept
        {
            return (ch >= '0') && (ch <= '9');
        }

        template <typename CharT>
        inline const CharT *skip_digits(_In_ const CharT *str) noexcept
        {
            for (; is_digit(*str); ++str);
            return str;
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
        inline const CharT *skip_sign(_In_ CharT *str) noexcept
        {
            if (is_sign(*str))
            {
                ++str;
            }
            return str;
        }

        template <typename CharT>
        inline constexpr bool is_decimal(_In_ CharT ch) noexcept
        {
            return ch == '.';
        }

        template <typename CharT>
        inline const CharT *skip_decimal(_In_ CharT *str) noexcept
        {
            if (is_decimal(*str))
            {
                ++str;
            }
            return str;
        }

        template <typename CharT>
        inline constexpr bool is_exponential(_In_ CharT ch) noexcept
        {
            return (ch == 'e') || (ch == 'E');
        }

        template <typename CharT>
        inline const CharT *skip_exponential(_In_ CharT *str) noexcept
        {
            if (is_exponential(*str))
            {
                ++str;
            }
            return str;
        }

        template <typename NumericType, typename CharT>
        const CharT *read_single_number(_In_ const CharT *str, _Inout_ NumericType *value)
        {
            for (; is_digit(*str); ++str)
            {
                *value = ((*value) * 10) + to_digit<NumericType>(*str);
            }

            return str;
        }

        template <typename NumericType, typename CharT>
        const CharT *read_single_number(_In_ const CharT *front, _In_ const CharT *back, _Inout_ NumericType *value)
        {
            for (; front < back; ++front)
            {
                assert(is_digit(*front));
                *value = ((*value) * 10) + to_digit<NumericType>(*front);
            }

            return front;
        }
    }



    /*
     * String literal conversion
     */
#pragma region String Literal Conversion

     template <typename NumericType, typename CharT>
     NumericType numeric_cast(_In_ const CharT *str)
     {
         // Since the string could be of some crazy form like 10000000000000e-13, we need to keep the value in string
         // form for as long as possible and shift the decimal via string pointers
         const CharT *integer_start;
         const CharT *integer_end;
         const CharT *fraction_start;
         const CharT *fraction_end;
         const CharT *string_front = str;

         str = garbage::skip_sign(str);

         // This may yield a string of zero length, which is okay
         integer_start = str;
         str = garbage::skip_digits(str);
         integer_end = str;

         str = garbage::skip_decimal(str);

         // This may yield a string of zero length, which is okay
         fraction_start = str;
         str = garbage::skip_digits(str);
         fraction_end = str;

         // We initially read the exponent as intmax_t since any value whose exponent is greater than
         // std::numeric_limits<intmax_t>::max() is guaranteed to not be representable
         intmax_t exponent = 0;
         if (garbage::is_exponential(*str))
         {
             ++str;

             bool isExponentNegative = (*str == '-');
             str = garbage::skip_sign(str);

             str = garbage::read_single_number<intmax_t>(str, &exponent);
             if (isExponentNegative)
             {
                 exponent = -exponent;
             }
         }

         // We now read an integer part and a fractional part in the resulting part. Since we know the exponent, we can
         // correctly shift the decimal so that if we get integer overflow we know that it is unavoidable and the
         // undefined behavior is okay
         NumericType integer_part = {};
         NumericType fractional_part = {};
         NumericType fractional_divide = 1;

         if (exponent >= 0)
         {
             // All of the integer remains. Note that we still need to multiply by some power of 10. That will come
             // when we read the fraction
             garbage::read_single_number(integer_start, integer_end, &integer_part);

             // Some of fraction goes to integer
             auto fraction_shift = fraction_start + exponent;
             if (fraction_shift > fraction_end)
             {
                 // The whole fraction and then some needs to go to the integer part
                 garbage::read_single_number(fraction_start, fraction_end, &integer_part);

                 auto power = fraction_shift - fraction_end;
                 integer_part *= static_cast<NumericType>(std::pow(static_cast<NumericType>(10), power));
             }
             else
             {
                 // Only part of the fraction needs to go to the integer part
                 garbage::read_single_number(fraction_start, fraction_shift, &integer_part);
                 garbage::read_single_number(fraction_shift, fraction_end, &fractional_part);

                 auto power = fraction_end - fraction_shift;
                 fractional_divide = static_cast<NumericType>(std::pow(static_cast<NumericType>(10), power));
             }
         }
         else
         {
             // The fractional divide will always be the same power, regardless of the integer length
             auto power = (fraction_end - fraction_start) - exponent;
             fractional_divide = static_cast<NumericType>(std::pow(static_cast<NumericType>(10), power));

             // Some of the integer needs to go to the fraction
             auto integer_shift = integer_end + exponent;
             if (integer_start > integer_shift)
             {
                 // The whole integer and then some needs to go to the fractional part. We've already accounted for the
                 // "and then some" above with the fractional_divide
                 garbage::read_single_number(integer_start, integer_end, &fractional_part);
             }
             else
             {
                 // Only part of the integer needs to go to the fractional part
                 garbage::read_single_number(integer_start, integer_shift, &integer_part);
                 garbage::read_single_number(integer_shift, integer_end, &fractional_part);
             }

             garbage::read_single_number(fraction_start, fraction_end, &fractional_part);
         }

         auto result = integer_part + (fractional_part / fractional_divide);

         // Negate if necessary; ignore if zero
         if ((*string_front == '-') && ((integer_part != 0) || (fractional_part != 0)))
         {
             result = garbage::negate(result);
         }

         return result;
     }

#pragma endregion
}

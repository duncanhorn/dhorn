/*
 * Duncan Horn
 *
 * nointrin_traits.h
 *
 * 
 */
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace dhorn::math
{
    namespace details
    {
        /*
         * nointrin_traits_base
         *
         * Without intrinsics, we operate on single scalar values. Thus, most of the operations are the same
         */
        template <typename Ty>
        struct nointrin_traits_base
        {
#pragma region Types/Constants

            using value_type = Ty;
            using vector_type = Ty;

            static constexpr std::size_t size = 1;

#pragma endregion

#pragma region Common Values/Masks

            static constexpr inline vector_type zero() noexcept
            {
                return 0;
            }

#pragma endregion

#pragma region Load/Store

            static constexpr inline vector_type splat(value_type value) noexcept
            {
                return value;
            }

            template <std::size_t Size>
            static constexpr inline vector_type splat(value_type value) noexcept
            {
                static_assert(Size == 1);
                return value;
            }

            static constexpr vector_type set(value_type v) noexcept
            {
                return v;
            }

            template <std::size_t Size, std::size_t... Indices, typename... Args>
            static constexpr void fill(
                std::array<vector_type, Size>& result,
                std::index_sequence<Indices...>,
                Args... args) noexcept
            {
                ((result[Indices] = args), ...);
            }

            template <std::size_t Size, typename... Args, std::enable_if_t<Size == sizeof...(Args), int> = 0>
            static constexpr void fill(std::array<vector_type, Size>& result, Args... args) noexcept
            {
                //result = { args... };
                fill(result, std::make_index_sequence<Size>{}, args...);
            }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

            static constexpr inline vector_type negate(vector_type value) noexcept
            {
                return -value;
            }

            static constexpr inline vector_type add(vector_type lhs, vector_type rhs) noexcept
            {
                return lhs + rhs;
            }

            static constexpr inline vector_type subtract(vector_type lhs, vector_type rhs) noexcept
            {
                return lhs - rhs;
            }

            static constexpr inline vector_type multiply(vector_type lhs, vector_type rhs) noexcept
            {
                return lhs * rhs;
            }

            static constexpr inline vector_type divide(vector_type lhs, vector_type rhs) noexcept
            {
                return lhs / rhs;
            }

#pragma endregion

#pragma region Comparison Operations

            static constexpr inline bool compare_equal(vector_type lhs, vector_type rhs) noexcept
            {
                return lhs == rhs;
            }

            static constexpr inline bool compare_not_equal(vector_type lhs, vector_type rhs) noexcept
            {
                return lhs != rhs;
            }

            static constexpr inline bool compare_less(vector_type lhs, vector_type rhs) noexcept
            {
                return lhs < rhs;
            }

            static constexpr inline bool compare_less_equal(vector_type lhs, vector_type rhs) noexcept
            {
                return lhs <= rhs;
            }

            static constexpr inline bool compare_greater(vector_type lhs, vector_type rhs) noexcept
            {
                return lhs > rhs;
            }

            static constexpr inline bool compare_greater_equal(vector_type lhs, vector_type rhs) noexcept
            {
                return lhs >= rhs;
            }

#pragma endregion
        };
    }



    /*
     * nointrin_traits
     */
    template <typename Ty>
    struct nointrin_traits;



    /*
     * nointrin_traits<char>
     */
    template <>
    struct nointrin_traits<char> :
        public details::nointrin_traits_base<char>
    {
#pragma region Bitwise Operations

        static constexpr inline vector_type bitwise_and(vector_type lhs, vector_type rhs) noexcept
        {
            return lhs & rhs;
        }

        static constexpr inline vector_type bitwise_or(vector_type lhs, vector_type rhs) noexcept
        {
            return lhs | rhs;
        }

        static constexpr inline vector_type bitwise_xor(vector_type lhs, vector_type rhs) noexcept
        {
            return lhs ^ rhs;
        }

        static constexpr inline vector_type bitwise_complement(vector_type v) noexcept
        {
            return ~v;
        }

#pragma endregion
    };



    /*
     * nointrin_traits<short>
     */
    template <>
    struct nointrin_traits<short> :
        public details::nointrin_traits_base<short>
    {
#pragma region Bitwise Operations

        static constexpr inline vector_type bitwise_and(vector_type lhs, vector_type rhs) noexcept
        {
            return lhs & rhs;
        }

        static constexpr inline vector_type bitwise_or(vector_type lhs, vector_type rhs) noexcept
        {
            return lhs | rhs;
        }

        static constexpr inline vector_type bitwise_xor(vector_type lhs, vector_type rhs) noexcept
        {
            return lhs ^ rhs;
        }

        static constexpr inline vector_type bitwise_complement(vector_type v) noexcept
        {
            return ~v;
        }

#pragma endregion
    };



    /*
     * nointrin_traits<std::int32_t>
     */
    template <>
    struct nointrin_traits<std::int32_t> :
        public details::nointrin_traits_base<std::int32_t>
    {
#pragma region Bitwise Operations

        static constexpr inline vector_type bitwise_and(vector_type lhs, vector_type rhs) noexcept
        {
            return lhs & rhs;
        }

        static constexpr inline vector_type bitwise_or(vector_type lhs, vector_type rhs) noexcept
        {
            return lhs | rhs;
        }

        static constexpr inline vector_type bitwise_xor(vector_type lhs, vector_type rhs) noexcept
        {
            return lhs ^ rhs;
        }

        static constexpr inline vector_type bitwise_complement(vector_type v) noexcept
        {
            return ~v;
        }

#pragma endregion
    };



    /*
     * nointrin_traits<std::int64_t>
     */
    template <>
    struct nointrin_traits<std::int64_t> :
        public details::nointrin_traits_base<std::int64_t>
    {
#pragma region Bitwise Operations

        static constexpr inline vector_type bitwise_and(vector_type lhs, vector_type rhs) noexcept
        {
            return lhs & rhs;
        }

        static constexpr inline vector_type bitwise_or(vector_type lhs, vector_type rhs) noexcept
        {
            return lhs | rhs;
        }

        static constexpr inline vector_type bitwise_xor(vector_type lhs, vector_type rhs) noexcept
        {
            return lhs ^ rhs;
        }

        static constexpr inline vector_type bitwise_complement(vector_type v) noexcept
        {
            return ~v;
        }

#pragma endregion
    };



    /*
     * nointrin_traits<float>
     */
    template <>
    struct nointrin_traits<float> :
        public details::nointrin_traits_base<float>
    {
    };



    /*
     * nointrin_traits<double>
     */
    template <>
    struct nointrin_traits<double> :
        public details::nointrin_traits_base<double>
    {
    };
}

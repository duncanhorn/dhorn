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
    /*
     * nointrin_traits
     */
    template <typename Ty>
    struct nointrin_traits;



    /*
     * nointrin_traits<char>
     */
    template <>
    struct nointrin_traits<char>
    {
#pragma region Types/Constants

        using value_type = char;
        using vector_type = char;

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

        static constexpr vector_type set(value_type v) noexcept
        {
            return v;
        }

        template <std::size_t Size, typename... Args, std::enable_if_t<Size == sizeof...(Args), int> = 0>
        static constexpr void fill(std::array<vector_type, Size>& result, Args... args) noexcept
        {
            result = { args... };
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

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



    /*
     * nointrin_traits<short>
     */
    template <>
    struct nointrin_traits<short>
    {
#pragma region Types/Constants

        using value_type = short;
        using vector_type = short;

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

        static constexpr vector_type set(value_type v) noexcept
        {
            return v;
        }

        template <std::size_t Size, typename... Args, std::enable_if_t<Size == sizeof...(Args), int> = 0>
        static constexpr void fill(std::array<vector_type, Size>& result, Args... args) noexcept
        {
            result = { args... };
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

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



    /*
     * nointrin_traits<std::int32_t>
     */
    template <>
    struct nointrin_traits<std::int32_t>
    {
#pragma region Types/Constants

        using value_type = std::int32_t;
        using vector_type = std::int32_t;

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

        static constexpr vector_type set(value_type v) noexcept
        {
            return v;
        }

        template <std::size_t Size, typename... Args, std::enable_if_t<Size == sizeof...(Args), int> = 0>
        static constexpr void fill(std::array<vector_type, Size>& result, Args... args) noexcept
        {
            result = { args... };
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

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



    /*
     * nointrin_traits<std::int64_t>
     */
    template <>
    struct nointrin_traits<std::int64_t>
    {
#pragma region Types/Constants

        using value_type = std::int64_t;
        using vector_type = std::int64_t;

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

        static constexpr vector_type set(value_type v) noexcept
        {
            return v;
        }

        template <std::size_t Size, typename... Args, std::enable_if_t<Size == sizeof...(Args), int> = 0>
        static constexpr void fill(std::array<vector_type, Size>& result, Args... args) noexcept
        {
            result = { args... };
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

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



    /*
     * nointrin_traits<float>
     */
    template <>
    struct nointrin_traits<float>
    {
#pragma region Types/Constants

        using value_type = float;
        using vector_type = float;

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

        static constexpr vector_type set(value_type v) noexcept
        {
            return v;
        }

        template <std::size_t Size, typename... Args, std::enable_if_t<Size == sizeof...(Args), int> = 0>
        static constexpr void fill(std::array<vector_type, Size>& result, Args... args) noexcept
        {
            result = { args... };
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

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



    /*
     * nointrin_traits<double>
     */
    template <>
    struct nointrin_traits<double>
    {
#pragma region Types/Constants

        using value_type = double;
        using vector_type = double;

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

        static constexpr vector_type set(value_type v) noexcept
        {
            return v;
        }

        template <std::size_t Size, typename... Args, std::enable_if_t<Size == sizeof...(Args), int> = 0>
        static constexpr void fill(std::array<vector_type, Size>& result, Args... args) noexcept
        {
            result = { args... };
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

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

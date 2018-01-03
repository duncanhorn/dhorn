/*
 * Duncan Horn
 *
 * sse2_traits.h
 *
 * The definition of the traits type for vectors using the SSE2 instructions
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include <xmmintrin.h> // SSE
#include <emmintrin.h> // SSE2

namespace dhorn::math
{
    /*
     * sse2_traits
     */
    template <typename Ty>
    struct sse2_traits;



    /*
     * sse2_traits<char>
     */
    template <>
    struct sse2_traits<char>
    {
#pragma region Types/Constants

        using value_type = char;
        using vector_type = __m128i;

        static constexpr std::size_t size = sizeof(vector_type) / sizeof(value_type);

#pragma endregion

#pragma region Common Values/Masks

        static inline vector_type zero() noexcept
        {
            return _mm_setzero_si128();
        }

#pragma endregion

#pragma region Load/Store

        static inline vector_type splat(value_type value) noexcept
        {
            return _mm_set1_epi8(value);
        }

        static inline vector_type set(
            value_type v0,  value_type v1,  value_type v2,  value_type v3,
            value_type v4,  value_type v5,  value_type v6,  value_type v7,
            value_type v8,  value_type v9,  value_type v10, value_type v11,
            value_type v12, value_type v13, value_type v14, value_type v15) noexcept
        {
            return _mm_setr_epi8(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15);
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

        static inline vector_type add(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_add_epi8(lhs, rhs);
        }

        static inline vector_type subtract(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_sub_epi8(lhs, rhs);
        }

        //static inline vector_type multiply(vector_type lhs, vector_type rhs) noexcept
        //{
        //    // TODO
        //}

#pragma endregion

#pragma region Bitwise Operations



#pragma endregion

#pragma region Comparison Operations



#pragma endregion
    };



    /*
     * sse2_traits<short>
     */
    template <>
    struct sse2_traits<short>
    {
#pragma region Types/Constants

        using value_type = short;
        using vector_type = __m128i;

        static constexpr std::size_t size = sizeof(vector_type) / sizeof(value_type);

#pragma endregion

#pragma region Common Values/Masks

        static inline vector_type zero() noexcept
        {
            return _mm_setzero_si128();
        }

#pragma endregion

#pragma region Load/Store

        static inline vector_type splat(value_type value) noexcept
        {
            return _mm_set1_epi16(value);
        }

        static inline vector_type set(
            value_type v0, value_type v1, value_type v2, value_type v3,
            value_type v4, value_type v5, value_type v6, value_type v7) noexcept
        {
            return _mm_setr_epi16(v0, v1, v2, v3, v4, v5, v6, v7);
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

        static inline vector_type add(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_add_epi16(lhs, rhs);
        }

        static inline vector_type subtract(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_sub_epi16(lhs, rhs);
        }

        static inline vector_type multiply(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_mullo_epi16(lhs, rhs);
        }

#pragma endregion

#pragma region Bitwise Operations



#pragma endregion

#pragma region Comparison Operations



#pragma endregion
    };



    /*
     * sse2_traits<std::int32_t>
     */
    template <>
    struct sse2_traits<std::int32_t>
    {
#pragma region Types/Constants

        using value_type = std::int32_t;
        using vector_type = __m128i;

        static constexpr std::size_t size = sizeof(vector_type) / sizeof(value_type);

#pragma endregion

#pragma region Common Values/Masks

        static inline vector_type zero() noexcept
        {
            return _mm_setzero_si128();
        }

#pragma endregion

#pragma region Load/Store

        static inline vector_type splat(value_type value) noexcept
        {
            return _mm_set1_epi32(value);
        }

        static inline vector_type set(value_type v0, value_type v1, value_type v2, value_type v3) noexcept
        {
            return _mm_setr_epi32(v0, v1, v2, v3);
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

        static inline vector_type add(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_add_epi32(lhs, rhs);
        }

        static inline vector_type subtract(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_sub_epi32(lhs, rhs);
        }

        //static inline vector_type multiply(vector_type lhs, vector_type rhs) noexcept
        //{
        //    // TODO
        //}

#pragma endregion

#pragma region Bitwise Operations



#pragma endregion

#pragma region Comparison Operations



#pragma endregion
    };



    /*
     * sse2_traits<std::int64_t>
     */
    template <>
    struct sse2_traits<std::int64_t>
    {
#pragma region Types/Constants

        using value_type = std::int64_t;
        using vector_type = __m128i;

        static constexpr std::size_t size = sizeof(vector_type) / sizeof(value_type);

#pragma endregion

#pragma region Common Values/Masks

        static inline vector_type zero() noexcept
        {
            return _mm_setzero_si128();
        }

#pragma endregion

#pragma region Load/Store

        static inline vector_type splat(value_type value) noexcept
        {
            return _mm_set1_epi64x(value);
        }

        static inline vector_type set(value_type v0, value_type v1) noexcept
        {
            // NOTE: _mm_setr_epi64x not available on 32-bit MSVC :(
            return _mm_set_epi64x(v1, v0);
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

        static inline vector_type add(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_add_epi64(lhs, rhs);
        }

        static inline vector_type subtract(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_sub_epi64(lhs, rhs);
        }

        //static inline vector_type multiply(vector_type lhs, vector_type rhs) noexcept
        //{
        //    // TODO
        //}

#pragma endregion

#pragma region Bitwise Operations



#pragma endregion

#pragma region Comparison Operations



#pragma endregion
    };



    /*
     * sse2_traits<float>
     */
    template <>
    struct sse2_traits<float>
    {
#pragma region Types/Constants

        using value_type = float;
        using vector_type = __m128;

        static constexpr std::size_t size = sizeof(vector_type) / sizeof(value_type);

#pragma endregion

#pragma region Common Values/Masks

        static inline vector_type zero() noexcept
        {
            return _mm_setzero_ps();
        }

#pragma endregion

#pragma region Load/Store

        static inline vector_type splat(value_type value) noexcept
        {
            return _mm_set1_ps(value);
        }

        static inline vector_type set(value_type v0, value_type v1, value_type v2, value_type v3) noexcept
        {
            return _mm_setr_ps(v0, v1, v2, v3);
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

        static inline vector_type add(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_add_ps(lhs, rhs);
        }

        static inline vector_type subtract(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_sub_ps(lhs, rhs);
        }

        static inline vector_type multiply(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_mul_ps(lhs, rhs);
        }

#pragma endregion

#pragma region Bitwise Operations



#pragma endregion

#pragma region Comparison Operations



#pragma endregion
    };



    /*
     * sse2_traits<double>
     */
    template <>
    struct sse2_traits<double>
    {
#pragma region Types/Constants

        using value_type = double;
        using vector_type = __m128d;

        static constexpr std::size_t size = sizeof(vector_type) / sizeof(value_type);

#pragma endregion

#pragma region Common Values/Masks

        static inline vector_type zero() noexcept
        {
            return _mm_setzero_pd();
        }

#pragma endregion

#pragma region Load/Store

        static inline vector_type splat(value_type value) noexcept
        {
            return _mm_set1_pd(value);
        }

        static inline vector_type set(value_type v0, value_type v1) noexcept
        {
            return _mm_setr_pd(v0, v1);
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Arithmetic Operations

        static inline vector_type add(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_add_pd(lhs, rhs);
        }

        static inline vector_type subtract(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_sub_pd(lhs, rhs);
        }

        static inline vector_type multiply(vector_type lhs, vector_type rhs) noexcept
        {
            return _mm_mul_pd(lhs, rhs);
        }

#pragma endregion

#pragma region Bitwise Operations



#pragma endregion

#pragma region Comparison Operations



#pragma endregion
    };
}

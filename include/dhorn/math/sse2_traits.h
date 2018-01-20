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
#include <utility>

#include <xmmintrin.h> // SSE
#include <emmintrin.h> // SSE2

#include "../utility.h"

namespace dhorn::math
{
    namespace details
    {
#pragma region Common Masks

        // Helper for generating a constexpr array of mask values
        template <typename Ty, std::size_t... Indices>
        inline constexpr auto sse2_mask_array() noexcept
        {
            static constexpr std::size_t size = 16 / sizeof(Ty);
            static constexpr Ty mask_value = static_cast<Ty>(-1);

            std::array<Ty, size> result = {};
            ((result[Indices] = mask_value), ...);

            return result;
        }

#pragma region 8-bit Masks

        template <std::size_t... Indices>
        inline auto sse_mask8() noexcept
        {
            static_assert(((Indices < 16) && ... && true));

            static constexpr auto masks = sse2_mask_array<char, Indices...>();
            return _mm_setr_epi8(
                masks[0], masks[1], masks[2], masks[3], masks[4], masks[5], masks[6], masks[7],
                masks[8], masks[9], masks[10], masks[11], masks[12], masks[13], masks[14], masks[15]);
        }

        template <std::size_t... Indices>
        inline auto sse_mask8(std::index_sequence<Indices...>) noexcept
        {
            return sse_mask8<Indices...>();
        }

        inline auto sse_mask8_0() noexcept
        {
            return sse_mask8<0>();
        }

        inline auto sse_mask8_1() noexcept
        {
            return sse_mask8<1>();
        }

        inline auto sse_mask8_2() noexcept
        {
            return sse_mask8<2>();
        }

        inline auto sse_mask8_3() noexcept
        {
            return sse_mask8<3>();
        }

        inline auto sse_mask8_4() noexcept
        {
            return sse_mask8<4>();
        }

        inline auto sse_mask8_5() noexcept
        {
            return sse_mask8<5>();
        }

        inline auto sse_mask8_6() noexcept
        {
            return sse_mask8<6>();
        }

        inline auto sse_mask8_7() noexcept
        {
            return sse_mask8<7>();
        }

        inline auto sse_mask8_8() noexcept
        {
            return sse_mask8<8>();
        }

        inline auto sse_mask8_9() noexcept
        {
            return sse_mask8<9>();
        }

        inline auto sse_mask8_10() noexcept
        {
            return sse_mask8<10>();
        }

        inline auto sse_mask8_11() noexcept
        {
            return sse_mask8<11>();
        }

        inline auto sse_mask8_12() noexcept
        {
            return sse_mask8<12>();
        }

        inline auto sse_mask8_13() noexcept
        {
            return sse_mask8<13>();
        }

        inline auto sse_mask8_14() noexcept
        {
            return sse_mask8<14>();
        }

        inline auto sse_mask8_15() noexcept
        {
            return sse_mask8<15>();
        }

#pragma endregion

#pragma region 16-bit Masks

        template <std::size_t... Indices>
        inline auto sse_mask16() noexcept
        {
            static_assert(((Indices < 8) && ... && true));

            static constexpr auto masks = sse2_mask_array<short, Indices...>();
            return _mm_setr_epi16(masks[0], masks[1], masks[2], masks[3], masks[4], masks[5], masks[6], masks[7]);
        }

        template <std::size_t... Indices>
        inline auto sse_mask16(std::index_sequence<Indices...>) noexcept
        {
            return sse_mask16<Indices...>();
        }

        inline auto sse_mask16_0() noexcept
        {
            return sse_mask16<0>();
        }

        inline auto sse_mask16_1() noexcept
        {
            return sse_mask16<1>();
        }

        inline auto sse_mask16_2() noexcept
        {
            return sse_mask16<2>();
        }

        inline auto sse_mask16_3() noexcept
        {
            return sse_mask16<3>();
        }

        inline auto sse_mask16_4() noexcept
        {
            return sse_mask16<4>();
        }

        inline auto sse_mask16_5() noexcept
        {
            return sse_mask16<5>();
        }

        inline auto sse_mask16_6() noexcept
        {
            return sse_mask16<6>();
        }

        inline auto sse_mask16_7() noexcept
        {
            return sse_mask16<7>();
        }

#pragma endregion

#pragma region 32-bit Masks

        template <std::size_t... Indices>
        inline auto sse_mask32() noexcept
        {
            static_assert(((Indices < 4) && ... && true));

            static constexpr auto masks = sse2_mask_array<std::int32_t, Indices...>();
            return _mm_setr_epi32(masks[0], masks[1], masks[2], masks[3]);
        }

        template <std::size_t... Indices>
        inline auto sse_mask32(std::index_sequence<Indices...>) noexcept
        {
            return sse_mask32<Indices...>();
        }

        inline auto sse_mask32_0() noexcept
        {
            return sse_mask32<0>();
        }

        inline auto sse_mask32_1() noexcept
        {
            return sse_mask32<1>();
        }

        inline auto sse_mask32_2() noexcept
        {
            return sse_mask32<2>();
        }

        inline auto sse_mask32_3() noexcept
        {
            return sse_mask32<3>();
        }

#pragma endregion

#pragma region 64-bit Masks

        template <std::size_t... Indices>
        inline auto sse_mask64() noexcept
        {
            static_assert(((Indices < 2) && ... && true));

            // NOTE: _mm_setr_epi64x not available on 32-bit MSVC :(
            static constexpr auto masks = sse2_mask_array<std::int64_t, Indices...>();
            return _mm_set_epi64x(masks[1], masks[0]);
        }

        template <std::size_t... Indices>
        inline auto sse_mask64(std::index_sequence<Indices...>) noexcept
        {
            return sse_mask64<Indices...>();
        }

        inline auto sse_mask64_0() noexcept
        {
            return sse_mask64<0>();
        }

        inline auto sse_mask64_1() noexcept
        {
            return sse_mask64<1>();
        }

#pragma endregion

#pragma region 128-bit Masks

        inline auto sse_mask128_0() noexcept
        {
            return _mm_set1_epi64x(0xFFFFFFFFFFFFFFFFull);
        }

#pragma endregion

#pragma endregion
    }



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

        template <std::size_t Size = size>
        static inline vector_type splat(value_type value) noexcept
        {
            static_assert(Size <= size);

            if constexpr (Size == size)
            {
                return _mm_set1_epi8(value);
            }
            else
            {
                return _mm_setr_epi8(
                    (Size >= 1) ? value : 0,
                    (Size >= 2) ? value : 0,
                    (Size >= 3) ? value : 0,
                    (Size >= 4) ? value : 0,
                    (Size >= 5) ? value : 0,
                    (Size >= 6) ? value : 0,
                    (Size >= 7) ? value : 0,
                    (Size >= 8) ? value : 0,
                    (Size >= 9) ? value : 0,
                    (Size >= 10) ? value : 0,
                    (Size >= 11) ? value : 0,
                    (Size >= 12) ? value : 0,
                    (Size >= 13) ? value : 0,
                    (Size >= 14) ? value : 0,
                    (Size >= 15) ? value : 0,
                    (Size >= 16) ? value : 0);
            }
        }

        static inline vector_type set(
            value_type v0,  value_type v1,  value_type v2,  value_type v3,
            value_type v4,  value_type v5,  value_type v6,  value_type v7,
            value_type v8,  value_type v9,  value_type v10, value_type v11,
            value_type v12, value_type v13, value_type v14, value_type v15) noexcept
        {
            return _mm_setr_epi8(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15);
        }

        // NOTE: Probably inefficient. TODO: Make better!
        template <typename = void>
        static inline void fill_array(vector_type*) {}

        template <typename... Args>
        static inline void fill_array(
            vector_type* result,
            value_type v0,      value_type v1  = 0, value_type v2  = 0, value_type v3 = 0,
            value_type v4  = 0, value_type v5  = 0, value_type v6  = 0, value_type v7 = 0,
            value_type v8  = 0, value_type v9  = 0, value_type v10 = 0, value_type v11 = 0,
            value_type v12 = 0, value_type v13 = 0, value_type v14 = 0, value_type v15 = 0, Args... args) noexcept
        {
            *result = set(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15);
            fill_array(result + 1, args...);
        }

        template <std::size_t Size, typename... Args>
        static inline void fill(std::array<vector_type, Size>& result, Args... args) noexcept
        {
            fill_array(result.data(), args...);
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Masks

        template <std::size_t Count>
        static inline vector_type low_mask(vector_type value) noexcept
        {
            using sequence_type = std::make_index_sequence<Count>;

            const auto mask = details::sse_mask8(sequence_type{});
            return _mm_and_si128(value, mask);
        }

        template <std::size_t Count>
        static inline vector_type high_mask(vector_type value) noexcept
        {
            using base_sequence = std::make_index_sequence<Count>;
            using sequence_type = increment_integer_sequence_t<base_sequence, size - Count>;

            const auto mask = details::sse_mask8(sequence_type{});
            return _mm_and_si128(value, mask);
        }

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

        template <std::size_t Size = size>
        static inline vector_type splat(value_type value) noexcept
        {
            static_assert(Size <= size);

            if constexpr (Size == size)
            {
                return _mm_set1_epi16(value);
            }
            else
            {
                return _mm_setr_epi16(
                    (Size >= 1) ? value : 0,
                    (Size >= 2) ? value : 0,
                    (Size >= 3) ? value : 0,
                    (Size >= 4) ? value : 0,
                    (Size >= 5) ? value : 0,
                    (Size >= 6) ? value : 0,
                    (Size >= 7) ? value : 0,
                    (Size >= 8) ? value : 0);
            }
        }

        static inline vector_type set(
            value_type v0, value_type v1, value_type v2, value_type v3,
            value_type v4, value_type v5, value_type v6, value_type v7) noexcept
        {
            return _mm_setr_epi16(v0, v1, v2, v3, v4, v5, v6, v7);
        }

        // NOTE: Probably inefficient. TODO: Make better!
        template <typename = void>
        static inline void fill_array(vector_type*) {}

        template <typename... Args>
        static inline void fill_array(
            vector_type* result,
            value_type v0,     value_type v1 = 0, value_type v2 = 0, value_type v3 = 0,
            value_type v4 = 0, value_type v5 = 0, value_type v6 = 0, value_type v7 = 0, Args... args) noexcept
        {
            *result = set(v0, v1, v2, v3, v4, v5, v6, v7);
            fill_array(result + 1, args...);
        }

        template <std::size_t Size, typename... Args>
        static inline void fill(std::array<vector_type, Size>& result, Args... args) noexcept
        {
            fill_array(result.data(), args...);
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Masks

        template <std::size_t Count>
        static inline vector_type low_mask(vector_type value) noexcept
        {
            using sequence_type = std::make_index_sequence<Count>;

            const auto mask = details::sse_mask16(sequence_type{});
            return _mm_and_si128(value, mask);
        }

        template <std::size_t Count>
        static inline vector_type high_mask(vector_type value) noexcept
        {
            using base_sequence = std::make_index_sequence<Count>;
            using sequence_type = increment_integer_sequence_t<base_sequence, size - Count>;

            const auto mask = details::sse_mask16(sequence_type{});
            return _mm_and_si128(value, mask);
        }

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

        template <std::size_t Size = size>
        static inline vector_type splat(value_type value) noexcept
        {
            static_assert(Size <= size);

            if constexpr (Size == size)
            {
                return _mm_set1_epi32(value);
            }
            else
            {
                return _mm_setr_epi32(
                    (Size >= 1) ? value : 0,
                    (Size >= 2) ? value : 0,
                    (Size >= 3) ? value : 0,
                    (Size >= 4) ? value : 0);
            }
        }

        static inline vector_type set(value_type v0, value_type v1, value_type v2, value_type v3) noexcept
        {
            return _mm_setr_epi32(v0, v1, v2, v3);
        }

        // NOTE: Probably inefficient. TODO: Make better!
        template <typename = void>
        static inline void fill_array(vector_type*) {}

        template <typename... Args>
        static inline void fill_array(
            vector_type* result,
            value_type v0, value_type v1 = 0, value_type v2 = 0, value_type v3 = 0, Args... args) noexcept
        {
            *result = set(v0, v1, v2, v3);
            fill_array(result + 1, args...);
        }

        template <std::size_t Size, typename... Args>
        static inline void fill(std::array<vector_type, Size>& result, Args... args) noexcept
        {
            fill_array(result.data(), args...);
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Masks

        template <std::size_t Count>
        static inline vector_type low_mask(vector_type value) noexcept
        {
            using sequence_type = std::make_index_sequence<Count>;

            const auto mask = details::sse_mask32(sequence_type{});
            return _mm_and_si128(value, mask);
        }

        template <std::size_t Count>
        static inline vector_type high_mask(vector_type value) noexcept
        {
            using base_sequence = std::make_index_sequence<Count>;
            using sequence_type = increment_integer_sequence_t<base_sequence, size - Count>;

            const auto mask = details::sse_mask32(sequence_type{});
            return _mm_and_si128(value, mask);
        }

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

        template <std::size_t Size = size>
        static inline vector_type splat(value_type value) noexcept
        {
            static_assert(Size <= size);

            if constexpr (Size == size)
            {
                return _mm_set1_epi64x(value);
            }
            else
            {
                return _mm_set_epi64x((Size >= 2) ? value : 0, (Size >= 1) ? value : 0);
            }
        }

        static inline vector_type set(value_type v0, value_type v1) noexcept
        {
            // NOTE: _mm_setr_epi64x not available on 32-bit MSVC :(
            return _mm_set_epi64x(v1, v0);
        }

        // NOTE: Probably inefficient. TODO: Make better!
        template <typename = void>
        static inline void fill_array(vector_type*) {}

        template <typename... Args>
        static inline void fill_array(vector_type* result, value_type v0, value_type v1 = 0, Args... args) noexcept
        {
            *result = set(v0, v1);
            fill_array(result + 1, args...);
        }

        template <std::size_t Size, typename... Args>
        static inline void fill(std::array<vector_type, Size>& result, Args... args) noexcept
        {
            fill_array(result.data(), args...);
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Masks

        template <std::size_t Count>
        static inline vector_type low_mask(vector_type value) noexcept
        {
            using sequence_type = std::make_index_sequence<Count>;

            const auto mask = details::sse_mask64(sequence_type{});
            return _mm_and_si128(value, mask);
        }

        template <std::size_t Count>
        static inline vector_type high_mask(vector_type value) noexcept
        {
            using base_sequence = std::make_index_sequence<Count>;
            using sequence_type = increment_integer_sequence_t<base_sequence, size - Count>;

            const auto mask = details::sse_mask64(sequence_type{});
            return _mm_and_si128(value, mask);
        }

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

        template <std::size_t Size = size>
        static inline vector_type splat(value_type value) noexcept
        {
            static_assert(Size <= size);

            if constexpr (Size == size)
            {
                return _mm_set1_ps(value);
            }
            else
            {
                return _mm_setr_ps(
                    (Size >= 1) ? value : 0,
                    (Size >= 2) ? value : 0,
                    (Size >= 3) ? value : 0,
                    (Size >= 4) ? value : 0);
            }
        }

        static inline vector_type set(value_type v0, value_type v1, value_type v2, value_type v3) noexcept
        {
            return _mm_setr_ps(v0, v1, v2, v3);
        }

        // NOTE: Probably inefficient. TODO: Make better!
        template <typename = void>
        static inline void fill_array(vector_type*) {}

        template <typename... Args>
        static inline void fill_array(
            vector_type* result,
            value_type v0, value_type v1 = 0, value_type v2 = 0, value_type v3 = 0, Args... args) noexcept
        {
            *result = set(v0, v1, v2, v3);
            fill_array(result + 1, args...);
        }

        template <std::size_t Size, typename... Args>
        static inline void fill(std::array<vector_type, Size>& result, Args... args) noexcept
        {
            fill_array(result.data(), args...);
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Masks

        template <std::size_t Count>
        static inline vector_type low_mask(vector_type value) noexcept
        {
            using sequence_type = std::make_index_sequence<Count>;

            const auto mask = details::sse_mask32(sequence_type{});
            return _mm_and_ps(value, _mm_castsi128_ps(mask));
        }

        template <std::size_t Count>
        static inline vector_type high_mask(vector_type value) noexcept
        {
            using base_sequence = std::make_index_sequence<Count>;
            using sequence_type = increment_integer_sequence_t<base_sequence, size - Count>;

            const auto mask = details::sse_mask32(sequence_type{});
            return _mm_and_ps(value, _mm_castsi128_ps(mask));
        }

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

        template <std::size_t Size = size>
        static inline vector_type splat(value_type value) noexcept
        {
            static_assert(Size <= size);

            if constexpr (Size == size)
            {
                return _mm_set1_pd(value);
            }
            else
            {
                return _mm_setr_pd((Size >= 1) ? value : 0, (Size >= 2) ? value : 0);
            }
        }

        static inline vector_type set(value_type v0, value_type v1) noexcept
        {
            return _mm_setr_pd(v0, v1);
        }

        // NOTE: Probably inefficient. TODO: Make better!
        template <typename = void>
        static inline void fill_array(vector_type*) {}

        template <typename... Args>
        static inline void fill_array(vector_type* result, value_type v0, value_type v1 = 0, Args... args) noexcept
        {
            *result = set(v0, v1);
            fill_array(result + 1, args...);
        }

        template <std::size_t Size, typename... Args>
        static inline void fill(std::array<vector_type, Size>& result, Args... args) noexcept
        {
            fill_array(result.data(), args...);
        }

#pragma endregion

#pragma region Component Access



#pragma endregion

#pragma region Masks

        template <std::size_t Count>
        static inline vector_type low_mask(vector_type value) noexcept
        {
            using sequence_type = std::make_index_sequence<Count>;

            const auto mask = details::sse_mask64(sequence_type{});
            return _mm_and_pd(value, _mm_castsi128_pd(mask));
        }

        template <std::size_t Count>
        static inline vector_type high_mask(vector_type value) noexcept
        {
            using base_sequence = std::make_index_sequence<Count>;
            using sequence_type = increment_integer_sequence_t<base_sequence, size - Count>;

            const auto mask = details::sse_mask64(sequence_type{});
            return _mm_and_pd(value, _mm_castsi128_pd(mask));
        }

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

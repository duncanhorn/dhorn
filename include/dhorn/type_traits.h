/*
 * Duncan Horn
 *
 * type_traits.h
 *
 * Definitions that should be in type_traits, but aren't
 */
#pragma once

#include <cstdint>
#include <type_traits>

namespace dhorn
{
    /*
     * is_comparable
     */
#pragma region is_comparable

    namespace garbage
    {
        template <typename Lhs, typename Rhs>
        struct IsComparable
        {
            template <typename Left, typename Right>
            static auto Fn(int) ->
                decltype((std::declval<Left>() == std::declval<Right>()), std::true_type())
            {
            }

            // The compiler will prefer int over float for integral constants
            template <typename Left, typename Right>
            static auto Fn(float) ->
                std::false_type
            {
            }

            using type = decltype(Fn<Lhs, Rhs>(0));
        };
    }

    template <typename Lhs, typename Rhs>
    struct is_comparable :
        public garbage::IsComparable<Lhs, Rhs>::type
    {
    };

    template <typename Lhs, typename Rhs>
    constexpr bool is_comparable_v = is_comparable<Lhs, Rhs>::value;

#pragma endregion



    /*
     * is_less_than_comparable
     */
#pragma region is_less_than_comparable

    namespace garbage
    {
        template <typename Lhs, typename Rhs>
        struct IsLessThanComparable
        {
            template <typename Left, typename Right>
            static auto Fn(int) ->
                decltype((std::declval<Left>() < std::declval<Right>()), std::true_type())
            {
            }

            // The compiler will prefer int over float for integral constants
            template <typename Left, typename Right>
            static auto Fn(float) ->
                std::false_type
            {
            }

            using type = decltype(Fn<Lhs, Rhs>(0));
        };
    }

    template <typename Lhs, typename Rhs>
    struct is_less_than_comparable :
        public garbage::IsLessThanComparable<Lhs, Rhs>::type
    {
    };

    template <typename Lhs, typename Rhs>
    constexpr bool is_less_than_comparable_v = is_less_than_comparable<Lhs, Rhs>::value;

#pragma endregion



    /*
     * array_traits
     */
#pragma region array_traits

    template <typename Ty>
    struct array_traits
    {
        static const bool is_array = false;
    };

    template <typename Ty>
    struct array_traits<Ty[]>
    {
        static const bool is_array = true;
        using value_type = Ty;
    };

    template <typename Ty, size_t Size>
    struct array_traits<Ty[Size]>
    {
        static const bool is_array = true;
        using value_type = Ty;
        static const size_t size = Size;
    };

#pragma endregion



    /*
     * array_size
     */
    template <typename Ty, size_t size>
    inline constexpr size_t array_size(const Ty(&)[size])
    {
        return size;
    }



    /*
     * byte_offset
     */
    template <typename Ty, typename Struct>
    inline constexpr size_t byte_offset(Ty Struct::*member)
    {
        return
            reinterpret_cast<uint8_t *>(&(reinterpret_cast<Struct *>(nullptr)->*member)) -
            reinterpret_cast<uint8_t *>(nullptr);
    }



    /*
     * select_integer
     */
#pragma region select_integer

    template <size_t Bytes>
    struct select_integer_t;

    template <size_t Bytes>
    struct select_unsigned_t;

    template <typename Ty>
    using select_integer = typename select_integer_t<sizeof(Ty)>::type;

    template <typename Ty>
    using select_unsigned = typename select_unsigned_t<sizeof(Ty)>::type;



    /*
     * int8_t
     */
    template <>
    struct select_integer_t<1>
    {
        using type = int8_t;
    };

    template <>
    struct select_unsigned_t<1>
    {
        using type = uint8_t;
    };



    /*
     * int16_t
     */
    template <>
    struct select_integer_t<2>
    {
        using type = int16_t;
    };

    template <>
    struct select_unsigned_t<2>
    {
        using type = uint16_t;
    };



    /*
     * int32_t
     */
    template <>
    struct select_integer_t<4>
    {
        using type = int32_t;
    };

    template <>
    struct select_unsigned_t<4>
    {
        using type = uint32_t;
    };



    /*
     * int64_t
     */
    template <>
    struct select_integer_t<8>
    {
        using type = int64_t;
    };

    template <>
    struct select_unsigned_t<8>
    {
        using type = uint64_t;
    };

#pragma endregion



    /*
     * is_c_string
     */
#pragma region is_c_string

    template <typename Ty>
    struct is_c_string :
        public std::conjunction<
            std::is_pointer<Ty>,
            std::disjunction<
                std::is_same<std::decay_t<std::remove_pointer_t<Ty>>, char>,
                std::is_same<std::decay_t<std::remove_pointer_t<Ty>>, wchar_t>,
                std::is_same<std::decay_t<std::remove_pointer_t<Ty>>, char16_t>,
                std::is_same<std::decay_t<std::remove_pointer_t<Ty>>, char32_t>
            >
        >
    {
    };

    template <typename Ty>
    constexpr bool is_c_string_v = is_c_string<Ty>::value;

#pragma endregion
}

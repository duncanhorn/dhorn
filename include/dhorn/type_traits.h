/*
 * type_traits.h
 *
 * Duncan Horn
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
    inline constexpr size_t array_size(_In_ const Ty(&)[size])
    {
        return size;
    }



    /*
     * byte_offset
     */
    template <typename Ty, typename Struct>
    inline constexpr size_t byte_offset(_In_ Ty Struct::*member)
    {
        return
            reinterpret_cast<uint8_t *>(&(reinterpret_cast<Struct *>(nullptr)->*member)) -
            reinterpret_cast<uint8_t *>(nullptr);
    }
}

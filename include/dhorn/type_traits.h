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
     *
     * Type trait for determining if operator== is well formed for the two specified types
     */
#pragma region is_comparable

    namespace details
    {
        template <typename Lhs, typename Rhs>
        struct IsComparable
        {
            template <typename Left, typename Right>
            static auto Fn(int) ->
                decltype((std::declval<Left>() == std::declval<Right>()), std::true_type{})
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
        public details::IsComparable<Lhs, Rhs>::type
    {
    };

    template <typename Lhs, typename Rhs>
    constexpr bool is_comparable_v = is_comparable<Lhs, Rhs>::value;

#pragma endregion



    /*
     * is_less_than_comparable
     *
     * Type trait for determining if operator< is well formed for the two specified types
     */
#pragma region is_less_than_comparable

    namespace details
    {
        template <typename Lhs, typename Rhs>
        struct IsLessThanComparable
        {
            template <typename Left, typename Right>
            static auto Fn(int) ->
                decltype((std::declval<Left>() < std::declval<Right>()), std::true_type{})
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
        public details::IsLessThanComparable<Lhs, Rhs>::type
    {
    };

    template <typename Lhs, typename Rhs>
    constexpr bool is_less_than_comparable_v = is_less_than_comparable<Lhs, Rhs>::value;

#pragma endregion



    /*
     * any_base_of
     *
     * An extension of `std::is_base_of` that applies to a variadic set of types where `value` is true if there exists
     * any type `T` in the template parameter pack such that `std::is_base_of<Base, T>::value` is true.
     */
#pragma region any_base_of

    template <typename Base, typename... Derived>
    struct any_base_of;

    template <typename Base, typename Ty>
    struct any_base_of<Base, Ty> :
        public std::is_base_of<Base, Ty>
    {
    };

    template <typename Base, typename Ty, typename... Derived>
    struct any_base_of<Base, Ty, Derived...> :
        public std::disjunction<std::is_base_of<Base, Ty>, any_base_of<Base, Derived...>>
    {
    };

    template <typename Base, typename... Derived>
    constexpr bool any_base_of_v = any_base_of<Base, Derived...>::value;

#pragma endregion



    /*
     * all_base_of
     *
     * An extension of `std::is_base_of` that applies to a variadic set of types where `value` is true if, for every
     * type `T` in the template parameter pack, `std::is_base_of<Base, T>::value` is true.
     */
#pragma region all_base_of

    template <typename Base, typename... Derived>
    struct all_base_of;

    template <typename Base, typename Ty>
    struct all_base_of<Base, Ty> :
        public std::is_base_of<Base, Ty>
    {
    };

    template <typename Base, typename Ty, typename... Derived>
    struct all_base_of<Base, Ty, Derived...> :
        public std::conjunction<std::is_base_of<Base, Ty>, all_base_of<Base, Derived...>>
    {
    };

    template <typename Base, typename... Derived>
    constexpr bool all_base_of_v = all_base_of<Base, Derived...>::value;

#pragma endregion



    /*
     * byte_offset
     *
     * An easy way to get the byte offset of a member within a struct. This works similar to the macro offsetof, but
     * with different syntax:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * struct my_struct
     * {
     *     uint32_t member1;
     *     uint32_t member2;
     * };
     *
     * std::cout << byte_offset(&mystruct::member1) << std::endl;
     * std::cout << byte_offset(&mystruct::member2) << std::endl;
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * Possible output:
     * 0
     * 4
     */
    template <typename Ty, typename Struct>
    inline constexpr size_t byte_offset(Ty Struct::*member)
    {
        return
            reinterpret_cast<uint8_t *>(&(reinterpret_cast<Struct *>(nullptr)->*member)) -
            reinterpret_cast<uint8_t *>(nullptr);
    }



    /*
     * is_c_string
     *
     * Type trait for determining if a type is a "C-style" string. That is, if it is a pointer to a char type (char,
     * wchar_t, char16_t, or char32_t). Note that this does *not* include arrays (e.g. char[]). Arrays need to be
     * decayed for them to give a value of 'true' for 'is_c_string'.
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



    /*
     * is_implicitly_default_constructible
     *
     * Type trait for determining if a type is implicitly default constructor. That is, its default constructor:
     * (1) exists, and (2) is not marked as explicit.
     */
#pragma region is_implicitly_default_constructible

    template <typename Ty>
    struct is_implicitly_default_constructible
    {
    private:

        template <typename T = Ty>
        static auto invoke(int) -> decltype(std::declval<T&>() = {}, std::true_type{});

        template <typename T = Ty>
        static auto invoke(float)->std::false_type;

    public:

        static const bool value = decltype(invoke(0))::value;
    };

    template <typename Ty>
    constexpr bool is_implicitly_default_constructible_v = is_implicitly_default_constructible<Ty>::value;

#pragma endregion
}

/*
 * Duncan Horn
 *
 * type_traits.h
 *
 * Definitions that should be in type_traits, but aren't
 */
#pragma once

#include <cstdint>
#include <functional>
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
            template <typename Left = Lhs, typename Right = Rhs>
            static auto evaluate(int) -> decltype((std::declval<Left>() == std::declval<Right>()), std::true_type{});

            template <typename Left = Lhs, typename Right = Rhs>
            static auto evaluate(float) -> std::false_type;

            using type = decltype(evaluate(0));
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
            template <typename Left = Lhs, typename Right = Rhs>
            static auto evaluate(int) -> decltype((std::declval<Left>() < std::declval<Right>()), std::true_type{});

            template <typename Left = Lhs, typename Right = Rhs>
            static auto evaluate(float) -> std::false_type;

            using type = decltype(evaluate(0));
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
     * is_implicitly_constructible
     *
     * Type trait for determining if a type is implicitly constructible for a given set of arguments.
     */
#pragma region is_implicitly_constructible

    namespace details
    {
        template <typename Ty, typename... Args>
        struct IsImplicitlyConstructible
        {
            template <typename T>
            struct test_struct { T value; };

            template <typename T = Ty>
            static auto evaluate(int) -> decltype(test_struct<T>{ { std::declval<Args>()... } }, std::true_type{});

            template <typename = Ty>
            static auto evaluate(float)->std::false_type;

            using type = decltype(evaluate(0));
        };
    }

    template <typename Ty, typename... Args>
    struct is_implicitly_constructible :
        public details::IsImplicitlyConstructible<Ty, Args...>::type
    {
    };

    template <typename Ty, typename... Args>
    constexpr bool is_implicitly_constructible_v = is_implicitly_constructible<Ty, Args...>::value;



    template <typename Ty>
    using is_implicitly_default_constructible = is_implicitly_constructible<Ty>;

    template <typename Ty>
    constexpr bool is_implicitly_default_constructible_v = is_implicitly_default_constructible<Ty>::value;

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
     * decay_ref
     *
     * Decays a type by `std::decay_t<T>`, unless the decayed type is `std::reference_wrapper<T>`, in which case the
     * result is `T&`.
     */
#pragma region decay_ref

    namespace details
    {
        template <typename Ty>
        struct decay_ref_helper
        {
            using type = Ty;
        };

        template <typename Ty>
        struct decay_ref_helper<std::reference_wrapper<Ty>>
        {
            using type = Ty&;
        };
    }

    template <typename Ty>
    struct decay_ref
    {
        using type = typename details::decay_ref_helper<std::decay_t<Ty>>::type;
    };

    template <typename Ty>
    using decay_ref_t = typename decay_ref<Ty>::type;

#pragma endregion



    /*
     * byte_offset
     *
     * An easy way to get the byte offset of a member within a struct. This works similar to the macro offsetof, but
     * with different syntax:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * struct my_struct
     * {
     *     std::uint32_t member1;
     *     std::uint32_t member2;
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
    inline constexpr std::size_t byte_offset(Ty Struct::*member)
    {
        return
            reinterpret_cast<std::uint8_t *>(&(reinterpret_cast<Struct *>(nullptr)->*member)) -
            reinterpret_cast<std::uint8_t *>(nullptr);
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
}

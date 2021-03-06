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
     * first_t
     *
     * Always selects the first template type argument. Useful when trying to SFINAE away member functions based off the
     * type's template argument(s). E.g. the following will fail to compile:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *  template <typename Ty>
     *  struct foo
     *  {
     *      template <std::enable_if_t<std::is_same_v<Ty, int>, int> = 0>
     *      void do_only_if_int() {}
     *  };
     *  foo<float> f;
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * Even though `f.do_only_if_int` is never called, instantiation of the class template will fail since the template
     * only depends on the class template type. One way to work around this is to introduce a new type. E.g.:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *      template <typename Type = Ty, std::enable_if_t<std::is_same_v<Type, int>, int> = 0>
     *      void do_only_if_int() {}
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * This will now compile, but can be avoided via something like `f.do_only_if_int<int>()` which will successfully
     * compile. Although doing something like that would be similar to shooting oneself in the foot, we can, and
     * therefore should, do better. With `first_t`, this would then look like:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *      template <typename Type = Ty, std::enable_if_t<std::is_same_v<first_t<Ty, Type>, int>, int> = 0>
     *      void do_only_if_int() {}
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * The compiler won't outright reject this since `first_t` could be specialized. E.g. one could work around this by:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *  template <>
     *  struct first<float, float> // NOTE: Assume this is in the namespace dhorn::details
     *  {
     *      using type = int;
     *  };
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * In this case, `f.do_only_if_int()` will compile successfully, but the requirements for doing so require shooting
     * oneself int the foot, amputating it, then shooting oneself in the head.
     */
#pragma region

    namespace details
    {
        template <typename Ty, typename...>
        struct first
        {
            using type = Ty;
        };
    }

    template <typename... Types>
    using first_t = typename details::first<Types...>::type;

#pragma endregion



    /*
     * is_comparable
     *
     * Type trait for determining if operator== is well formed for the two specified types
     */
#pragma region is_comparable

    namespace details
    {
        template <typename Lhs, typename Rhs>
        struct is_comparable
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
        public details::is_comparable<Lhs, Rhs>::type
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
        struct is_less_than_comparable
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
        public details::is_less_than_comparable<Lhs, Rhs>::type
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
        struct is_implicitly_constructible
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
        public details::is_implicitly_constructible<Ty, Args...>::type
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
     * any type `T` in the template parameter pack such that `std::is_base_of_v<Base, T>` is true.
     */
#pragma region any_base_of

    template <typename Base, typename... Derived>
    struct any_base_of :
        public std::disjunction<std::is_base_of<Base, Derived>...>
    {
    };

    template <typename Base, typename... Derived>
    constexpr bool any_base_of_v = any_base_of<Base, Derived...>::value;

#pragma endregion



    /*
     * all_base_of
     *
     * An extension of `std::is_base_of` that applies to a variadic set of types where `value` is true if, for every
     * type `T` in the template parameter pack, `std::is_base_of_v<Base, T>` is true.
     */
#pragma region all_base_of

    template <typename Base, typename... Derived>
    struct all_base_of :
        public std::conjunction<std::is_base_of<Base, Derived>...>
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
        struct decay_ref
        {
            using type = Ty;
        };

        template <typename Ty>
        struct decay_ref<std::reference_wrapper<Ty>>
        {
            using type = Ty&;
        };

        template <typename Ty>
        using decay_ref_t = typename decay_ref<Ty>::type;
    }

    template <typename Ty>
    struct decay_ref
    {
        using type = details::decay_ref_t<std::decay_t<Ty>>;
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
    inline /*constexpr*/ std::size_t byte_offset(Ty Struct::*member)
    {
        return reinterpret_cast<std::uintptr_t>(&(static_cast<Struct*>(nullptr)->*member));
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

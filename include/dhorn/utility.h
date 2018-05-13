/*
 * Duncan Horn
 *
 * utility.h
 *
 * Functions/types that operate with, or follow the same patterns as functions/types in the STL `utility` header.
 */
#pragma once

#include <utility>
#include <variant>

namespace dhorn
{
    /*
     * join_integer_sequence
     *
     * Concatenates, or joins, the values of two `std::integer_sequence`-s
     */
#pragma region

    template <typename FirstIntegerSequence, typename SecondIntegerSequence>
    struct join_integer_sequence;

    template <typename Ty, Ty... FirstValues, Ty... SecondValues>
    struct join_integer_sequence<std::integer_sequence<Ty, FirstValues...>, std::integer_sequence<Ty, SecondValues...>>
    {
        using type = std::integer_sequence<Ty, FirstValues..., SecondValues...>;
    };

    template <typename FirstIntegerSequence, typename SecondIntegerSequence>
    using join_integer_sequence_t = typename join_integer_sequence<FirstIntegerSequence, SecondIntegerSequence>::type;

#pragma endregion



    /*
     * make_reverse_integer_sequence
     */
#pragma region make_reverse_integer_sequence

    namespace details
    {
        template <typename IntegerSequence>
        struct make_reverse_integer_sequence;

        template <typename Ty, Ty... Values>
        struct make_reverse_integer_sequence<std::integer_sequence<Ty, Values...>>
        {
            using type = std::integer_sequence<Ty, (sizeof...(Values) - Values - 1)...>;
        };
    }

    template <typename Ty, std::size_t N>
    using make_reverse_integer_sequence =
        typename details::make_reverse_integer_sequence<std::make_integer_sequence<Ty, N>>::type;

    template <std::size_t N>
    using make_reverse_index_sequence = make_reverse_integer_sequence<std::size_t, N>;

#pragma endregion



    /*
     * reverse_integer_sequence
     *
     * Reverses the values of a `std::integer_sequence`
     */
#pragma region reverse_integer_sequence

    template <typename IntegerSequence>
    struct reverse_integer_sequence;

    template <typename Ty>
    struct reverse_integer_sequence<std::integer_sequence<Ty>>
    {
        using type = std::integer_sequence<Ty>;
    };

    template <typename Ty, Ty First, Ty... Others>
    struct reverse_integer_sequence<std::integer_sequence<Ty, First, Others...>>
    {
        using type = join_integer_sequence_t<
            typename reverse_integer_sequence<std::integer_sequence<Ty, Others...>>::type,
            std::integer_sequence<Ty, First>>;
    };

    template <typename IntegerSequence>
    using reverse_integer_sequence_t = typename reverse_integer_sequence<IntegerSequence>::type;

#pragma endregion



    /*
     * increment_integer_sequence
     */
#pragma region 

    template <typename IntegerSequence, typename IntegerSequence::value_type Value>
    struct increment_integer_sequence;

    template <typename Ty, Ty... Values, Ty IncrementValue>
    struct increment_integer_sequence<std::integer_sequence<Ty, Values...>, IncrementValue>
    {
        using type = std::integer_sequence<Ty, (Values + IncrementValue)...>;
    };

    template <typename IntegerSequence, typename IntegerSequence::value_type Value>
    using increment_integer_sequence_t = typename increment_integer_sequence<IntegerSequence, Value>::type;

#pragma endregion



    /*
     * decrement_integer_sequence
     */
#pragma region 

    template <typename IntegerSequence, typename IntegerSequence::value_type Value>
    struct decrement_integer_sequence;

    template <typename Ty, Ty... Values, Ty DecrementValue>
    struct decrement_integer_sequence<std::integer_sequence<Ty, Values...>, DecrementValue>
    {
        using type = std::integer_sequence<Ty, (Values - DecrementValue)...>;
    };

    template <typename IntegerSequence, typename IntegerSequence::value_type Value>
    using decrement_integer_sequence_t = typename decrement_integer_sequence<IntegerSequence, Value>::type;

#pragma endregion



    /*
     * get_byte
     *
     * Returns the Nth byte in some integer type.
     */
#pragma region get_byte

    template <std::size_t Index, typename Integer>
    inline constexpr std::uint8_t get_byte(Integer value)
    {
        static_assert(Index < sizeof(Integer), "Attempting to access a byte outside the bounds of the type");
        using UnsignedInteger = std::make_unsigned_t<Integer>;

        constexpr std::size_t shift = 8 * Index;
        return static_cast<std::uint8_t>((value >> shift) & static_cast<UnsignedInteger>(0xFF));
    }

#pragma endregion



    /*
     * variant_index
     */
#pragma region variant_index

    namespace details
    {
        template <typename Ty, typename...>
        struct variant_index;

        template <typename Ty, typename... Types>
        constexpr std::size_t variant_index_v = variant_index<Ty, Types...>::value;

        template <typename Ty>
        struct variant_index<Ty>
        {
            // Type not found
        };

        template <typename Ty, typename... OtherTypes>
        struct variant_index<Ty, Ty, OtherTypes...> :
            std::integral_constant<std::size_t, 0>
        {
        };

        template <typename Ty, typename WrongType, typename... OtherTypes>
        struct variant_index<Ty, WrongType, OtherTypes...> :
            std::integral_constant<std::size_t, 1 + variant_index_v<Ty, OtherTypes...>>
        {
        };
    }

    template <typename Ty, typename Variant>
    struct variant_index;

    template <typename Ty, typename Variant>
    constexpr std::size_t variant_index_v = variant_index<Ty, Variant>::value;

    template <typename Ty, typename... Types>
    struct variant_index<Ty, std::variant<Types...>> :
        details::variant_index<Ty, Types...>
    {
    };

#pragma endregion
}

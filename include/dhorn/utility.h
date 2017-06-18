/*
 * Duncan Horn
 *
 * utility.h
 *
 * Functions/types that operate with, or follow the same patterns as functions/types in the STL `utility` header.
 */
#pragma once

#include <utility>

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
     * get_byte
     *
     * Returns the Nth byte in some integer type.
     */
#pragma region get_byte

    template <size_t Index, typename Integer>
    inline constexpr uint8_t get_byte(Integer value)
    {
        static_assert(Index < sizeof(Integer), "Attempting to access a byte outside the bounds of the type");
        using UnsignedInteger = std::make_unsigned_t<Integer>;

        constexpr size_t shift = 8 * Index;
        return static_cast<uint8_t>((value >> shift) & static_cast<UnsignedInteger>(0xFF));
    }

#pragma endregion
}

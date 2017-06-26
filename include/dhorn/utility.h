/*
 * Duncan Horn
 *
 * utility.h
 *
 * Functions/types that operate with, or follow the same patterns as functions/types in the STL `utility` header.
 */
#pragma once

#include <cstdint>
#include <utility>

namespace dhorn
{
    /*
     * concat_integer_sequence
     *
     * concats the values of two `std::integer_sequence`-s
     */
#pragma region 

    template <typename FirstIntegerSequence, typename SecondIntegerSequence>
    struct concat_integer_sequence;

    template <typename Ty, Ty... FirstValues, Ty... SecondValues>
    struct concat_integer_sequence<
        std::integer_sequence<Ty, FirstValues...>,
        std::integer_sequence<Ty, SecondValues...>>
    {
        using type = std::integer_sequence<Ty, FirstValues..., SecondValues...>;
    };

    template <typename FirstIntegerSequence, typename SecondIntegerSequence>
    using concat_integer_sequence_t =
        typename concat_integer_sequence<FirstIntegerSequence, SecondIntegerSequence>::type;

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

    template <typename Ty, size_t N>
    using make_reverse_integer_sequence =
        typename details::make_reverse_integer_sequence<std::make_integer_sequence<Ty, N>>::type;

    template <size_t N>
    using make_reverse_index_sequence = make_reverse_integer_sequence<size_t, N>;

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
        using type = concat_integer_sequence_t<
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

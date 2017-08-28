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
#pragma region std::integer_sequence Interop

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
     *
     * Creates a `std::integer_sequence` with decending values such that the last value is zero.
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
     * integer_sequence_get
     *
     * Gets the value from a `std::integer_sequence` at the specified index
     */
#pragma region integer_sequence_get

    template <typename IntegerSequence, size_t Index>
    struct integer_sequence_get;

    template <typename IntegerSequence, size_t Index>
    constexpr auto integer_sequence_get_v = integer_sequence_get<IntegerSequence, Index>::value;

    template <typename Ty, Ty... Values, size_t Index>
    struct integer_sequence_get<std::integer_sequence<Ty, Values...>, Index>
    {
        static_assert(Index < sizeof...(Values), "Index references a value outside the bounds of the integer_sequence");
    private:

        static constexpr Ty values[] = { Values... };

    public:

        static constexpr Ty value = values[Index];
    };

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

    namespace details
    {
        template <typename IntegerSequence, typename IndexSequence>
        struct reverse_integer_sequence;

        template <typename Ty, Ty... Values, size_t... Indices>
        struct reverse_integer_sequence<std::integer_sequence<Ty, Values...>, std::index_sequence<Indices...>>
        {
            using seq_type = std::integer_sequence<Ty, Values...>;
            using type = std::integer_sequence<
                Ty,
                integer_sequence_get<seq_type, (sizeof...(Values) - Indices - 1)>::value...>;
        };
    }

    template <typename Ty, Ty... Values>
    struct reverse_integer_sequence<std::integer_sequence<Ty, Values...>>
    {
        using type = typename details::reverse_integer_sequence<
            std::integer_sequence<Ty, Values...>,
            std::make_index_sequence<sizeof...(Values)>>::type;
    };

    template <typename IntegerSequence>
    using reverse_integer_sequence_t = typename reverse_integer_sequence<IntegerSequence>::type;

#pragma endregion

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
}

/*
 * Duncan Horn
 *
 * utility.h
 *
 * 
 */
#pragma once

#include <utility>

namespace dhorn
{
    /*
     * join_integer_sequence
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
}

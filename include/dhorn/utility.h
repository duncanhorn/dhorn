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

    template <typename FirstIntegerSequence, typename SecondIntegerSequence>
    using join_integer_sequence_t = typename join_integer_sequence<FirstIntegerSequence, SecondIntegerSequence>::type;

    template <typename Ty, Ty... FirstValues, Ty... SecondValues>
    struct join_integer_sequence<std::integer_sequence<Ty, FirstValues...>, std::integer_sequence<Ty, SecondValues...>>
    {
        using type = std::integer_sequence<Ty, FirstValues..., SecondValues...>;
    };

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

        template <typename IntegerSequence>
        using make_reverse_integer_sequence_t = typename make_reverse_integer_sequence<IntegerSequence>::type;
    }

    template <typename Ty, std::size_t N>
    using make_reverse_integer_sequence =
        details::make_reverse_integer_sequence_t<std::make_integer_sequence<Ty, N>>;

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

    template <typename IntegerSequence>
    using reverse_integer_sequence_t = typename reverse_integer_sequence<IntegerSequence>::type;

    template <typename Ty>
    struct reverse_integer_sequence<std::integer_sequence<Ty>>
    {
        using type = std::integer_sequence<Ty>;
    };

    template <typename Ty, Ty First, Ty... Others>
    struct reverse_integer_sequence<std::integer_sequence<Ty, First, Others...>>
    {
        using type = join_integer_sequence_t<
            reverse_integer_sequence_t<std::integer_sequence<Ty, Others...>>,
            std::integer_sequence<Ty, First>>;
    };

#pragma endregion



    /*
     * increment_integer_sequence
     */
#pragma region 

    template <typename IntegerSequence, typename IntegerSequence::value_type Value>
    struct increment_integer_sequence;

    template <typename IntegerSequence, typename IntegerSequence::value_type Value>
    using increment_integer_sequence_t = typename increment_integer_sequence<IntegerSequence, Value>::type;

    template <typename Ty, Ty... Values, Ty IncrementValue>
    struct increment_integer_sequence<std::integer_sequence<Ty, Values...>, IncrementValue>
    {
        using type = std::integer_sequence<Ty, (Values + IncrementValue)...>;
    };

#pragma endregion



    /*
     * decrement_integer_sequence
     */
#pragma region 

    template <typename IntegerSequence, typename IntegerSequence::value_type Value>
    struct decrement_integer_sequence;

    template <typename IntegerSequence, typename IntegerSequence::value_type Value>
    using decrement_integer_sequence_t = typename decrement_integer_sequence<IntegerSequence, Value>::type;

    template <typename Ty, Ty... Values, Ty DecrementValue>
    struct decrement_integer_sequence<std::integer_sequence<Ty, Values...>, DecrementValue>
    {
        using type = std::integer_sequence<Ty, (Values - DecrementValue)...>;
    };

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



    /*
     * any_of
     *
     * A helper type for comparing a single value with multiple values, returning true if the comparison is true for any
     * one of the values.
     */
#pragma region any_of

    namespace details
    {
        template <typename... Types>
        class any_of_t
        {
            using seq = std::make_index_sequence<sizeof...(Types)>;

        public:
            constexpr any_of_t(Types&&... values) :
                _values(std::forward<Types>(values)...)
            {
            }

            template <typename Ty>
            friend constexpr bool operator==(const Ty& lhs, const any_of_t& rhs)
            {
                return compare(lhs, rhs, std::equal_to<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator==(const any_of_t& lhs, const Ty& rhs)
            {
                return compare(lhs, rhs, std::equal_to<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator!=(const Ty& lhs, const any_of_t& rhs)
            {
                return compare(lhs, rhs, std::not_equal_to<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator!=(const any_of_t& lhs, const Ty& rhs)
            {
                return compare(lhs, rhs, std::not_equal_to<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator<(const Ty& lhs, const any_of_t& rhs)
            {
                return compare(lhs, rhs, std::less<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator<(const any_of_t& lhs, const Ty& rhs)
            {
                return compare(lhs, rhs, std::less<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator>(const Ty& lhs, const any_of_t& rhs)
            {
                return compare(lhs, rhs, std::greater<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator>(const any_of_t& lhs, const Ty& rhs)
            {
                return compare(lhs, rhs, std::greater<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator<=(const Ty& lhs, const any_of_t& rhs)
            {
                return compare(lhs, rhs, std::less_equal<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator<=(const any_of_t& lhs, const Ty& rhs)
            {
                return compare(lhs, rhs, std::less_equal<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator>=(const Ty& lhs, const any_of_t& rhs)
            {
                return compare(lhs, rhs, std::greater_equal<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator>=(const any_of_t& lhs, const Ty& rhs)
            {
                return compare(lhs, rhs, std::greater_equal<>{}, seq{});
            }

        private:

            template <typename Ty, typename Compare, std::size_t... Indices>
            static constexpr bool compare(
                [[maybe_unused]] const Ty& lhs,
                [[maybe_unused]] const any_of_t& rhs,
                [[maybe_unused]] const Compare& compareOp,
                std::index_sequence<Indices...>)
            {
                return (compareOp(lhs, std::get<Indices>(rhs._values)) || ...);
            }

            template <typename Ty, typename Compare, std::size_t... Indices>
            static constexpr bool compare(
                [[maybe_unused]] const any_of_t& lhs,
                [[maybe_unused]] const Ty& rhs,
                [[maybe_unused]] const Compare& compareOp,
                std::index_sequence<Indices...>)
            {
                return (compareOp(std::get<Indices>(lhs._values), rhs) || ...);
            }

            std::tuple<Types...> _values;
        };
    }

    template <typename... Types>
    inline constexpr details::any_of_t<Types...> any_of(Types&&... values)
    {
        return details::any_of_t(std::forward<Types>(values)...);
    }

#pragma endregion



    /*
     * all_of
     */
#pragma region all_of

    namespace details
    {
        template <typename... Types>
        class all_of_t
        {
            using seq = std::make_index_sequence<sizeof...(Types)>;

        public:
            constexpr all_of_t(Types&&... values) :
                _values(std::forward<Types>(values)...)
            {
            }

            template <typename Ty>
            friend constexpr bool operator==(const Ty& lhs, const all_of_t& rhs)
            {
                return compare(lhs, rhs, std::equal_to<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator==(const all_of_t& lhs, const Ty& rhs)
            {
                return compare(lhs, rhs, std::equal_to<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator!=(const Ty& lhs, const all_of_t& rhs)
            {
                return compare(lhs, rhs, std::not_equal_to<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator!=(const all_of_t& lhs, const Ty& rhs)
            {
                return compare(lhs, rhs, std::not_equal_to<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator<(const Ty& lhs, const all_of_t& rhs)
            {
                return compare(lhs, rhs, std::less<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator<(const all_of_t& lhs, const Ty& rhs)
            {
                return compare(lhs, rhs, std::less<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator>(const Ty& lhs, const all_of_t& rhs)
            {
                return compare(lhs, rhs, std::greater<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator>(const all_of_t& lhs, const Ty& rhs)
            {
                return compare(lhs, rhs, std::greater<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator<=(const Ty& lhs, const all_of_t& rhs)
            {
                return compare(lhs, rhs, std::less_equal<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator<=(const all_of_t& lhs, const Ty& rhs)
            {
                return compare(lhs, rhs, std::less_equal<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator>=(const Ty& lhs, const all_of_t& rhs)
            {
                return compare(lhs, rhs, std::greater_equal<>{}, seq{});
            }

            template <typename Ty>
            friend constexpr bool operator>=(const all_of_t& lhs, const Ty& rhs)
            {
                return compare(lhs, rhs, std::greater_equal<>{}, seq{});
            }

        private:

            template <typename Ty, typename Compare, std::size_t... Indices>
            static constexpr bool compare(
                [[maybe_unused]] const Ty& lhs,
                [[maybe_unused]] const all_of_t& rhs,
                [[maybe_unused]] const Compare& compareOp,
                std::index_sequence<Indices...>)
            {
                return (compareOp(lhs, std::get<Indices>(rhs._values)) && ...);
            }

            template <typename Ty, typename Compare, std::size_t... Indices>
            static constexpr bool compare(
                [[maybe_unused]] const all_of_t& lhs,
                [[maybe_unused]] const Ty& rhs,
                [[maybe_unused]] const Compare& compareOp,
                std::index_sequence<Indices...>)
            {
                return (compareOp(std::get<Indices>(lhs._values), rhs) && ...);
            }

            std::tuple<Types...> _values;
        };
    }

    template <typename... Types>
    inline constexpr details::all_of_t<Types...> all_of(Types&&... values)
    {
        return details::all_of_t(std::forward<Types>(values)...);
    }

#pragma endregion



    /*
     * USer-Defined Literals
     */
    inline namespace literals
    {
        inline namespace integer_literals
        {
            inline constexpr std::size_t operator""_sz(unsigned long long int value) noexcept
            {
                return static_cast<std::size_t>(value);
            }
        }
    }
}

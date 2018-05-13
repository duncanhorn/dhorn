/*
 * Duncan Horn
 *
 * compressed_pair.h
 *
 * It is a rule that in C++ all types have a size of at least one (so that all objects can have a distinct address).
 * This can be annoying when you need to store empty (or likely to be empty) types within a struct, but you don't want
 * to pay that extra cost (especially if you never take the address of that object). This is particularly true of types
 * that include an allocator/deleter/traits object alongside another object/pointer. E.g. you wouldn't want
 * `std::unique_ptr` to be larger than (and likely twice as large as!) a pointer.
 *
 * This is where `compressed_pair` comes in handy. It solves this problem by deriving from types - as opposed to keeping
 * them as members (and therefore taking up space) - that are both empty and non-final. All other types that don't fit
 * these restrictions are kept as members. This allows pointers/objects to live alongside a traits type without the
 * traits type taking up space (so long as the traits type is empty and non-final).
 */
#pragma once

#include "compressed_base.h"
#include "type_traits.h"

namespace dhorn
{
#pragma region compressed_pair

    /*
     * compressed_pair
     */
#pragma region compressed_pair

    namespace details
    {
        /*
         * tag_base
         *
         * In C++, it is not possible to directly derive from the same type more than once (for good reasons; you would
         * otherwise be unable to un-ambiguously differentiate them). The alternate is `tag_base` where you change the
         * direct type you are deriving from by "tagging" it with an integer.
         */
        template <std::size_t tag, typename Ty>
        struct tag_base :
            public Ty
        {
            using Ty::Ty;
        };
    }



    template <typename First, typename Second>
    class compressed_pair :
        public details::tag_base<0, compressed_base<First>>,
        public details::tag_base<1, compressed_base<Second>>
    {
        // Bases
        using first_base = details::tag_base<0, compressed_base<First>>;
        using second_base = details::tag_base<1, compressed_base<Second>>;

        // Friends
        template <typename, typename>
        friend class compressed_pair;



    public:
        /*
         * Public Types
         */
        using first_type = First;
        using second_type = Second;



        /*
         * Constructor(s)/Destructor
         */
#pragma region Constructor(s)/Destructor

        // Default Construction
        template <
            typename FirstTy = First,
            typename SecondTy = Second,
            std::enable_if_t<std::conjunction_v<
                std::is_default_constructible<FirstTy>,
                std::is_default_constructible<SecondTy>
            >, int> = 0,
            std::enable_if_t<std::conjunction_v<
                is_implicitly_default_constructible<FirstTy>,
                is_implicitly_default_constructible<SecondTy>
            >, int> = 0>
        constexpr compressed_pair()
            noexcept(std::conjunction_v<
                std::is_nothrow_default_constructible<First>,
                std::is_nothrow_default_constructible<Second>
            >)
        {
        }

        template <
            typename FirstTy = First,
            typename SecondTy = Second,
            std::enable_if_t<std::conjunction_v<
                std::is_default_constructible<FirstTy>,
                std::is_default_constructible<SecondTy>
            >, int> = 0,
            std::enable_if_t<std::negation_v<std::conjunction<
                is_implicitly_default_constructible<FirstTy>,
                is_implicitly_default_constructible<SecondTy>
            >>, int> = 0>
        explicit constexpr compressed_pair()
            noexcept(std::conjunction_v<
                std::is_nothrow_default_constructible<First>,
                std::is_nothrow_default_constructible<Second>
            >)
        {
        }



        // Value Copy Construction
        template <
            typename FirstTy = First,
            typename SecondTy = Second,
            std::enable_if_t<std::conjunction_v<
                std::is_copy_constructible<FirstTy>,
                std::is_copy_constructible<SecondTy>
            >, int> = 0,
            std::enable_if_t<std::conjunction_v<
                std::is_convertible<const FirstTy&, First>,
                std::is_convertible<const SecondTy&, Second>
            >, int> = 0>
        constexpr compressed_pair(const first_type& first, const second_type& second)
            noexcept(std::conjunction_v<
                std::is_nothrow_copy_constructible<First>,
                std::is_nothrow_copy_constructible<Second>
            >) :
            first_base(first),
            second_base(second)
        {
        }

        template <
            typename FirstTy = First,
            typename SecondTy = Second,
            std::enable_if_t<std::conjunction_v<
                std::is_copy_constructible<FirstTy>,
                std::is_copy_constructible<SecondTy>
            >, int> = 0,
            std::enable_if_t<std::negation_v<std::conjunction<
                std::is_convertible<const FirstTy&, First>,
                std::is_convertible<const SecondTy&, Second>
            >>, int> = 0>
        explicit constexpr compressed_pair(const first_type& first, const second_type& second)
            noexcept(std::conjunction_v<
                std::is_nothrow_copy_constructible<First>,
                std::is_nothrow_copy_constructible<Second>
            >) :
            first_base(first),
            second_base(second)
        {
        }



        // Value Move Construction
        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction_v<
                std::is_constructible<First, FirstTy&&>,
                std::is_constructible<Second, SecondTy&&>
            >, int> = 0,
            std::enable_if_t<std::conjunction_v<
                std::is_convertible<FirstTy&&, First>,
                std::is_convertible<SecondTy&&, Second>
            >, int> = 0>
        constexpr compressed_pair(FirstTy&& first, SecondTy&& second)
            noexcept(std::conjunction_v<
                std::is_nothrow_constructible<First, FirstTy&&>,
                std::is_nothrow_constructible<Second, SecondTy&&>
            >) :
            first_base(std::forward<FirstTy>(first)),
            second_base(std::forward<SecondTy>(second))
        {
        }

        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction_v<
                std::is_constructible<First, FirstTy&&>,
                std::is_constructible<Second, SecondTy&&>
            >, int> = 0,
            std::enable_if_t<std::negation_v<std::conjunction<
                std::is_convertible<FirstTy&&, First>,
                std::is_convertible<SecondTy&&, Second>
            >>, int> = 0>
        explicit constexpr compressed_pair(FirstTy&& first, SecondTy&& second)
            noexcept(std::conjunction_v<
                std::is_nothrow_constructible<First, FirstTy&&>,
                std::is_nothrow_constructible<Second, SecondTy&&>
            >) :
            first_base(std::forward<FirstTy>(first)),
            second_base(std::forward<SecondTy>(second))
        {
        }



        // Copy Conversion Construction
        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction_v<
                std::is_constructible<First, const FirstTy&>,
                std::is_constructible<Second, const SecondTy&>
            >, int> = 0,
            std::enable_if_t<std::conjunction_v<
                std::is_convertible<const FirstTy&, First>,
                std::is_convertible<const SecondTy&, Second>
            >, int> = 0>
        constexpr compressed_pair(const compressed_pair<FirstTy, SecondTy>& other)
            noexcept(std::conjunction_v<
                std::is_nothrow_constructible<First, const FirstTy&>,
                std::is_nothrow_constructible<Second, const SecondTy&>
            >) :
            first_base(other.first()),
            second_base(other.second())
        {
        }

        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction_v<
                std::is_constructible<First, const FirstTy&>,
                std::is_constructible<Second, const SecondTy&>
            >, int> = 0,
            std::enable_if_t<std::negation_v<std::conjunction<
                std::is_convertible<const FirstTy&, First>,
                std::is_convertible<const SecondTy&, Second>
            >>, int> = 0>
        explicit constexpr compressed_pair(const compressed_pair<FirstTy, SecondTy>& other)
            noexcept(std::conjunction_v<
                std::is_nothrow_constructible<First, const FirstTy&>,
                std::is_nothrow_constructible<Second, const SecondTy&>
            >) :
            first_base(other.first()),
            second_base(other.second())
        {
        }



        // Move Conversion Construction
        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction_v<
                std::is_constructible<First, FirstTy&&>,
                std::is_constructible<Second, SecondTy&&>
            >, int> = 0,
            std::enable_if_t<std::conjunction_v<
                std::is_convertible<FirstTy&&, First>,
                std::is_convertible<SecondTy&&, Second>
            >, int> = 0>
        constexpr compressed_pair(compressed_pair<FirstTy, SecondTy>&& other)
            noexcept(std::conjunction_v<
                std::is_nothrow_constructible<First, FirstTy&&>,
                std::is_nothrow_constructible<Second, SecondTy&&>
            >) :
            first_base(std::forward<FirstTy>(other.first())),
            second_base(std::forward<SecondTy>(other.second()))
        {
        }

        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction_v<
                std::is_constructible<First, FirstTy&&>,
                std::is_constructible<Second, SecondTy&&>
            >, int> = 0,
            std::enable_if_t<std::negation_v<std::conjunction<
                std::is_convertible<FirstTy&&, First>,
                std::is_convertible<SecondTy&&, Second>
            >>, int> = 0>
        explicit constexpr compressed_pair(compressed_pair<FirstTy, SecondTy>&& other)
            noexcept(std::conjunction_v<
                std::is_nothrow_constructible<First, FirstTy&&>,
                std::is_nothrow_constructible<Second, SecondTy&&>
            >) :
            first_base(std::forward<FirstTy>(other.first())),
            second_base(std::forward<SecondTy>(other.second()))
        {
        }



        // Piecewise Construction
        template <typename... FirstTypes, typename... SecondTypes>
        compressed_pair(
            std::piecewise_construct_t,
            std::tuple<FirstTypes...> firstArgs,
            std::tuple<SecondTypes...> secondArgs)
            noexcept(std::conjunction_v<
                std::is_nothrow_constructible<First, FirstTypes&&...>,
                std::is_nothrow_constructible<Second, SecondTypes&&...>
            >) :
            first_base(std::move(firstArgs)),
            second_base(std::move(secondArgs))
        {
        }



        // Copy Construction
        compressed_pair(const compressed_pair&) = default;

        // Move Construction
        compressed_pair(compressed_pair&&) = default;

#pragma endregion



        /*
         * Assignment Operators
         */
#pragma region Assignment Operators

        compressed_pair& operator=(const compressed_pair& other) = default;

        compressed_pair& operator=(compressed_pair&& other) = default;

        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction_v<
                std::is_assignable<First&, const FirstTy&>,
                std::is_assignable<Second&, const SecondTy&>
            >, int> = 0>
        compressed_pair& operator=(const compressed_pair<FirstTy, SecondTy>& other)
        {
            static_cast<first_base&>(*this) = other.first();
            static_cast<second_base&>(*this) = other.second();
            return *this;
        }

        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction_v<
                std::is_assignable<First&, FirstTy&&>,
                std::is_assignable<Second&, SecondTy&&>
            >, int> = 0>
        compressed_pair& operator=(compressed_pair<FirstTy, SecondTy>&& other)
        {
            static_cast<first_base&>(*this) = std::forward<FirstTy>(other.first());
            static_cast<second_base&>(*this) = std::forward<SecondTy>(other.second());
            return *this;
        }

#pragma endregion



        /*
         * Accessors
         */
#pragma region Accessors

        // First
        constexpr first_type& first() noexcept
        {
            return first_base::value();
        }

        constexpr const first_type& first() const noexcept
        {
            return first_base::value();
        }

        constexpr volatile first_type& first() volatile noexcept
        {
            return first_base::value();
        }

        constexpr const volatile first_type& first() const volatile noexcept
        {
            return first_base::value();
        }

        // Second
        constexpr second_type& second() noexcept
        {
            return second_base::value();
        }

        constexpr const second_type& second() const noexcept
        {
            return second_base::value();
        }

        constexpr volatile second_type& second() volatile noexcept
        {
            return second_base::value();
        }

        constexpr const volatile second_type& second() const volatile noexcept
        {
            return second_base::value();
        }

#pragma endregion



        /*
         * Modifiers
         */
#pragma region Modifiers

        template <
            typename FirstTy = First,
            typename SecondTy = Second,
            std::enable_if_t<std::conjunction_v<
                std::is_swappable<FirstTy>,
                std::is_swappable<SecondTy>
            >, int> = 0>
        void swap(compressed_pair& other)
            noexcept(std::is_nothrow_swappable_v<First> && std::is_nothrow_swappable_v<Second>)
        {
            first_base::swap(static_cast<first_base&>(other));
            second_base::swap(static_cast<second_base&>(other));
        }

#pragma endregion
    };

#pragma endregion



    /*
     * make_compressed_pair
     */
#pragma region make_compressed_pair

    template <typename First, typename Second>
    inline constexpr compressed_pair<decay_ref_t<First>, decay_ref_t<Second>> make_compressed_pair(
        First&& first,
        Second&& second)
    {
        using return_type = compressed_pair<decay_ref_t<First>, decay_ref_t<Second>>;
        return return_type(std::forward<First>(first), std::forward<Second>(second));
    }

#pragma endregion



    /*
     * Comparison Operators
     */
#pragma region Comparison Operators

    template <typename First, typename Second>
    inline constexpr bool operator==(
        const compressed_pair<First, Second>& lhs,
        const compressed_pair<First, Second>& rhs)
    {
        return (lhs.first() == rhs.first()) && (lhs.second() == rhs.second());
    }

    template <typename First, typename Second>
    inline constexpr bool operator!=(
        const compressed_pair<First, Second>& lhs,
        const compressed_pair<First, Second>& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename First, typename Second>
    inline constexpr bool operator<(
        const compressed_pair<First, Second>& lhs,
        const compressed_pair<First, Second>& rhs)
    {
        return (lhs.first() < rhs.first()) || (!(rhs.first() < lhs.first()) && (lhs.second() < rhs.second()));
    }

    template <typename First, typename Second>
    inline constexpr bool operator>(
        const compressed_pair<First, Second>& lhs,
        const compressed_pair<First, Second>& rhs)
    {
        return rhs < lhs;
    }

    template <typename First, typename Second>
    inline constexpr bool operator<=(
        const compressed_pair<First, Second>& lhs,
        const compressed_pair<First, Second>& rhs)
    {
        // Negation of operator>
        return !(rhs < lhs);
    }

    template <typename First, typename Second>
    inline constexpr bool operator>=(
        const compressed_pair<First, Second>& lhs,
        const compressed_pair<First, Second>& rhs)
    {
        // Negation of operator<
        return !(lhs < rhs);
    }

#pragma endregion



    /*
     * swap
     */
    template <
        typename First,
        typename Second,
        std::enable_if_t<std::conjunction_v<
            std::is_swappable<First>,
            std::is_swappable<Second>
        >, int> = 0>
    inline void swap(compressed_pair<First, Second>& lhs, compressed_pair<First, Second>& rhs)
        noexcept(noexcept(lhs.swap(rhs)))
    {
        lhs.swap(rhs);
    }



    /*
     * Helpers
     */
    namespace details
    {
        template <typename PairTy>
        inline constexpr decltype(auto) compressed_pair_get(PairTy& pair, std::integral_constant<std::size_t, 0>) noexcept
        {
            return pair.first();
        }

        template <typename PairTy>
        inline constexpr decltype(auto) compressed_pair_get(PairTy& pair, std::integral_constant<std::size_t, 1>) noexcept
        {
            return pair.second();
        }
    }
}



#ifndef DHORN_NO_STD

namespace std
{
    /*
     * tuple_size
     */
    template <typename First, typename Second>
    struct tuple_size<dhorn::compressed_pair<First, Second>> :
        public std::integral_constant<std::size_t, 2>
    {
    };



    /*
     * tuple_element
     */
#pragma region tuple_element

    template <typename First, typename Second>
    struct tuple_element<0, dhorn::compressed_pair<First, Second>>
    {
        using type = First;
    };

    template <typename First, typename Second>
    struct tuple_element<1, dhorn::compressed_pair<First, Second>>
    {
        using type = Second;
    };

#pragma endregion



    /*
     * get
     */
#pragma region get

    // By Index
    template <std::size_t Index, typename First, typename Second>
    inline constexpr tuple_element_t<Index, dhorn::compressed_pair<First, Second>>& get(
        dhorn::compressed_pair<First, Second>& pair) noexcept
    {
        return dhorn::details::compressed_pair_get(pair, integral_constant<std::size_t, Index>{});
    }

    template <std::size_t Index, typename First, typename Second>
    inline constexpr const tuple_element_t<Index, dhorn::compressed_pair<First, Second>>& get(
        const dhorn::compressed_pair<First, Second>& pair) noexcept
    {
        return dhorn::details::compressed_pair_get(pair, integral_constant<std::size_t, Index>{});
    }

    template <std::size_t Index, typename First, typename Second>
    inline constexpr tuple_element_t<Index, dhorn::compressed_pair<First, Second>>&& get(
        dhorn::compressed_pair<First, Second>&& pair) noexcept
    {
        return std::forward<tuple_element_t<Index, dhorn::compressed_pair<First, Second>>&&>(get<Index>(pair));
    }

    template <std::size_t Index, typename First, typename Second>
    inline constexpr const tuple_element_t<Index, dhorn::compressed_pair<First, Second>>&& get(
        const dhorn::compressed_pair<First, Second>&& pair) noexcept
    {
        return std::forward<const tuple_element_t<Index, dhorn::compressed_pair<First, Second>>&&>(get<Index>(pair));
    }



    // By Type
    template <typename First, typename Second>
    inline constexpr First& get(dhorn::compressed_pair<First, Second>& pair) noexcept
    {
        return pair.first();
    }

    template <typename First, typename Second>
    inline constexpr const First& get(const dhorn::compressed_pair<First, Second>& pair) noexcept
    {
        return pair.first();
    }

    template <typename First, typename Second>
    inline constexpr First&& get(dhorn::compressed_pair<First, Second>&& pair) noexcept
    {
        return pair.first();
    }

    template <typename First, typename Second>
    inline constexpr const First&& get(const dhorn::compressed_pair<First, Second>&& pair) noexcept
    {
        return pair.first();
    }

    template <typename Second, typename First>
    inline constexpr Second& get(dhorn::compressed_pair<First, Second>& pair) noexcept
    {
        return pair.second();
    }

    template <typename Second, typename First>
    inline constexpr const Second& get(const dhorn::compressed_pair<First, Second>& pair) noexcept
    {
        return pair.second();
    }

    template <typename Second, typename First>
    inline constexpr Second&& get(dhorn::compressed_pair<First, Second>&& pair) noexcept
    {
        return pair.second();
    }

    template <typename Second, typename First>
    inline constexpr const Second&& get(const dhorn::compressed_pair<First, Second>&& pair) noexcept
    {
        return pair.second();
    }

#pragma endregion
}

#endif

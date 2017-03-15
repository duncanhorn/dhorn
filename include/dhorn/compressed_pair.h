/*
 * Duncan Horn
 *
 * compressed_pair.h
 *
 * 
 */
#pragma once

#include <tuple>
#include <type_traits>

#include "type_traits.h"

namespace dhorn
{
    /*
     * compressed_pair
     *
     * Generic implementation: Both are either final or non-empty; act like a normal pair
     */
    template <
        typename First,
        typename Second,
        bool CanDeriveFirst = !std::is_final_v<First> && std::is_empty_v<First>,
        bool CanDeriveSecond = !std::is_final_v<Second> && std::is_empty_v<Second>>
    class compressed_pair
    {
        // Friends
        template <typename, typename, bool, bool>
        friend class compressed_pair;



        // Tuple Construction Helper
        template <typename... FirstTypes, typename... SecondTypes, size_t... FirstIndices, size_t... SecondIndices>
        compressed_pair(
            std::tuple<FirstTypes...>& firstTuple,
            std::tuple<SecondTypes...>& secondTuple,
            std::index_sequence<FirstIndices...>,
            std::index_sequence<SecondIndices...>) : // TODO: noexcept??
            _first(std::get<FirstIndices>(firstTuple)...),
            _second(std::get<SecondIndices>(secondTuple)...)
        {
        }



    public:
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
        constexpr compressed_pair(const First& first, const Second& second)
            noexcept(std::conjunction_v<
                std::is_nothrow_copy_constructible<First>,
                std::is_nothrow_copy_constructible<Second>
            >) :
            _first(first),
            _second(second)
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
        explicit constexpr compressed_pair(const First& first, const Second& second)
            noexcept(std::conjunction_v<
                std::is_nothrow_copy_constructible<First>,
                std::is_nothrow_copy_constructible<Second>
            >) :
            _first(first),
            _second(second)
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
            _first(std::forward<FirstTy>(first)),
            _second(std::forward<SecondTy>(second))
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
            _first(std::forward<FirstTy>(first)),
            _second(std::forward<SecondTy>(second))
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
            _first(other.first()),
            _second(other.second())
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
            _first(other.first()),
            _second(other.second())
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
            _first(std::forward<FirstTy>(other.first())),
            _second(std::forward<SecondTy>(other.second()))
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
            _first(std::forward<FirstTy>(other.first())),
            _second(std::forward<SecondTy>(other.second()))
        {
        }



        // Piecewise Construction
        template <typename... FirstTypes, typename... SecondTypes>
        constexpr compressed_pair(
            std::piecewise_construct_t,
            std::tuple<FirstTypes...> firstArgs,
            std::tuple<SecondTypes...> secondArgs) :
            compressed_pair(
                firstArgs,
                secondArgs,
                std::make_index_sequence<sizeof...(FirstTypes)>{},
                std::make_index_sequence<sizeof...(SecondTypes)>{})
        {
        }



        // Copy Construction
        compressed_pair(const compressed_pair&) = default;

        // Move Construction
        compressed_pair(compressed_pair&&) = default;

#pragma endregion



        /*
         * Accessors
         */
#pragma region Accessors

        // First
        First& first() noexcept
        {
            return this->_first;
        }

        const First& first() const noexcept
        {
            return this->_first;
        }

        volatile First& first() volatile noexcept
        {
            return this->_first;
        }

        const volatile First& first() const volatile noexcept
        {
            return this->_first;
        }

        // Second
        Second& second() noexcept
        {
            return this->_second;
        }

        const Second& second() const noexcept
        {
            return this->_second;
        }

        volatile Second& second() volatile noexcept
        {
            return this->_second;
        }

        const volatile Second& second() const volatile noexcept
        {
            return this->_second;
        }

#pragma endregion



    private:

        First _first;
        Second _second;
    };



    /*
     * compressed_pair
     *
     * First type is empty and non-final, so we derive from it
     */
    template <typename First, typename Second, bool CanDeriveSecond>
    class compressed_pair<First, Second, true, CanDeriveSecond> :
        public First
    {
    public:

        // TODO



        /*
         * Accessors
         */
#pragma region Accessors

        // First
        First& first() noexcept
        {
            return *this;
        }

        const First& first() const noexcept
        {
            return *this;
        }

        volatile First& first() volatile noexcept
        {
            return *this;
        }

        const volatile First& first() const volatile noexcept
        {
            return *this;
        }

        // Second
        Second& second() noexcept
        {
            return this->_second;
        }

        const Second& second() const noexcept
        {
            return this->_second;
        }

        volatile Second& second() volatile noexcept
        {
            return this->_second;
        }

        const volatile Second& second() const volatile noexcept
        {
            return this->_second;
        }

#pragma endregion



    private:

        Second _second;
    };



    /*
     * compressed_pair
     *
     * First type either non-empty or final, but the second type both, so derive from it instead
     */
    template <typename First, typename Second>
    class compressed_pair<First, Second, false, true> :
        public Second
    {
    public:

        // TODO



        /*
         * Accessors
         */
#pragma region Accessors

        // First
        First& first() noexcept
        {
            return this->_first;
        }

        const First& first() const noexcept
        {
            return this->_first;
        }

        volatile First& first() volatile noexcept
        {
            return this->_first;
        }

        const volatile First& first() const volatile noexcept
        {
            return this->_first;
        }

        // Second
        Second& second() noexcept
        {
            return *this;
        }

        const Second& second() const noexcept
        {
            return *this;
        }

        volatile Second& second() volatile noexcept
        {
            return *this;
        }

        const volatile Second& second() const volatile noexcept
        {
            return *this;
        }

#pragma endregion



    private:

        First _first;
    };

}

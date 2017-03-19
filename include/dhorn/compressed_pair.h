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
#pragma region compressed_pair

    /*
     * compressed_pair
     *
     * 
     */
#pragma region compressed_pair

    namespace details
    {
        /*
         * compressed_base
         *
         * Conditionally derives from the type if (1) the type is not final, and (2) the type is empty. This allows size
         * optimizations if we are to derive from `compressed_base` more than once. Note that although there's not
         * really anything wrong from deriving from a non-final, non-empty type, this does help us avoid deriving from
         * types such as `int` which would fail to compile.
         */
#pragma region compressed_base

        // CanDerive == false overload. Keep the value as a member as opposed to deriving from it
        template <typename Ty, bool CanDerive = !std::is_final<Ty>::value && std::is_empty<Ty>::value>
        class compressed_base
        {
            // Friends
            template <typename, bool>
            friend class compressed_base;

            // Tuple Construction Helper
            template <typename... Types, size_t... Indices>
            compressed_base(std::tuple<Types...>& args, std::index_sequence<Indices...>) : // TODO: noexcept?
                _value(std::get<Indices>(args)...)
            {
            }



        public:
            /*
             * Constructor(s)/Destructor
             */
#pragma region Constructor(s)/Destructor

            // Default Construction
            template <typename Type = Ty, std::enable_if_t<std::is_default_constructible<Type>::value, int> = 0>
            constexpr compressed_base() noexcept(std::is_nothrow_default_constructible<Ty>::value)
            {
            }

            // Value Copy Construction
            template <typename Type = Ty, std::enable_if_t<std::is_copy_constructible<Type>::value, int> = 0>
            constexpr compressed_base(const Ty& value) noexcept(std::is_nothrow_copy_constructible<Ty>::value) :
                _value(value)
            {
            }

            // Value Move Construction
            template <typename Type, std::enable_if_t<std::is_constructible<Ty, Type&&>::value, int> = 0>
            constexpr compressed_base(Type&& value) noexcept(std::is_nothrow_constructible<Ty, Type&&>::value) :
                _value(std::forward<Type>(value))
            {
            }

            // Copy Conversion Construction
            template <typename Type, std::enable_if_t<std::is_constructible<Ty, const Type&>::value, int> = 0>
            constexpr compressed_base(const compressed_base<Type>& other)
                noexcept(std::is_nothrow_constructible<Ty, const Type&>::value) :
                _value(other.value())
            {
            }

            // Move Conversion Construction
            template <typename Type, std::enable_if_t<std::is_constructible<Ty, Type&&>::value, int> = 0>
            constexpr compressed_base(compressed_base<Type>&& other)
                noexcept(std::is_nothrow_constructible<Ty, Type&&>::value) :
                _value(std::forward<Type>(other.value()))
            {
            }

            // Tuple Construction
            template <typename... Types>
            constexpr compressed_base(std::tuple<Types...> args) : // noexcet?
                compressed_base(args, std::make_index_sequence<sizeof...(Types)>{})
            {
            }

            // Copy Construction
            compressed_base(const compressed_base&) = default;

            // Move Construction
            compressed_base(compressed_base&&) = default;

#pragma endregion



            /*
             * Assignment Operators
             */
#pragma region Assignment Operators

            compressed_base& operator=(const compressed_base&) = default;
            compressed_base& operator=(compressed_base&&) = default;

            template <typename Type, std::enable_if_t<std::is_assignable<Ty&, const Type&>::value, int> = 0>
            compressed_base& operator=(const compressed_base<Type>& other)
            {
                this->_value = other.value();
                return *this;
            }

            template <typename Type, std::enable_if_t<std::is_assignable<Ty&, Type&&>::value, int> = 0>
            compressed_base& operator=(compressed_base<Type>&& other)
            {
                this->_value = std::forward<Type>(other.value());
                return *this;
            }

#pragma endregion



            /*
             * Accessors
             */
#pragma region Accessors

            Ty& value() noexcept
            {
                return this->_value;
            }

            const Ty& value() const noexcept
            {
                return this->_value;
            }

            volatile Ty& value() volatile noexcept
            {
                return this->_value;
            }

            const volatile Ty& value() const volatile noexcept
            {
                return this->_value;
            }

#pragma endregion



            /*
             * Modifiers
             */
#pragma region Modifiers

            template <typename Type = Ty, std::enable_if_t<std::is_swappable<Type>::value, int> = 0>
            void swap(compressed_base& other) noexcept(std::is_nothrow_swappable<Ty>::value)
            {
                if (this != std::addressof(other))
                {
                    std::swap(this->_value, other._value);
                }
            }

#pragma endregion



        private:

            Ty _value;
        };



        // CanDerive == true overload. Use the type as a base type
        template <typename Ty>
        class compressed_base<Ty, true> : public Ty
        {
            // Friends
            template <typename, bool>
            friend class compressed_base;

            // Tuple Construction Helper
            template <typename... Types, size_t... Indices>
            compressed_base(std::tuple<Types...>& args, std::index_sequence<Indices...>) : // TODO: noexcept?
                Ty(std::get<Indices>(args)...)
            {
                // Visual Studio bug? Warning C4100: unreferenced formal parameter
                args;
            }



        public:
            /*
             * Constructor(s)/Destructor
             */
#pragma region Constructor(s)/Destructor

            // Default Construction
            template <typename Type = Ty, std::enable_if_t<std::is_default_constructible<Type>::value, int> = 0>
            constexpr compressed_base() noexcept(std::is_nothrow_default_constructible<Ty>::value)
            {
            }

            // Value Copy Construction
            template <typename Type = Ty, std::enable_if_t<std::is_copy_constructible<Type>::value, int> = 0>
            constexpr compressed_base(const Ty& value) noexcept(std::is_nothrow_copy_constructible<Ty>::value) :
                Ty(value)
            {
            }

            // Value Move Construction
            template <typename Type, std::enable_if_t<std::is_constructible<Ty, Type&&>::value, int> = 0>
            constexpr compressed_base(Type&& value) noexcept(std::is_nothrow_constructible<Ty, Type&&>::value) :
                Ty(std::forward<Type>(value))
            {
            }

            // Copy Conversion Construction
            template <typename Type, std::enable_if_t<std::is_constructible<Ty, const Type&>::value, int> = 0>
            constexpr compressed_base(const compressed_base<Type>& other)
                noexcept(std::is_nothrow_constructible<Ty, const Type&>::value) :
                Ty(other.value())
            {
            }

            // Move Conversion Construction
            template <typename Type, std::enable_if_t<std::is_constructible<Ty, Type&&>::value, int> = 0>
            constexpr compressed_base(compressed_base<Type>&& other)
                noexcept(std::is_nothrow_constructible<Ty, Type&&>::value) :
                Ty(std::forward<Type>(other.value()))
            {
            }

            // Tuple Construction
            template <typename... Types>
            constexpr compressed_base(std::tuple<Types...> args) : // noexcet?
                compressed_base(args, std::make_index_sequence<sizeof...(Types)>{})
            {
            }

            // Copy Construction
            compressed_base(const compressed_base&) = default;

            // Move Construction
            compressed_base(compressed_base&&) = default;

#pragma endregion



            /*
             * Assignment Operators
             */
#pragma region Assignment Operators

            compressed_base& operator=(const compressed_base&) = default;
            compressed_base& operator=(compressed_base&&) = default;

            template <typename Type, std::enable_if_t<std::is_assignable<Ty&, const Type&>::value, int> = 0>
            compressed_base& operator=(const compressed_base<Type>& other)
            {
                static_cast<Ty&>(*this) = other.value();
                return *this;
            }

            template <typename Type, std::enable_if_t<std::is_assignable<Ty&, Type&&>::value, int> = 0>
            compressed_base& operator=(compressed_base<Type>&& other)
            {
                static_cast<Ty&>(*this) = std::forward<Type>(other.value());
                return *this;
            }

#pragma endregion



            /*
             * Accessors
             */
#pragma region Accessors

            Ty& value() noexcept
            {
                return *this;
            }

            const Ty& value() const noexcept
            {
                return *this;
            }

            volatile Ty& value() volatile noexcept
            {
                return *this;
            }

            const volatile Ty& value() const volatile noexcept
            {
                return *this;
            }

#pragma endregion



            /*
             * Modifiers
             */
#pragma region Modifiers

            template <typename Type = Ty, std::enable_if_t<std::is_swappable<Type>::value, int> = 0>
            void swap(compressed_base&) noexcept(std::is_nothrow_swappable<Ty>::value)
            {
                // No state, so no need to swap anything
                // TODO: Is this definitely okay?
            }

#pragma endregion
        };

#pragma endregion



        /*
         *
         */
        template <size_t tag, typename Ty>
        struct tag_base :
            public Ty
        {
            using Ty::Ty;
        };
    }

    template <typename First, typename Second>
    class compressed_pair :
        public details::tag_base<0, details::compressed_base<First>>,
        public details::tag_base<1, details::compressed_base<Second>>
    {
        // Bases
        using first_base = details::tag_base<0, details::compressed_base<First>>;
        using second_base = details::tag_base<1, details::compressed_base<Second>>;

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
            std::enable_if_t<std::conjunction<
                std::is_default_constructible<FirstTy>,
                std::is_default_constructible<SecondTy>
            >::value, int> = 0,
            std::enable_if_t<std::conjunction<
                is_implicitly_default_constructible<FirstTy>,
                is_implicitly_default_constructible<SecondTy>
            >::value, int> = 0>
        constexpr compressed_pair()
            noexcept(std::conjunction<
                std::is_nothrow_default_constructible<First>,
                std::is_nothrow_default_constructible<Second>
            >::value)
        {
        }

        template <
            typename FirstTy = First,
            typename SecondTy = Second,
            std::enable_if_t<std::conjunction<
                std::is_default_constructible<FirstTy>,
                std::is_default_constructible<SecondTy>
            >::value, int> = 0,
            std::enable_if_t<std::negation<std::conjunction<
                is_implicitly_default_constructible<FirstTy>,
                is_implicitly_default_constructible<SecondTy>
            >>::value, int> = 0>
        explicit constexpr compressed_pair()
            noexcept(std::conjunction<
                std::is_nothrow_default_constructible<First>,
                std::is_nothrow_default_constructible<Second>
            >::value)
        {
        }



        // Value Copy Construction
        template <
            typename FirstTy = First,
            typename SecondTy = Second,
            std::enable_if_t<std::conjunction<
                std::is_copy_constructible<FirstTy>,
                std::is_copy_constructible<SecondTy>
            >::value, int> = 0,
            std::enable_if_t<std::conjunction<
                std::is_convertible<const FirstTy&, First>,
                std::is_convertible<const SecondTy&, Second>
            >::value, int> = 0>
        constexpr compressed_pair(const first_type& first, const second_type& second)
            noexcept(std::conjunction<
                std::is_nothrow_copy_constructible<First>,
                std::is_nothrow_copy_constructible<Second>
            >::value) :
            first_base(first),
            second_base(second)
        {
        }

        template <
            typename FirstTy = First,
            typename SecondTy = Second,
            std::enable_if_t<std::conjunction<
                std::is_copy_constructible<FirstTy>,
                std::is_copy_constructible<SecondTy>
            >::value, int> = 0,
            std::enable_if_t<std::negation<std::conjunction<
                std::is_convertible<const FirstTy&, First>,
                std::is_convertible<const SecondTy&, Second>
            >>::value, int> = 0>
        explicit constexpr compressed_pair(const first_type& first, const second_type& second)
            noexcept(std::conjunction<
                std::is_nothrow_copy_constructible<First>,
                std::is_nothrow_copy_constructible<Second>
            >::value) :
            first_base(first),
            second_base(second)
        {
        }



        // Value Move Construction
        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction<
                std::is_constructible<First, FirstTy&&>,
                std::is_constructible<Second, SecondTy&&>
            >::value, int> = 0,
            std::enable_if_t<std::conjunction<
                std::is_convertible<FirstTy&&, First>,
                std::is_convertible<SecondTy&&, Second>
            >::value, int> = 0>
        constexpr compressed_pair(FirstTy&& first, SecondTy&& second)
            noexcept(std::conjunction<
                std::is_nothrow_constructible<First, FirstTy&&>,
                std::is_nothrow_constructible<Second, SecondTy&&>
            >::value) :
            first_base(std::forward<FirstTy>(first)),
            second_base(std::forward<SecondTy>(second))
        {
        }

        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction<
                std::is_constructible<First, FirstTy&&>,
                std::is_constructible<Second, SecondTy&&>
            >::value, int> = 0,
            std::enable_if_t<std::negation<std::conjunction<
                std::is_convertible<FirstTy&&, First>,
                std::is_convertible<SecondTy&&, Second>
            >>::value, int> = 0>
        explicit constexpr compressed_pair(FirstTy&& first, SecondTy&& second)
            noexcept(std::conjunction<
                std::is_nothrow_constructible<First, FirstTy&&>,
                std::is_nothrow_constructible<Second, SecondTy&&>
            >::value) :
            first_base(std::forward<FirstTy>(first)),
            second_base(std::forward<SecondTy>(second))
        {
        }



        // Copy Conversion Construction
        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction<
                std::is_constructible<First, const FirstTy&>,
                std::is_constructible<Second, const SecondTy&>
            >::value, int> = 0,
            std::enable_if_t<std::conjunction<
                std::is_convertible<const FirstTy&, First>,
                std::is_convertible<const SecondTy&, Second>
            >::value, int> = 0>
        constexpr compressed_pair(const compressed_pair<FirstTy, SecondTy>& other)
            noexcept(std::conjunction<
                std::is_nothrow_constructible<First, const FirstTy&>,
                std::is_nothrow_constructible<Second, const SecondTy&>
            >::value) :
            first_base(other.first()),
            second_base(other.second())
        {
        }

        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction<
                std::is_constructible<First, const FirstTy&>,
                std::is_constructible<Second, const SecondTy&>
            >::value, int> = 0,
            std::enable_if_t<std::negation<std::conjunction<
                std::is_convertible<const FirstTy&, First>,
                std::is_convertible<const SecondTy&, Second>
            >>::value, int> = 0>
        explicit constexpr compressed_pair(const compressed_pair<FirstTy, SecondTy>& other)
            noexcept(std::conjunction<
                std::is_nothrow_constructible<First, const FirstTy&>,
                std::is_nothrow_constructible<Second, const SecondTy&>
            >::value) :
            first_base(other.first()),
            second_base(other.second())
        {
        }



        // Move Conversion Construction
        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction<
                std::is_constructible<First, FirstTy&&>,
                std::is_constructible<Second, SecondTy&&>
            >::value, int> = 0,
            std::enable_if_t<std::conjunction<
                std::is_convertible<FirstTy&&, First>,
                std::is_convertible<SecondTy&&, Second>
            >::value, int> = 0>
        constexpr compressed_pair(compressed_pair<FirstTy, SecondTy>&& other)
            noexcept(std::conjunction<
                std::is_nothrow_constructible<First, FirstTy&&>,
                std::is_nothrow_constructible<Second, SecondTy&&>
            >::value) :
            first_base(std::forward<FirstTy>(other.first())),
            second_base(std::forward<SecondTy>(other.second()))
        {
        }

        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction<
                std::is_constructible<First, FirstTy&&>,
                std::is_constructible<Second, SecondTy&&>
            >::value, int> = 0,
            std::enable_if_t<std::negation<std::conjunction<
                std::is_convertible<FirstTy&&, First>,
                std::is_convertible<SecondTy&&, Second>
            >>::value, int> = 0>
        explicit constexpr compressed_pair(compressed_pair<FirstTy, SecondTy>&& other)
            noexcept(std::conjunction<
                std::is_nothrow_constructible<First, FirstTy&&>,
                std::is_nothrow_constructible<Second, SecondTy&&>
            >::value) :
            first_base(std::forward<FirstTy>(other.first())),
            second_base(std::forward<SecondTy>(other.second()))
        {
        }



        // Piecewise Construction
        template <typename... FirstTypes, typename... SecondTypes>
        constexpr compressed_pair(
            std::piecewise_construct_t,
            std::tuple<FirstTypes...> firstArgs,
            std::tuple<SecondTypes...> secondArgs) :
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
            std::enable_if_t<std::conjunction<
                std::is_assignable<First&, const FirstTy&>,
                std::is_assignable<Second&, const SecondTy&>
            >::value, int> = 0>
        compressed_pair& operator=(const compressed_pair<FirstTy, SecondTy>& other)
        {
            static_cast<first_base&>(*this) = other.first();
            static_cast<second_base&>(*this) = other.second();
            return *this;
        }

        template <
            typename FirstTy,
            typename SecondTy,
            std::enable_if_t<std::conjunction<
                std::is_assignable<First&, FirstTy&&>,
                std::is_assignable<Second&, SecondTy&&>
            >::value, int> = 0>
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
        first_type& first() noexcept
        {
            return first_base::value();
        }

        const first_type& first() const noexcept
        {
            return first_base::value();
        }

        volatile first_type& first() volatile noexcept
        {
            return first_base::value();
        }

        const volatile first_type& first() const volatile noexcept
        {
            return first_base::value();
        }

        // Second
        second_type& second() noexcept
        {
            return second_base::value();
        }

        const second_type& second() const noexcept
        {
            return second_base::value();
        }

        volatile second_type& second() volatile noexcept
        {
            return second_base::value();
        }

        const volatile second_type& second() const volatile noexcept
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
            std::enable_if_t<std::conjunction<
                std::is_swappable<FirstTy>,
                std::is_swappable<SecondTy>
            >::value, int> = 0>
        void swap(compressed_pair& other)
            noexcept(std::is_nothrow_swappable<First>::value && std::is_nothrow_swappable<Second>::value)
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
}



#ifndef DHORN_NO_STD

namespace std
{
    template <
        typename First,
        typename Second,
        std::enable_if_t<std::conjunction<
            std::is_swappable<First>,
            std::is_swappable<Second>
        >::value, int> = 0>
    inline void swap(dhorn::compressed_pair<First, Second>& lhs, dhorn::compressed_pair<First, Second>& rhs)
        noexcept(noexcept(lhs.swap(rhs)))
    {
        lhs.swap(rhs);
    }
}

#endif

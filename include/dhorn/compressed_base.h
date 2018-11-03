/*
 * Duncan Horn
 *
 * compressed_base.h
 *
 * `compressed_base` is a type that conditionally derives from the template argument type if (1) the type is not final,
 * and (2) the type is empty. This allows size optimizations if the consumer is wanting to store a (potentially) empty
 * type as a member without taking the mandatory memory hit that comes with it (since all data members must be
 * individually and indentically addressible). For example, the following type:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  template <typename Ty, typename Deleter = std::default_delete<Ty>>
 *  class unique_ptr :
 *      public compressed_base<Deleter>
 *  {
 *      // ...
 *
 *  private:
 *      Ty* _ptr;
 *  };
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Would have the size of a pointer (i.e. `sizeof(Ty*)`) if `Deleter` is an empty type.
 */
#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace dhorn
{
    template <typename Ty, bool CanDerive = !std::is_final_v<Ty> && std::is_empty_v<Ty>>
    class compressed_base;

    // CanDerive == false specialization. Keep the value as a member as opposed to deriving from it
    template <typename Ty>
    class compressed_base<Ty, false>
    {
        // Friends
        template <typename, bool>
        friend class compressed_base;

        // Tuple Construction Helper
        template <typename... Types, std::size_t... Indices>
        constexpr compressed_base(std::tuple<Types...>& args, std::index_sequence<Indices...>)
            noexcept(std::is_nothrow_constructible_v<Ty, Types&&...>) :
            _value(std::get<Indices>(std::move(args))...)
        {
        }



    public:
        /*
         * Constructor(s)/Destructor
         */
#pragma region Constructor(s)/Destructor

        // Default Construction
        constexpr compressed_base() = default;

        // Forwarding Construction
        template <typename Type, std::enable_if_t<std::is_constructible_v<Ty, Type&&>, int> = 0>
        constexpr compressed_base(Type&& value) noexcept(std::is_nothrow_constructible_v<Ty, Type&&>) :
            _value(std::forward<Type>(value))
        {
        }

        // Copy Conversion Construction
        template <typename Type, std::enable_if_t<std::is_constructible_v<Ty, const Type&>, int> = 0>
        constexpr compressed_base(const compressed_base<Type>& other)
            noexcept(std::is_nothrow_constructible_v<Ty, const Type&>) :
            _value(other.value())
        {
        }

        // Move Conversion Construction
        template <typename Type, std::enable_if_t<std::is_constructible_v<Ty, Type&&>, int> = 0>
        constexpr compressed_base(compressed_base<Type>&& other) noexcept(std::is_nothrow_constructible_v<Ty, Type&&>) :
            _value(std::forward<Type>(other.value()))
        {
        }

        // "Emplace" Construction
        template <typename... Types, std::enable_if_t<std::is_constructible_v<Ty, Types&&...>, int> = 0>
        constexpr compressed_base(Types&&... types) noexcept(std::is_nothrow_constructible_v<Ty, Types&&...>) :
            _value(std::forward<Types>(types)...)
        {
        }

        // Tuple Construction
        template <typename... Types, std::enable_if_t<std::is_constructible_v<Ty, Types&&...>, int> = 0>
        constexpr compressed_base(std::tuple<Types...> args) noexcept(std::is_nothrow_constructible_v<Ty, Types&&...>) :
            compressed_base(args, std::make_index_sequence<sizeof...(Types)>{})
        {
        }

        // Copy Construction
        constexpr compressed_base(const compressed_base&) = default;

        // Move Construction
        constexpr compressed_base(compressed_base&&) = default;

#pragma endregion



        /*
         * Assignment Operators
         */
#pragma region Assignment Operators

        compressed_base& operator=(const compressed_base&) = default;
        compressed_base& operator=(compressed_base&&) = default;

        template <typename Type, std::enable_if_t<std::is_assignable_v<Ty&, Type&&>, int> = 0>
        compressed_base& operator=(Type&& value) noexcept(std::is_nothrow_assignable_v<Ty&, Type&&>)
        {
            this->_value = std::forward<Type>(value);
            return *this;
        }

        template <typename Type, std::enable_if_t<std::is_assignable_v<Ty&, const Type&>, int> = 0>
        compressed_base& operator=(const compressed_base<Type>& other)
            noexcept(std::is_nothrow_assignable_v<Ty&, const Type&>)
        {
            this->_value = other.value();
            return *this;
        }

        template <typename Type, std::enable_if_t<std::is_assignable_v<Ty&, Type&&>, int> = 0>
        compressed_base& operator=(compressed_base<Type>&& other)
            noexcept(std::is_nothrow_assignable_v<Ty&, Type&&>)
        {
            this->_value = std::forward<Type>(other.value());
            return *this;
        }

#pragma endregion



        /*
         * Accessors
         */
#pragma region Accessors

        constexpr Ty& value() noexcept
        {
            return this->_value;
        }

        constexpr const Ty& value() const noexcept
        {
            return this->_value;
        }

        constexpr volatile Ty& value() volatile noexcept
        {
            return this->_value;
        }

        constexpr const volatile Ty& value() const volatile noexcept
        {
            return this->_value;
        }

#pragma endregion



        /*
         * Modifiers
         */
#pragma region Modifiers

        template <typename Type = Ty, std::enable_if_t<std::is_swappable_v<Type>, int> = 0>
        void swap(compressed_base& other) noexcept(std::is_nothrow_swappable_v<Ty>)
        {
            using std::swap;
            if (this != std::addressof(other))
            {
                swap(this->_value, other._value);
            }
        }

#pragma endregion



    private:

        Ty _value;
    };



    // CanDerive == true specialization. Use the type as a base type
    template <typename Ty>
    class compressed_base<Ty, true> : public Ty
    {
        // Friends
        template <typename, bool>
        friend class compressed_base;

        // Tuple Construction Helper
        template <typename... Types, std::size_t... Indices>
        constexpr compressed_base(std::tuple<Types...>& args, std::index_sequence<Indices...>)
            noexcept(std::is_nothrow_constructible_v<Ty, Types&&...>) :
            Ty(std::get<Indices>(std::move(args))...)
        {
        }



    public:
        /*
         * Constructor(s)/Destructor
         */
#pragma region Constructor(s)/Destructor

        // Default Construction
        template <typename Type = Ty, std::enable_if_t<std::is_default_constructible_v<Type>, int> = 0>
        constexpr compressed_base() noexcept(std::is_nothrow_default_constructible_v<Ty>)
        {
        }

        // Forwarding Construction
        template <typename Type, std::enable_if_t<std::is_constructible_v<Ty, Type&&>, int> = 0>
        constexpr compressed_base(Type&& value) noexcept(std::is_nothrow_constructible_v<Ty, Type&&>) :
            Ty(std::forward<Type>(value))
        {
        }

        // Copy Conversion Construction
        template <typename Type, std::enable_if_t<std::is_constructible_v<Ty, const Type&>, int> = 0>
        constexpr compressed_base(const compressed_base<Type>& other)
            noexcept(std::is_nothrow_constructible_v<Ty, const Type&>) :
            Ty(other.value())
        {
        }

        // Move Conversion Construction
        template <typename Type, std::enable_if_t<std::is_constructible_v<Ty, Type&&>, int> = 0>
        constexpr compressed_base(compressed_base<Type>&& other)
            noexcept(std::is_nothrow_constructible_v<Ty, Type&&>) :
            Ty(std::forward<Type>(other.value()))
        {
        }

        // "Emplace" Construction
        template <typename... Types, std::enable_if_t<std::is_constructible_v<Ty, Types&&...>, int> = 0>
        constexpr compressed_base(Types&&... types) noexcept(std::is_nothrow_constructible_v<Ty, Types&&...>) :
            Ty(std::forward<Types>(types)...)
        {
        }

        // Tuple Construction
        template <typename... Types, std::enable_if_t<std::is_constructible_v<Ty, Types&&...>, int> = 0>
        constexpr compressed_base(std::tuple<Types...> args) noexcept(std::is_nothrow_constructible_v<Ty, Types&&...>) :
            compressed_base(args, std::make_index_sequence<sizeof...(Types)>{})
        {
        }

        // Copy Construction
        constexpr compressed_base(const compressed_base&) = default;

        // Move Construction
        constexpr compressed_base(compressed_base&&) = default;

#pragma endregion



        /*
         * Assignment Operators
         */
#pragma region Assignment Operators

        compressed_base& operator=(const compressed_base&) = default;
        compressed_base& operator=(compressed_base&&) = default;

        template <typename Type, std::enable_if_t<std::is_assignable_v<Ty&, Type&&>, int> = 0>
        compressed_base& operator=(Type&& value) noexcept(std::is_nothrow_assignable_v<Ty&, Type&&>)
        {
            static_cast<Ty&>(*this) = std::forward<Type>(value);
            return *this;
        }

        template <typename Type, std::enable_if_t<std::is_assignable_v<Ty&, const Type&>, int> = 0>
        compressed_base& operator=(const compressed_base<Type>& other)
            noexcept(std::is_nothrow_assignable_v<Ty&, const Type&>)
        {
            static_cast<Ty&>(*this) = other.value();
            return *this;
        }

        template <typename Type, std::enable_if_t<std::is_assignable_v<Ty&, Type&&>, int> = 0>
        compressed_base& operator=(compressed_base<Type>&& other)
            noexcept(std::is_nothrow_assignable_v<Ty&, Type&&>)
        {
            static_cast<Ty&>(*this) = std::forward<Type>(other.value());
            return *this;
        }

#pragma endregion



        /*
         * Accessors
         */
#pragma region Accessors

        constexpr Ty& value() noexcept
        {
            return *this;
        }

        constexpr const Ty& value() const noexcept
        {
            return *this;
        }

        constexpr volatile Ty& value() volatile noexcept
        {
            return *this;
        }

        constexpr const volatile Ty& value() const volatile noexcept
        {
            return *this;
        }

#pragma endregion



        /*
         * Modifiers
         */
#pragma region Modifiers

        template <typename Type = Ty, std::enable_if_t<std::is_swappable_v<Type>, int> = 0>
        void swap(compressed_base&) noexcept(std::is_nothrow_swappable_v<Ty>)
        {
            // No state, so no need to swap anything
            // TODO: Is this definitely okay?
        }

#pragma endregion
    };
}

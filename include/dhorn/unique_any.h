/*
 * Duncan Horn
 *
 * unique_any.h
 *
 * 
 */
#pragma once

#include "type_traits.h"

namespace dhorn
{
    /*
     * unique_any_traits
     */
#pragma region unique_any_traits

    template <typename Ty>
    struct unique_any_traits;

    template <typename Ty>
    struct unique_any_traits<Ty*>
    {
        using value_type = Ty*;

        static constexpr bool valid(value_type ptr) noexcept
        {
            return (ptr != nullptr);
        }

        static constexpr value_type default_value() noexcept
        {
            return nullptr;
        }

        static void destroy(value_type ptr) noexcept
        {
            delete ptr;
        }
    };

    template <typename Ty>
    struct unique_any_traits<Ty[]> :
        public unique_any_traits<Ty*>
    {
        static void destroy(Ty* ptr) noexcept
        {
            delete[] ptr;
        }
    };

#pragma endregion



    template <typename Ty, typename Traits = unique_any_traits<Ty>>
    class unique_any
    {
    public:
        /*
         * Public Types
         */
        using value_type = typename Traits::value_type;



        /*
         * Constructor(s)/Destructor
         */
#pragma region Constructor(s)/Destructor

        // Default/no-arg Construction:
        //  * Always constexpr
        //  * explicit if construction of Ty is explicit






        unique_any() noexcept :
            _value(Traits::default_value())
        {
        }

        unique_any(const unique_any&) = delete;

        unique_any(unique_any&& other) : // TODO: noexcept
            _value(std::move(other._value))
        {
            other.swap(*this);
        }

        ~unique_any()
        {
            // reset();
        }

#pragma endregion



        /*
         * Operators
         */
#pragma region Operators

        unique_any& operator=(const unique_any&) = delete;

        unique_any& operator=(unique_any&& other) noexcept(swap(other))
        {
            other.swap(*this);
            return *this;
        }

        operator bool() const noexcept
        {
            return Traits::valid(this->_value);
        }

#pragma endregion



        /*
         * 
         */
#pragma region

        void swap(unique_any& other) noexcept(
            std::is_nothrow_move_constructible_v<value_type> &&
            std::is_nothrow_swappable_v<value_type>)
        {
            using std::swap;
            swap(this->_value, other._value);
        }

#pragma endregion



    private:

        value_type _value;
    };
}

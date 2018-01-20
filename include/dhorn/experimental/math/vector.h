/*
 * Duncan Horn
 *
 * vector.h
 *
 * 
 */
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include "../../crtp_base.h"
#include "nointrin_traits.h"

#if _M_X64 || _M_IX86

#include "sse2_traits.h"

#else
#error Architecture not supported
#endif

namespace dhorn::experimental::math
{
    // TODO: Arch specific/customizable
    template <typename Ty>
    using default_vector_traits = nointrin_traits<Ty>;



    namespace details
    {
        /*
         * vector_common
         *
         * Contains the definitions of functions common to all vector types
         */
        template <typename Derived, typename Ty, std::size_t Dimensions>
        class vector_common :
            protected crtp_base<Derived>
        {
        public:



        private:


        };
    }



    /*
     * vector
     *
     * The user-facing vector type. Note the heirarchy of functionality:
     *      -   Size-shared functionality. E.g. 3 and 4 dimensional vectors both have z components, whereas 2
     *          dimensional vectors do not
     *      -   Size-specific functionality. E.g. cross product is only defined for 3 dimensional vectors
     *      -   Type-specific functionality. E.g. bitwise operations are not defined for float/double
     *      -   Shared/common functionality. E.g. dot product is defined for all vectors
     */
    template <typename Ty, std::size_t Dimensions, typename Traits = default_vector_traits<Ty>>
    class vector :
        public details::vector_common<vector<Ty, Dimensions>, Ty, Dimensions>
    {
    public:
        /*
         * Types/Constants
         */
        using value_type = Ty;
        using vector_type = typename Traits::vector_type;

        static constexpr std::size_t dimensions = Dimensions;
        static constexpr std::size_t vector_components = Traits::size;

    private:

        // Internal types/constants for book-keeping/optimization
        static constexpr std::size_t array_size = (Dimensions + vector_components - 1) / vector_components;
        static constexpr std::size_t extra_space = (vector_components * array_size) - Dimensions;
        static constexpr std::size_t final_fill = vector_components - extra_space;

        using array_index_sequence = std::make_index_sequence<array_size>;
        using full_array_index_sequence = std::make_index_sequence<array_size - 1>;

        using vector_array = std::array<vector_type, array_size>;



    public:
        /*
         * Constructor(s)/Destructors
         */
        vector() = default;

        template <
            typename... Args,
            std::enable_if_t<Dimensions == sizeof...(Args), int> = 0,
            std::enable_if_t<std::conjunction_v<std::is_convertible<Args, value_type>...>, int> = 0>
        vector(Args... args) noexcept
        {
            Traits::fill(this->_values, args...);
        }



        /*
         * Static Creation Functions
         */
        static vector zero() noexcept
        {
#if 1
            return zero(array_index_sequence{});
#else
            // TODO: Poor code generation for MSVC here... Anything we can do to improve?
            vector result;
            std::fill(begin(result._values), end(result._values), Traits::zero());

            return result;
#endif
        }

        static vector splat(value_type value) noexcept
        {
#if 1
            return splat(value, full_array_index_sequence{});
#else
            vector result;
            std::fill(begin(result._values), end(result._values), Traits::splat(value));

            if constexpr (extra_space > 0)
            {
                result._values.back() = Traits::low_mask<vector_components - extra_space>(result._values.back());
            }

            return result;
#endif
        }



        /*
         * Properties
         */
        constexpr std::size_t size() const noexcept
        {
            return Dimensions;
        }



        /*
         * Arithmetic Operations
         */
        friend vector operator-(vector value) noexcept
        {
            vector result;
            for (std::size_t i = 0; i < array_size; ++i)
            {
                result._values[i] = Traits::negate(value._values[i]);
            }

            return result;
        }

        vector& operator+=(vector other) noexcept
        {
            this->_values = add(this->_values, other._values, array_index_sequence{});
            return *this;
        }

        friend vector operator+(vector lhs, vector rhs) noexcept
        {
            vector result;
            result._values = add(lhs._values, rhs._values, array_index_sequence{});

            return result;
        }

        vector& operator-=(vector other) noexcept
        {
            this->_values = subtract(*this, other, array_index_sequence{});
            return *this;
        }

        friend vector operator-(vector lhs, vector rhs) noexcept
        {
            vector result;
            result._values = subtract(lhs, rhs, array_index_sequence{});

            return result;
        }



    private:

        template <std::size_t... Indices>
        static inline vector zero(std::index_sequence<Indices...>) noexcept
        {
            vector result;
            ((result._values[Indices] = Traits::zero()), ...);

            return result;
        }

        template <std::size_t... Indices>
        static inline vector splat(value_type value, std::index_sequence<Indices...>) noexcept
        {
            vector result;

            // All except (potentially) the last value is a full splat
            ((result._values[Indices] = Traits::splat(value)), ...);
            result._values[array_size - 1] = Traits::template splat<final_fill>(value);

            return result;
        }

        template <std::size_t... Indices>
        static inline vector_array add(vector_array lhs, vector_array rhs, std::index_sequence<Indices...>) noexcept
        {
            vector_array result;
            ((result[Indices] = Traits::add(lhs[Indices], rhs[Indices])), ...);

            return result;
        }

        template <std::size_t... Indices>
        static inline vector_array subtract(const vector& lhs, const vector& rhs, std::index_sequence<Indices...>) noexcept
        {
            vector_array result;
            ((result[Indices] = Traits::subtract(lhs._values[Indices], rhs._values[Indices])), ...);

            return result;
        }




        /* NOTE: Poor MSVC optimization, so removing for now
        template <std::size_t... Indices>
        static inline vector fill(vector_type value, std::index_sequence<Indices...>) noexcept
        {
            vector result;
            ((result._values[Indices] = value), ...);

            return result;
        }

        template <typename Func, std::size_t... Indices>
        static inline vector fill(Func&& func) noexcept
        {
            return fill(func(), array_index_sequence{});
        }

        template <typename Func, std::size_t... Indices>
        vector unary_transform(Func&& func, std::index_sequence<Indices...> = array_index_sequence{}) noexcept
        {
            vector result;
            ((result._values[Indices] = func(this->_values[Indices])), ...);

            return result;
        }

        template <typename Func, std::size_t... Indices>
        vector binary_transform(
            vector other,
            Func&& func,
            std::index_sequence<Indices...> = array_index_sequence{}) noexcept
        {
            vector result;
            ((result._values[Indices] = func(this->_values[Indices], other._values[Indices])), ...);

            return result;
        }
        */

        vector_array _values;
    };



    /*
     * Common Type Aliases
     */
    template <typename Ty>
    using vector2 = vector<Ty, 2>;

    template <typename Ty>
    using vector3 = vector<Ty, 3>;

    template <typename Ty>
    using vector4 = vector<Ty, 4>;
}

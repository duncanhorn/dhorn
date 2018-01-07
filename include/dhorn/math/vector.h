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

#include "../crtp_base.h"
#include "nointrin_traits.h"

#if _M_X64 || _M_IX86

#include "sse2_traits.h"

#else
#error Architecture not supported
#endif

namespace dhorn::math
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

        // Internal values for book-keeping/optimization
        static constexpr std::size_t array_size = (Dimensions + vector_components - 1) / vector_components;
        static constexpr std::size_t extra_space = (vector_components * array_size) - Dimensions;



    public:
        /*
         * Constructor(s)/Destructors
         */
        vector() = default;

        template <
            typename... Args,
            std::enable_if_t<Dimensions == sizeof...(Args), int> = 0,
            std::enable_if_t<std::conjunction_v<std::is_convertible<Args, Ty>...>, int> = 0>
        vector(Args... args) noexcept
        {
            Traits::fill(this->_values, args...);
        }



        /*
         * Static Creation Functions
         */
        static vector zero() noexcept
        {
            // TODO: Poor code generation for MSVC here... Anything we can do to improve?
            vector result;
            std::fill(begin(result._values), end(result._values), Traits::zero());

            return result;
        }

        static vector splat(Ty value) noexcept
        {
            vector result;
            std::fill(begin(result._values), end(result._values), Traits::splat(value));

            if constexpr (extra_space > 0)
            {
                result._values.back() = Traits::low_mask<vector_components - extra_space>(result._values.back());
            }

            return result;
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
        vector& operator+=(vector other) noexcept
        {
            for (std::size_t i = 0; i < array_size; ++i)
            {
                this->_values[i] = Traits::add(this->_values[i], other._values[i]);
            }

            return *this;
        }

        friend vector operator+(vector lhs, vector rhs) noexcept
        {
            vector result;
            for (std::size_t i = 0; i < array_size; ++i)
            {
                result._values[i] = Traits::add(lhs._values[i], rhs._values[i]);
            }

            return result;
        }

        vector& operator-=(vector other) noexcept
        {
            for (std::size_t i = 0; i < array_size; ++i)
            {
                this->_values[i] = Traits::subtract(this->_values[i], other._values[i]);
            }

            return *this;
        }

        friend vector operator-(vector lhs, vector rhs) noexcept
        {
            vector result;
            for (std::size_t i = 0; i < array_size; ++i)
            {
                result._values[i] = Traits::subtract(lhs._values[i], rhs._values[i]);
            }

            return result;
        }



    private:

        std::array<vector_type, array_size> _values;
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

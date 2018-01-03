/*
 * Duncan Horn
 *
 * vector.h
 *
 * 
 */
#pragma once

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
        using vector_type = typename Traits::vector_type;
        static constexpr std::size_t vector_components = Traits::size;
        static constexpr std::size_t array_size = (Dimensions + vector_components - 1) / vector_components;

    public:
        /*
         * Types/Constants
         */



        /*
         * Constructor(s)/Destructors
         */
        vector() = default;

        template <typename... Args, std::enable_if_t<Dimensions == sizeof...(Args), int> = 0>
        vector(Args... args) noexcept
        {
            Traits::fill(this->_values, args...);
        }



        /*
         * Static Creation Functions
         */
        static vector zero() noexcept
        {
            vector result;

            auto zero = Traits::zero();
            for (auto& v : result._values)
            {
                v = zero;
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

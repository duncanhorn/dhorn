/*
 * Duncan Horn
 *
 * vector.h
 *
 * 
 */
#pragma once

#include <cstdint>

namespace dhorn::math
{
    namespace details
    {
        /*
         * vector_common
         *
         * Contains the definitions of functions common to all vector types
         */
        template <typename Ty, std::size_t Dimensions>
        class vector_common
        {

        };
    }

    template <typename Ty, std::size_t Dimensions>
    class vector
    {
    public:



    private:


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

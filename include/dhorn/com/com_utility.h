/*
 * Duncan Horn
 *
 * com_utility.h
 *
 *
 */
#pragma once

#include <type_traits>
#include <Unknwn.h>

namespace dhorn::com
{
    /*
     * is_unknown
     */
#pragma region is_unknown

    template <typename... IFaces>
    struct is_unknown;

    template <typename IFace>
    struct is_unknown<IFace> :
        public std::is_base_of<IUnknown, IFace>
    {
    };

    template <typename IFace, typename... IFaces>
    struct is_unknown<IFace, IFaces...> :
        public std::bool_constant<is_unknown<IFace>::value || is_unknown<IFaces...>::value>
    {
    };

    template <typename... IFaces>
    constexpr bool is_unknown_v = is_unknown<IFaces...>::value;

#pragma endregion

}

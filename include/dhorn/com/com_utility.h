/*
 * Duncan Horn
 *
 * com_utility.h
 *
 * Helpers and type traits for dealing with COM types and objects.
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
        public std::disjunction<is_unknown<IFace>, is_unknown<IFaces...>>
    {
    };

    template <typename... IFaces>
    constexpr bool is_unknown_v = is_unknown<IFaces...>::value;

#pragma endregion



    /*
     * all_unknown
     */
#pragma region all_unknown

    template <typename... IFaces>
    struct all_unknown;

    template <typename IFace>
    struct all_unknown<IFace> :
        public is_unknown<IFace>
    {
    };

    template <typename IFace, typename... IFaces>
    struct all_unknown<IFace, IFaces...> :
        public std::conjunction<is_unknown<IFace>, all_unknown<IFaces...>>
    {
    };

    template <typename... IFaces>
    constexpr bool all_unknown_v = all_unknown<IFaces...>::value;

#pragma endregion
}

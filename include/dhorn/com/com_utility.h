/*
 * Duncan Horn
 *
 * com_utility.h
 *
 * Helpers and type traits for dealing with COM types and objects.
 */
#pragma once

#include <Unknwn.h>

#include "../type_traits.h"

namespace dhorn::com
{
    /*
     * any_unknown
     */
#pragma region any_unknown

    template <typename... IFaces>
    using any_unknown = any_base_of<IUnknown, IFaces...>;

    template <typename... IFaces>
    constexpr bool any_unknown_v = any_unknown<IFaces...>::value;

#pragma endregion



    /*
     * all_unknown
     */
#pragma region all_unknown

    template <typename... IFaces>
    using all_unknown = all_base_of<IUnknown, IFaces...>;

    template <typename... IFaces>
    constexpr bool all_unknown_v = all_unknown<IFaces...>::value;

#pragma endregion



    /*
     * is_unknown
     */
#pragma region is_unknown

    template <typename IFace>
    using is_unknown = std::is_base_of<IUnknown, IFace>;

    template <typename IFace>
    constexpr bool is_unknown_v = is_unknown<IFace>::value;

#pragma endregion
}

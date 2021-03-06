/*
 * Duncan Horn
 *
 * winrt_traits.h
 *
 * Helpers and type traits for dealing with WinRt types and objects.
 */
#pragma once

#include <inspectable.h>

#include "../type_traits.h"

namespace dhorn::winrt
{
    /*
     * any_inspectable
     */
#pragma region any_inspectable

    template <typename... IFaces>
    using any_inspectable = any_base_of<IInspectable, IFaces...>;

    template <typename... IFaces>
    constexpr bool any_inspectable_v = any_inspectable<IFaces...>::value;

#pragma endregion



    /*
     * all_inspectable
     */
#pragma region all_inspectable

    template <typename... IFaces>
    using all_inspectable = all_base_of<IInspectable, IFaces...>;

    template <typename... IFaces>
    constexpr bool all_inspectable_v = all_inspectable<IFaces...>::value;

#pragma endregion



    /*
     * is_inspectable
     */
#pragma region is_inspectable

    template <typename IFace>
    using is_inspectable = std::is_base_of<IInspectable, IFace>;

    template <typename IFace>
    constexpr bool is_inspectable_v = is_inspectable<IFace>::value;

#pragma endregion
}

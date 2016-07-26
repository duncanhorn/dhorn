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
#include <wrl/implements.h>

#include "com_ptr.h"

namespace dhorn
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
        public std::bool_constant<is_unknown<IFace>::value && all_unknown<IFaces...>::value>
    {
    };

    template <typename... IFaces>
    constexpr bool all_unknown_v = all_unknown<IFaces...>::value;

#pragma endregion



    /*
     * make
     */
#pragma region make

    template <typename Ty, typename... Args>
    com_ptr<Ty> make(Args &&...args)
    {
        auto result = Microsoft::WRL::Make<Ty>(std::forward<Args>(args)...);
        throw_hr_if_null(result);

        com_ptr<Ty> ptr;
        ptr.attach(result.Detach());
        return ptr;
    }

    template <typename Ty, typename... Args>
    com_ptr<Ty> make_and_initialize(Args &&...args)
    {
        Microsoft::WRL::ComPtr<Ty> result;
        throw_if_failed(Microsoft::WRL::MakeAndInitialize(&result, std::forward<Args>(args)...));

        com_ptr<Ty> ptr;
        ptr.attach(result.Detach());
        return ptr;
    }

#pragma endregion
}

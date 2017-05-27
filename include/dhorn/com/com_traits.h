/*
 * Duncan Horn
 *
 * com_traits.h
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



    /*
     * has_iid
     */
#pragma region has_iid

    namespace details
    {
        template <typename Ty, typename = void>
        struct has_iid_impl :
            public std::false_type
        {
        };

        template <typename Ty>
        struct has_iid_impl<Ty, std::void_t<decltype(__uuidof(Ty))>> :
            public std::true_type
        {
        };
    }

    template <typename Ty>
    struct has_iid :
        public details::has_iid_impl<Ty>
    {
    };

    template <typename Ty>
    constexpr bool has_iid_v = has_iid<Ty>::value;

#pragma endregion



    /*
     * interface_traits
     */
#pragma region interface_traits

    template <typename IFace>
    struct interface_traits
    {
        static_assert(is_unknown_v<IFace>, "Interfaces must derive from IUnknown to be used with interface_traits");

        /*
         * Types
         */
        using difference_type = std::ptrdiff_t;
        using value_type = IFace;
        using pointer = IFace*;
        using output_pointer = IFace**;
        using reference = IFace&;



        /* 
         * Static Functions
         */
        static constexpr const GUID& interface_id() noexcept
        {
            return __uuidof(IFace);
        }

        template <typename Ty>
        static HRESULT query_from(Ty* ptr, output_pointer result) noexcept
        {
            assert(*result == nullptr);
            return ptr->QueryInterface(interface_id(), reinterpret_cast<void**>(result));
        }
    };

#pragma endregion
}

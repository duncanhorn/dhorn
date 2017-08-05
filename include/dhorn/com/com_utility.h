/*
 * Duncan Horn
 *
 * com_utility.h
 *
 * Utility functions and types for dealing with COM objects, functions, and types.
 */
#pragma once

#include "com_traits.h"

namespace dhorn::com
{
    /*
     * variadic_query_interface
     *
     * A helper function for implementing the `QueryInterface` function. Note that this does _not_ take care of
     * disambiguation (e.g. for `IUnknown`, etc.)
     */
#pragma region variadic_query_interface

    template <typename Ty>
    inline HRESULT variadic_query_interface(Ty*, const GUID&, void** result) noexcept
    {
        *result  = nullptr;
        return E_NOINTERFACE;
    }

    template <typename IFace, typename... IFaces, typename Ty>
    inline HRESULT variadic_query_interface(Ty* ptr, const GUID& iid, void** result) noexcept
    {
        if (iid == interface_traits<IFace>::interface_id())
        {
            *result = static_cast<IFace*>(ptr);
            ptr->AddRef();
            return S_OK;
        }

        return variadic_query_interface<IFaces...>(ptr, iid, result);
    }

#pragma endregion



    /*
     * cast_to
     *
     *
     */
#pragma region cast_to



#pragma endregion
}

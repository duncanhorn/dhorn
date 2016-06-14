/*
 * Duncan Horn
 *
 * winrt_utility.h
 *
 *
 */
#pragma once

#include "com_ptr.h"
#include "com_utility.h"
#include "windows_exception.h"

namespace dhorn
{
    /*
     * get_activation_factory
     */
    template <typename IFace>
    com_ptr<IFace> get_activation_factory(_In_ HSTRING activatableClassId)
    {
        com_ptr<IFace> result;
        throw_if_failed(::RoGetActivationFactory(activatableClassId, IID_PPV_ARGS(&result)));
        return result;
    }



    /*
     * activate_instance
     */
    template <typename IFace>
    com_ptr<IFace> activate_instance(_In_ HSTRING activatableClassId)
    {
        com_ptr<IInspectable> insp;
        throw_if_failed(::RoActivateInstance(activatableClassId, &insp));
        return insp.as<IFace>();
    }
}

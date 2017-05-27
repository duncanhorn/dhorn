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
#include "../com/hresult_error.h"

namespace dhorn
{
    namespace experimental
    {
        /*
         * get_activation_factory
         */
        template <typename IFace>
        com_ptr<IFace> get_activation_factory(HSTRING activatableClassId)
        {
            com_ptr<IFace> result;
            com::check_hresult(::RoGetActivationFactory(activatableClassId, IID_PPV_ARGS(&result)));
            return result;
        }



        /*
         * activate_instance
         */
        template <typename IFace>
        com_ptr<IFace> activate_instance(HSTRING activatableClassId)
        {
            com_ptr<IInspectable> insp;
            com::check_hresult(::RoActivateInstance(activatableClassId, &insp));
            return insp.as<IFace>();
        }
    }
}

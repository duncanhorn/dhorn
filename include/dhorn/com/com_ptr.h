/*
 * Duncan Horn
 *
 * com_ptr.h
 *
 * A throwing COM pointer implementation
 */
#pragma once

#include "com_utility.h"
#include "hresult_error.h"

namespace dhorn::com
{
    template <typename IFace>
    struct com_ptr
    {
        static_assert(is_unknown_v<IFace>, "You can only use IUnknown types with com_ptr");

    public:
        /*
         * Public Types
         */



    private:

        IFace* _ptr;
    };
}

/*
 *
 */
#pragma once

#include <WeakReference.h>

#include "../com/com_ptr.h"
#include "../com/com_traits.h"

namespace dhorn::winrt
{
    /*
     * Helpers
     */
    namespace details
    {
        template <typename Ty, bool HasIid>
        class weak_ref_impl;

        template <typename Ty>
        class weak_ref_impl<Ty, true>
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            weak_ref_impl() = default;

            template <typename Type, std::enable_if_t<std::is_base_of<Ty, Type>::value, int> = 0>
            weak_ref_impl(Type* ptr)
            {
                this->_ptr = com::query<IWeakReference>(ptr);
            }



        private:

            com::com_ptr<IWeakReference> _ptr;
        };
    }



    /*
     * weak_ref
     */
}

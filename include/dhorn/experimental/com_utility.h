/*
 * Duncan Horn
 *
 * com_utility.h
 *
 * 
 */
#pragma once

#include <type_traits>
#include <wrl/implements.h>

#include "com_ptr.h"

namespace dhorn::experimental
{
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

/*
 * Duncan Horn
 *
 * wrl_utility.h
 */
#pragma once

#include <wrl/implements.h>

#include "../com/com_utility.h"

namespace dhorn::wrl
{
    namespace details
    {
        /*
         * select_runtime_class_flags
         */
        template <typename... IFaces>
        struct select_runtime_class_flags;

        template <int flags, typename... IFaces>
        struct select_runtime_class_flags<Microsoft::WRL::RuntimeClassFlags<flags>, IFaces...>
        {
            // If explicitly provided, use the RuntimeClassFlags provided by the programmer
            using type = Microsoft::WRL::RuntimeClassFlags<flags>;
        };
    }



    /*
     * implements
     *
     * WRL was created before variadic templates and unfortunately hasn't been updated to use them.
     */
#pragma region implements

    namespace details
    {
        template <typename... IFaces>
        struct implements_helper
        {
            using type = Microsoft::WRL::Implements<IFaces...>;
        };

        template <typename I0, typename I1, typename I2, typename I3, typename I4,
                  typename I5, typename I6, typename I7, typename I8, typename I9, typename... IFaces>
        struct implements_helper<I0, I1, I2, I3, I4, I5, I6, I7, I8, I9, IFaces...>
        {
            using type = Microsoft::WRL::Implements<I0, I1, I2, I3, I4, I5, I6, I7, I8,
                Microsoft::WRL::Implements<I9, IFaces...>>;
        };

        template <typename... IFaces>
        using implements_helper_t = typename implements_helper<IFaces...>::type;
    }



    template <typename... IFaces>
    using implements = details::implements_helper_t<IFaces...>;

#pragma endregion



    /*
     * runtime_class
     */
#pragma region runtime_class

    namespace details
    {
        template <typename... IFaces>
        struct runtime_class_helper
        {
            using type = Microsoft::WRL::RuntimeClass<IFaces...>;
        };

        template <typename I0, typename I1, typename I2, typename I3, typename I4,
                  typename I5, typename I6, typename I7, typename I8, typename I9, typename... IFaces>
        struct runtime_class_helper<I0, I1, I2, I3, I4, I5, I6, I7, I8, I9, IFaces...>
        {
            using type = Microsoft::WRL::RuntimeClass<I0, I1, I2, I3, I4, I5, I6, I7, I8,
                Microsoft::WRL::Implements<I9, IFaces...>>;
        };

        template <typename... IFaces>
        using runtime_class_helper_t = typename runtime_class_helper<IFaces...>::type;
    }



    template <typename... IFaces>
    using runtime_class = details::runtime_class_helper_t<IFaces...>;

#pragma endregion
}

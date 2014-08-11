/*
 * type_traits.h
 *
 * Duncan Horn
 *
 * Definitions that should be in type_traits, but aren't
 */

#include <type_traits>

namespace dhorn
{
    /*
     * is_comparable
     */
#pragma region is_comparable

    template <typename _Lhs, typename _Rhs>
    struct _Is_Comparable
    {
        template <typename _L, typename _R>
        static auto _Fn(int) ->
            decltype((std::declval<_L>() == std::declval<_R>()), std::true_type())
        {
        }

        // The compiler will prefer int over float for integral constants
        template <typename _L, typename _R>
        static auto _Fn(float) ->
            std::false_type
        {
        }

        using type = decltype(_Fn<_Lhs, _Rhs>(0));
    };

    template <typename _Lhs, typename _Rhs>
    struct is_comparable :
        public _Is_Comparable<_Lhs, _Rhs>::type
    {
    };

#pragma endregion



    /*
     * is_less_than_comparable
     */
#pragma region is_less_than_comparable

    template <typename _Lhs, typename _Rhs>
    struct _Is_Less_Than_Comparable
    {
        template <typename _L, typename _R>
        static auto _Fn(int) ->
            decltype((std::declval<_L>() < std::declval<_R>()), std::true_type())
        {
        }

        // The compiler will prefer int over float for integral constants
        template <typename _L, typename _R>
        static auto _Fn(float) ->
            std::false_type
        {
        }

        using type = decltype(_Fn<_Lhs, _Rhs>(0));
    };

    template <typename _Lhs, typename _Rhs>
    struct is_less_than_comparable :
        _Is_Less_Than_Comparable<_Lhs, _Rhs>::type
    {
    };

#pragma endregion
}

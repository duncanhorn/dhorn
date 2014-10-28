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

    namespace garbage
    {
        template <typename Lhs, typename Rhs>
        struct IsComparable
        {
            template <typename Left, typename Right>
            static auto Fn(int) ->
                decltype((std::declval<Left>() == std::declval<Right>()), std::true_type())
            {
            }

            // The compiler will prefer int over float for integral constants
            template <typename Left, typename Right>
            static auto Fn(float) ->
                std::false_type
            {
            }

            using type = decltype(Fn<Lhs, Rhs>(0));
        };
    }

    template <typename Lhs, typename Rhs>
    struct is_comparable :
        public garbage::IsComparable<Lhs, Rhs>::type
    {
    };

#pragma endregion



    /*
     * is_less_than_comparable
     */
#pragma region is_less_than_comparable

    namespace garbage
    {
        template <typename Lhs, typename Rhs>
        struct IsLessThanComparable
        {
            template <typename Left, typename Right>
            static auto Fn(int) ->
                decltype((std::declval<Left>() < std::declval<Right>()), std::true_type())
            {
            }

            // The compiler will prefer int over float for integral constants
            template <typename Left, typename Right>
            static auto Fn(float) ->
                std::false_type
            {
            }

            using type = decltype(Fn<Lhs, Rhs>(0));
        };
    }

    template <typename Lhs, typename Rhs>
    struct is_less_than_comparable :
        public garbage::IsLessThanComparable<Lhs, Rhs>::type
    {
    };

#pragma endregion
}

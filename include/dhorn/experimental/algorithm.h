/*
 * Duncan Horn
 *
 * algorithm.h
 *
 * Definitions that should be in algorithm, but aren't
 */
#pragma once

#include <algorithm>

namespace dhorn
{
    namespace experimental
    {
        /*
         * Variadic max implementation
         */
#pragma region max

        template <typename T1>
        inline constexpr auto max(T1 &&val) -> decltype(auto)
        {
            return std::forward<T1>(val);
        }

        template <typename T1, typename T2>
        inline constexpr auto max(T1 &&val1, T2 &&val2) -> decltype(auto)
        {
            return (val1 > val2) ? std::forward<T1>(val1) : std::forward<T2>(val2);
        }

        template <typename T1, typename T2, typename... Args>
        inline constexpr auto max(T1 &&val1, T2 &&val2, Args &&...args) -> decltype(auto)
        {
            return max(max(std::forward<T1>(val1), std::forward<T2>(val2)), std::forward<Args>(args)...);
        }

#pragma endregion



        /*
         * Variadic min implementation
         */
#pragma region min

        template <typename T1>
        inline constexpr auto min(T1 &&val) -> decltype(auto)
        {
            return std::forward<T1>(val);
        }

        template <typename T1, typename T2>
        inline constexpr auto min(T1 &&val1, T2 &&val2) -> decltype(auto)
        {
            return (val1 < val2) ? std::forward<T1>(val1) : std::forward<T2>(val2);
        }

        template <typename T1, typename T2, typename... Args>
        inline constexpr auto min(T1 &&val1, T2 &&val2, Args &&...args) -> decltype(auto)
        {
            return min(min(std::forward<T1>(val1), std::forward<T2>(val2)), std::forward<Args>(args)...);
        }

#pragma endregion
    }
}

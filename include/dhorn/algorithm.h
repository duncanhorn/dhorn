/*
 * algorithm.h
 *
 * Duncan Horn
 *
 * Definitions that should be in algorithm, but aren't
 */
#pragma once

#include <algorithm>

namespace dhorn
{
    /*
     * Variadic max implementation
     */
#pragma region max

    template <typename T1>
    inline auto max(_In_ T1 val)
    {
        return val;
    }

    template <typename T1, typename T2>
    inline auto max(_In_ T1 val1, _In_ T2 val2)
    {
        return (val1 > val2) ? val1 : val2;
    }

    template <typename T1, typename T2, typename... Args>
    inline auto max(_In_ T1 val1, _In_ T2 val2, _In_ Args&&... args)
    {
        return max(max(val1, val2), std::forward<Args>(args)...);
    }

#pragma endregion



    /*
     * Variadic min implementation
     */
#pragma region max

    template <typename T1>
    inline auto min(_In_ T1 val)
    {
        return val;
    }

    template <typename T1, typename T2>
    inline auto min(_In_ T1 val1, _In_ T2 val2)
    {
        return (val1 < val2) ? val1 : val2;
    }

    template <typename T1, typename T2, typename... Args>
    inline auto min(_In_ T1 val1, _In_ T2 val2, _In_ Args&&... args)
    {
        return min(min(val1, val2), std::forward<Args>(args)...);
    }

#pragma endregion
}

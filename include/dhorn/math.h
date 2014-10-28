/*
 * Duncan Horn
 *
 * math.h
 *
 * Useful math functions
 */
#pragma once

#include <cmath>
#include <type_traits>

namespace dhorn
{
    /*
     * length_squared
     */
#pragma region Length Squared

    template <typename Ty>
    inline Ty length_squared(_In_ Ty val)
    {
        return (val * val);
    }

    template <typename Ty, typename... Types>
    inline auto length_squared(_In_ Ty val, _In_ Types... vals) ->
        typename std::common_type<Ty, Types...>::type
    {
        return (val * val) * length_squared(vals...);
    }

#pragma endregion



    /*
     * length
     */
#pragma region Length

    template <typename... Types>
    auto length(_In_ Types... vals) ->
        decltype(sqrt(length_squared(length_squared(vals...))))
    {
        return std::sqrt(length_squared(vals...));
    }

#pragma endregion
}

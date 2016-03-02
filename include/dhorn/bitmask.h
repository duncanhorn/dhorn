/*
 * Duncan Horn
 *
 * bitmask.h
 *
 * Helpers that deal with setting, clearing, testing, etc. bitmask values
 */
#pragma once

#include <sal.h>

namespace dhorn
{
    template <typename Ty>
    inline constexpr Ty set_flag(_In_ const Ty &value, _In_ const Ty &mask) noexcept
    {
        return value | mask;
    }

    template <typename Ty>
    inline constexpr Ty clear_flag(_In_ const Ty &value, _In_ const Ty &mask) noexcept
    {
        return value & (~mask);
    }

    template <typename Ty>
    inline constexpr Ty toggle_flag(_In_ const Ty &value, _In_ const Ty &mask) noexcept
    {
        return value ^ mask;
    }

    template <typename Ty>
    inline constexpr bool is_any_flag_set(_In_ const Ty &value, _In_ const Ty &mask) noexcept
    {
        return !!(value & mask);
    }

    template <typename Ty>
    inline constexpr bool are_all_flags_set(_In_ const Ty &value, _In_ const Ty &mask) noexcept
    {
        return (value & mask) == mask;
    }
}

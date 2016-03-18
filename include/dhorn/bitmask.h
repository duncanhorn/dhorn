/*
 * Duncan Horn
 *
 * bitmask.h
 *
 * Helpers that deal with setting, clearing, testing, etc. bitmask values
 */
#pragma once

#include "type_traits.h"

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



#ifndef DHORN_NO_BITMASK_OPERATORS

#define DHORN_DECLARE_BITMASK_OPERATORS(Type)   \
inline Type operator|(const Type &lhs, const Type &rhs)                                 \
{                                                                                       \
    using IntType = dhorn::select_unsigned<Type>;                                       \
    return static_cast<Type>(static_cast<IntType>(lhs) | static_cast<IntType>(rhs));    \
}                                                                                       \
                                                                                        \
inline Type operator&(const Type &lhs, const Type &rhs)                                 \
{                                                                                       \
    using IntType = dhorn::select_unsigned<Type>;                                       \
    return static_cast<Type>(static_cast<IntType>(lhs) & static_cast<IntType>(rhs));    \
}                                                                                       \
                                                                                        \
inline Type operator^(const Type &lhs, const Type &rhs)                                 \
{                                                                                       \
    using IntType = dhorn::select_unsigned<Type>;                                       \
    return static_cast<Type>(static_cast<IntType>(lhs) ^ static_cast<IntType>(rhs));    \
}

#endif

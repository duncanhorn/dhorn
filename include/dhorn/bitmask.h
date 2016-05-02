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
    template <typename ValueTy, typename MaskTy>
    inline constexpr ValueTy set_flag(_In_ const ValueTy &value, _In_ const MaskTy &mask) noexcept
    {
        return value | mask;
    }

    template <typename ValueTy, typename MaskTy>
    inline constexpr ValueTy clear_flag(_In_ const ValueTy &value, _In_ const MaskTy &mask) noexcept
    {
        return value & (~mask);
    }

    template <typename ValueTy, typename MaskTy>
    inline constexpr ValueTy toggle_flag(_In_ const ValueTy &value, _In_ const MaskTy &mask) noexcept
    {
        return value ^ mask;
    }

    template <typename ValueTy, typename MaskTy>
    inline constexpr bool is_any_flag_set(_In_ const ValueTy &value, _In_ const MaskTy &mask) noexcept
    {
        return !!(value & mask);
    }

    template <typename ValueTy, typename MaskTy>
    inline constexpr bool are_all_flags_set(_In_ const ValueTy &value, _In_ const MaskTy &mask) noexcept
    {
        return (value & mask) == mask;
    }

    template <typename ValueTy, typename MaskTy>
    inline constexpr bool are_all_flags_clear(_In_ const ValueTy &value, _In_ const MaskTy &mask) noexcept
    {
        return (value & mask) == 0;
    }
}



#ifndef DHORN_NO_BITMASK_OPERATORS

#define DHORN_DECLARE_BITMASK_OPERATORS(Type)   \
inline constexpr Type operator|(const Type &lhs, const Type &rhs)                       \
{                                                                                       \
    using IntType = dhorn::select_unsigned<Type>;                                       \
    return static_cast<Type>(static_cast<IntType>(lhs) | static_cast<IntType>(rhs));    \
}                                                                                       \
                                                                                        \
inline constexpr Type operator&(const Type &lhs, const Type &rhs)                       \
{                                                                                       \
    using IntType = dhorn::select_unsigned<Type>;                                       \
    return static_cast<Type>(static_cast<IntType>(lhs) & static_cast<IntType>(rhs));    \
}                                                                                       \
                                                                                        \
inline constexpr Type operator^(const Type &lhs, const Type &rhs)                       \
{                                                                                       \
    using IntType = dhorn::select_unsigned<Type>;                                       \
    return static_cast<Type>(static_cast<IntType>(lhs) ^ static_cast<IntType>(rhs));    \
}

#endif

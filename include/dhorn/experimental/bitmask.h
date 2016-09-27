/*
 * Duncan Horn
 *
 * bitmask.h
 *
 * Helpers that deal with setting, clearing, testing, etc. bitmask values
 */
#pragma once

#include <type_traits>

namespace dhorn
{
    namespace experimental
    {
        /*
         * set_flag
         *
         * Sets all bits in 'value' that are set in 'mask'.
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * std::cout << std::hex << "0x" << set_flag(0x84, 0x48) << std::endl;
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * Output:
         * 0xCC
         */
        template <typename ValueTy, typename MaskTy>
        inline constexpr ValueTy set_flag(const ValueTy &value, const MaskTy &mask) noexcept
        {
            return value | mask;
        }

        /*
         * set_flags
         *
         * Sets all bits in 'value' that are set in 'mask'.
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * std::cout << std::hex << "0x" << set_flags(0x84, 0x48) << std::endl;
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * Output:
         * 0xCC
         */
        template <typename ValueTy, typename MaskTy>
        inline constexpr ValueTy set_flags(const ValueTy &value, const MaskTy &mask) noexcept
        {
            return value | mask;
        }

        /*
         * clear_flag
         *
         * Clears all bits in 'value' that are set in 'mask'.
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * std::cout << std::hex << "0x" << clear_flag(0xCC, 0x84) << std::endl;
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * Output:
         * 0x48
         */
        template <typename ValueTy, typename MaskTy>
        inline constexpr ValueTy clear_flag(const ValueTy &value, const MaskTy &mask) noexcept
        {
            return value & (~mask);
        }

        /*
         * clear_flags
         *
         * Clears all bits in 'value' that are set in 'mask'.
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * std::cout << std::hex << "0x" << clear_flags(0xCC, 0x84) << std::endl;
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * Output:
         * 0x48
         */
        template <typename ValueTy, typename MaskTy>
        inline constexpr ValueTy clear_flags(const ValueTy &value, const MaskTy &mask) noexcept
        {
            return value & (~mask);
        }

        /*
         * toggle_flag
         *
         * Toggles all bits in 'value' that are set in 'mask'.
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * std::cout << std::hex << "0x" << toggle_flag(0xAA, 0x66) << std::endl;
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * Output:
         * 0xCC
         */
        template <typename ValueTy, typename MaskTy>
        inline constexpr ValueTy toggle_flag(const ValueTy &value, const MaskTy &mask) noexcept
        {
            return value ^ mask;
        }

        /*
         * toggle_flags
         *
         * Toggles all bits in 'value' that are set in 'mask'.
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * std::cout << std::hex << "0x" << toggle_flags(0xAA, 0x66) << std::endl;
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * Output:
         * 0xCC
         */
        template <typename ValueTy, typename MaskTy>
        inline constexpr ValueTy toggle_flags(const ValueTy &value, const MaskTy &mask) noexcept
        {
            return value ^ mask;
        }

        /*
         * is_any_flag_set
         *
         * Returns true if any bit(s) set in 'mask' are set in 'value', else false. Note that this implies an empty mask
         * will always give a result of false.
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * std::cout << std::boolalpha;
         * std::cout << are_all_flags_clear(0x04, 0xFF) << std::endl;
         * std::cout << are_all_flags_clear(0x04, 0x0A) << std::endl;
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * Output:
         * true
         * false
         */
        template <typename ValueTy, typename MaskTy>
        inline constexpr bool is_any_flag_set(const ValueTy &value, const MaskTy &mask) noexcept
        {
            return !!(value & mask);
        }

        /*
         * are_all_flags_set
         *
         * Returns true if all bits set in 'mask' are set in 'value', else false. Note that this implies an empty mask
         * will always give a result of true.
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * std::cout << std::boolalpha;
         * std::cout << are_all_flags_clear(0x04, 0xFF) << std::endl;
         * std::cout << are_all_flags_clear(0x04, 0x04) << std::endl;
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * Output:
         * false
         * true
         */
        template <typename ValueTy, typename MaskTy>
        inline constexpr bool are_all_flags_set(const ValueTy &value, const MaskTy &mask) noexcept
        {
            return (value & mask) == mask;
        }

        /*
         * are_all_flags_clear
         *
         * Returns true if all bits set in 'mask' are not set in 'value', else false. Note that this implies an empty
         * mask will always give a result of true.
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * std::cout << std::boolalpha;
         * std::cout << are_all_flags_clear(0x04, 0xC2) << std::endl;
         * std::cout << are_all_flags_clear(0x04, 0xFF) << std::endl;
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * Output:
         * true
         * false
         */
        template <typename ValueTy, typename MaskTy>
        inline constexpr bool are_all_flags_clear(const ValueTy &value, const MaskTy &mask) noexcept
        {
            return (value & mask) == 0;
        }
    }
}



#ifndef DHORN_NO_BITMASK_OPERATORS

#define DHORN_DECLARE_BITMASK_OPERATORS(Type)   \
inline constexpr Type operator|(const Type &lhs, const Type &rhs)                       \
{                                                                                       \
    using IntType = std::underlying_type_t<Type>;                                       \
    return static_cast<Type>(static_cast<IntType>(lhs) | static_cast<IntType>(rhs));    \
}                                                                                       \
                                                                                        \
inline constexpr Type operator&(const Type &lhs, const Type &rhs)                       \
{                                                                                       \
    using IntType = std::underlying_type_t<Type>;                                       \
    return static_cast<Type>(static_cast<IntType>(lhs) & static_cast<IntType>(rhs));    \
}                                                                                       \
                                                                                        \
inline constexpr Type operator^(const Type &lhs, const Type &rhs)                       \
{                                                                                       \
    using IntType = std::underlying_type_t<Type>;                                       \
    return static_cast<Type>(static_cast<IntType>(lhs) ^ static_cast<IntType>(rhs));    \
}

#endif

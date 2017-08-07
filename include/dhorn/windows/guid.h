/*
 * Duncan Horn
 *
 * guid.h
 *
 * Helper functions and types for the Windows `GUID` structure.
 */
#pragma once

#include <windows.h>

#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>

namespace dhorn::windows
{
    /*
     * Helpers
     */
    namespace details
    {
        // {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
        constexpr size_t guid_string_length = 38;
        constexpr const char* guid_fmt = "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}";

        // TODO: Need to be able to handle wchar_t for operator<<
        inline std::array<char, guid_string_length + 1> guid_to_string(const GUID& guid)
        {
            std::array<char, guid_string_length + 1> result;
            std::snprintf(
                result.data(), result.size(), guid_fmt,
                guid.Data1,
                guid.Data2,
                guid.Data3,
                guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
                guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
            return result;
        }
    }



    /*
     * guid_compare
     *
     * A "memcmp" style comparison of two GUIDs. Returns a negative value if `lhs` is less than `rhs`, a positive value
     * if greater, and zero if they are equal.
     */
    constexpr int guid_compare(const GUID& lhs, const GUID& rhs) noexcept
    {
        if (lhs.Data1 != rhs.Data1)
        {
            return (lhs.Data1 < rhs.Data1) ? -1 : 1;
        }
        else if (lhs.Data2 != rhs.Data2)
        {
            return (lhs.Data2 < rhs.Data2) ? -1 : 1;
        }
        else if (lhs.Data3 != rhs.Data3)
        {
            return (lhs.Data3 < rhs.Data3) ? -1 : 1;
        }

        using std::size;
        for (std::size_t i = 0; i < size(lhs.Data4); ++i)
        {
            if (lhs.Data4[i] != rhs.Data4[i])
            {
                return (lhs.Data4[i] < rhs.Data4[i]) ? -1 : 1;
            }
        }

        return 0;
    }

    /*
     * fast_guid_compare
     *
     * Similar to `guid_compare`, but doesn't do element-wise comparison. E.g. the result is not necessarily guaranteed
     * to be negative if `lhs.Data1` is less than `rhs.Data1`.
     */
    constexpr int fast_guid_compare(const GUID& lhs, const GUID& rhs) noexcept
    {
        // GUIDs are 128 bits
        auto lhs64 = reinterpret_cast<const int64_t*>(&lhs);
        auto rhs64 = reinterpret_cast<const int64_t*>(&rhs);

        if (auto cmp = lhs64[0] - rhs64[0])
        {
            return (cmp < 0) ? -1 : 1;
        }

        if (auto cmp = lhs64[1] - rhs64[1])
        {
            return (cmp < 0) ? -1 : 1;
        }

        return 0;
    }



    /*
     * GUID Comparison Function Objects
     *
     * For use with containers such as `std::map`, etc.
     */
#pragma region Comparison Function Objects

    struct guid_equal_to
    {
        constexpr bool operator()(const GUID& lhs, const GUID& rhs) const noexcept
        {
            // fast_guid_compare for equality should yield the same result as guid_compare
            return fast_guid_compare(lhs, rhs) == 0;
        }
    };

    struct fast_guid_equal_to
    {
        constexpr bool operator()(const GUID& lhs, const GUID& rhs) const noexcept
        {
            return fast_guid_compare(lhs, rhs) == 0;
        }
    };

    struct guid_not_equal_to
    {
        constexpr bool operator()(const GUID& lhs, const GUID& rhs) const noexcept
        {
            // fast_guid_compare for equality should yield the same result as guid_compare
            return fast_guid_compare(lhs, rhs) != 0;
        }
    };

    struct fast_guid_not_equal_to
    {
        constexpr bool operator()(const GUID& lhs, const GUID& rhs) const noexcept
        {
            return fast_guid_compare(lhs, rhs) != 0;
        }
    };

    struct guid_greater
    {
        constexpr bool operator()(const GUID& lhs, const GUID& rhs) const noexcept
        {
            return guid_compare(lhs, rhs) > 0;
        }
    };

    struct fast_guid_greater
    {
        constexpr bool operator()(const GUID& lhs, const GUID& rhs) const noexcept
        {
            return fast_guid_compare(lhs, rhs) > 0;
        }
    };

    struct guid_greater_equal
    {
        constexpr bool operator()(const GUID& lhs, const GUID& rhs) const noexcept
        {
            return guid_compare(lhs, rhs) >= 0;
        }
    };

    struct fast_guid_greater_equal
    {
        constexpr bool operator()(const GUID& lhs, const GUID& rhs) const noexcept
        {
            return fast_guid_compare(lhs, rhs) >= 0;
        }
    };

    struct guid_less
    {
        constexpr bool operator()(const GUID& lhs, const GUID& rhs) const noexcept
        {
            return guid_compare(lhs, rhs) < 0;
        }
    };

    struct fast_guid_less
    {
        constexpr bool operator()(const GUID& lhs, const GUID& rhs) const noexcept
        {
            return fast_guid_compare(lhs, rhs) < 0;
        }
    };

    struct guid_less_equal
    {
        constexpr bool operator()(const GUID& lhs, const GUID& rhs) const noexcept
        {
            return guid_compare(lhs, rhs) <= 0;
        }
    };

    struct fast_guid_less_equal
    {
        constexpr bool operator()(const GUID& lhs, const GUID& rhs) const noexcept
        {
            return fast_guid_compare(lhs, rhs) <= 0;
        }
    };

#pragma endregion



    /*
     * guid
     *
     * An alternative to using the `GUID` type that provides additional useful functionality such as string formatting
     * and comparison operators.
     */
#pragma region guid

    class guid :
        public GUID
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        constexpr guid() :
            GUID{}
        {
        }

        constexpr guid(const GUID& value) noexcept :
            GUID{ value }
        {
        }

        constexpr guid(
            std::uint32_t data1,
            std::uint16_t data2,
            std::uint16_t data3,
            const std::array<std::uint8_t, 8>& data4) noexcept :
        GUID{ data1, data2, data3, { data4[0], data4[1], data4[2], data4[3], data4[4], data4[5], data4[6], data4[7] } }
        {
        }



        /*
         * Modifiers
         */
        constexpr guid& operator=(const GUID& guid) noexcept
        {
            static_cast<GUID&>(*this) = guid;
            return *this;
        }



        /*
         * Accessors
         */
        explicit constexpr operator bool() const noexcept
        {
            auto ptr = reinterpret_cast<const std::uint64_t*>(this);
            return (ptr[0] != 0) || (ptr[1] != 0);
        }

        std::string to_string() const
        {
            auto str = details::guid_to_string(*this);
            return std::string(str.data(), str.size() - 1);
        }
    };

    constexpr guid null_guid{};



    /*
     * Comparison Operators
     */
#pragma region Comparison Operators

    constexpr bool operator==(const guid& lhs, const guid& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) == 0;
    }

    constexpr bool operator==(const guid& lhs, const GUID& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) == 0;
    }

    constexpr bool operator==(const GUID& lhs, const guid& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) == 0;
    }

    constexpr bool operator!=(const guid& lhs, const guid& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) != 0;
    }

    constexpr bool operator!=(const GUID& lhs, const guid& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) != 0;
    }

    constexpr bool operator!=(const guid& lhs, const GUID& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) != 0;
    }

    constexpr bool operator<(const guid& lhs, const guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) < 0;
    }

    constexpr bool operator<(const GUID& lhs, const guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) < 0;
    }

    constexpr bool operator<(const guid& lhs, const GUID& rhs) noexcept
    {
        return guid_compare(lhs, rhs) < 0;
    }

    constexpr bool operator<=(const guid& lhs, const guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) <= 0;
    }

    constexpr bool operator<=(const GUID& lhs, const guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) <= 0;
    }

    constexpr bool operator<=(const guid& lhs, const GUID& rhs) noexcept
    {
        return guid_compare(lhs, rhs) <= 0;
    }

    constexpr bool operator>(const guid& lhs, const guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) > 0;
    }

    constexpr bool operator>(const GUID& lhs, const guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) > 0;
    }

    constexpr bool operator>(const guid& lhs, const GUID& rhs) noexcept
    {
        return guid_compare(lhs, rhs) > 0;
    }

    constexpr bool operator>=(const guid& lhs, const guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) >= 0;
    }

    constexpr bool operator>=(const GUID& lhs, const guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) >= 0;
    }

    constexpr bool operator>=(const guid& lhs, const GUID& rhs) noexcept
    {
        return guid_compare(lhs, rhs) >= 0;
    }

#pragma endregion



    /*
     * Stream Operators
     */
#pragma region Stream Operators

    template <typename CharTy, typename Traits>
    inline std::basic_ostream<CharTy, Traits>& operator<<(std::basic_ostream<CharTy, Traits>& stream, const guid& guid)
    {
        return stream << details::guid_to_string(guid).data();
    }

#pragma endregion

#pragma endregion



    /*
     * ref_guid
     *
     * Intended to be used in place of `const GUID&` function arguments, providing additional functionality such as
     * comparison operators, string conversion, etc.
     */
#pragma region ref_guid

    class ref_guid
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        constexpr ref_guid(const GUID& value) noexcept :
            _value(value)
        {
        }



        /*
         * Accessors
         */
        constexpr operator const GUID&() const noexcept
        {
            return this->_value;
        }

        explicit constexpr operator bool() const noexcept
        {
            auto ptr = reinterpret_cast<const std::uint64_t*>(&this->_value);
            return (ptr[0] != 0) || (ptr[1] != 0);
        }

        constexpr const GUID& get() const noexcept
        {
            return this->_value;
        }

        std::string to_string() const
        {
            auto str = details::guid_to_string(this->_value);
            return std::string(str.data(), str.size() - 1);
        }



    private:

        const GUID& _value;
    };



    /*
     * Comparison Operators
     */
#pragma region Comparison Operators

    constexpr bool operator==(const ref_guid& lhs, const ref_guid& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) == 0;
    }

    constexpr bool operator==(const guid& lhs, const ref_guid& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) == 0;
    }

    constexpr bool operator==(const ref_guid& lhs, const guid& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) == 0;
    }

    constexpr bool operator==(const GUID& lhs, const ref_guid& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) == 0;
    }

    constexpr bool operator==(const ref_guid& lhs, const GUID& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) == 0;
    }

    constexpr bool operator!=(const ref_guid& lhs, const ref_guid& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) != 0;
    }

    constexpr bool operator!=(const guid& lhs, const ref_guid& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) != 0;
    }

    constexpr bool operator!=(const ref_guid& lhs, const guid& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) != 0;
    }

    constexpr bool operator!=(const GUID& lhs, const ref_guid& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) != 0;
    }

    constexpr bool operator!=(const ref_guid& lhs, const GUID& rhs) noexcept
    {
        return fast_guid_compare(lhs, rhs) != 0;
    }

    constexpr bool operator<(const ref_guid& lhs, const ref_guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) < 0;
    }

    constexpr bool operator<(const guid& lhs, const ref_guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) < 0;
    }

    constexpr bool operator<(const ref_guid& lhs, const guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) < 0;
    }

    constexpr bool operator<(const GUID& lhs, const ref_guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) < 0;
    }

    constexpr bool operator<(const ref_guid& lhs, const GUID& rhs) noexcept
    {
        return guid_compare(lhs, rhs) < 0;
    }

    constexpr bool operator<=(const ref_guid& lhs, const ref_guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) <= 0;
    }

    constexpr bool operator<=(const guid& lhs, const ref_guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) <= 0;
    }

    constexpr bool operator<=(const ref_guid& lhs, const guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) <= 0;
    }

    constexpr bool operator<=(const GUID& lhs, const ref_guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) <= 0;
    }

    constexpr bool operator<=(const ref_guid& lhs, const GUID& rhs) noexcept
    {
        return guid_compare(lhs, rhs) <= 0;
    }

    constexpr bool operator>(const ref_guid& lhs, const ref_guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) > 0;
    }

    constexpr bool operator>(const guid& lhs, const ref_guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) > 0;
    }

    constexpr bool operator>(const ref_guid& lhs, const guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) > 0;
    }

    constexpr bool operator>(const GUID& lhs, const ref_guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) > 0;
    }

    constexpr bool operator>(const ref_guid& lhs, const GUID& rhs) noexcept
    {
        return guid_compare(lhs, rhs) > 0;
    }

    constexpr bool operator>=(const ref_guid& lhs, const ref_guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) >= 0;
    }

    constexpr bool operator>=(const guid& lhs, const ref_guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) >= 0;
    }

    constexpr bool operator>=(const ref_guid& lhs, const guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) >= 0;
    }

    constexpr bool operator>=(const GUID& lhs, const ref_guid& rhs) noexcept
    {
        return guid_compare(lhs, rhs) >= 0;
    }

    constexpr bool operator>=(const ref_guid& lhs, const GUID& rhs) noexcept
    {
        return guid_compare(lhs, rhs) >= 0;
    }

#pragma endregion



    /*
     * Stream Operators
     */
#pragma region Stream Operators

    template <typename CharTy, typename Traits>
    inline std::basic_ostream<CharTy, Traits>& operator<<(
        std::basic_ostream<CharTy, Traits>& stream,
        const ref_guid& guid)
    {
        return stream << details::guid_to_string(guid).data();
    }

#pragma endregion

#pragma endregion
}



#ifndef DHORN_NO_STD

namespace std
{
#ifndef DHORN_NO_STD_GUID_COMPARISONS

    /*
     * Comparison Specializations
     */
#pragma region Comparison Function Object Specializations

#ifndef DHORN_FAST_GUID_COMPARISON

    template <>
    struct equal_to<GUID> : public dhorn::windows::guid_equal_to {};

    template <>
    struct not_equal_to<GUID> : public dhorn::windows::guid_not_equal_to {};

    template <>
    struct greater<GUID> : public dhorn::windows::guid_greater {};

    template <>
    struct greater_equal<GUID> : public dhorn::windows::guid_greater_equal {};

    template <>
    struct less<GUID> : public dhorn::windows::guid_less {};

    template <>
    struct less_equal<GUID> : public dhorn::windows::guid_less_equal {};

#else

    template <>
    struct equal_to<GUID> : public dhorn::windows::fast_guid_equal_to {};

    template <>
    struct not_equal_to<GUID> : public dhorn::windows::fast_guid_not_equal_to {};

    template <>
    struct greater<GUID> : public dhorn::windows::fast_guid_greater {};

    template <>
    struct greater_equal<GUID> : public dhorn::windows::fast_guid_greater_equal {};

    template <>
    struct less<GUID> : public dhorn::windows::fast_guid_less {};

    template <>
    struct less_equal<GUID> : public dhorn::windows::fast_guid_less_equal {};

#endif

#pragma endregion

#endif



    /*
     * Hash Specializations
     */
#pragma region Hash Function Object Specializations

    template <>
    struct hash<GUID>
    {
        constexpr size_t operator()(const GUID& guid)
        {
            static_assert(sizeof(GUID) % sizeof(size_t) == 0);
            auto ptr = reinterpret_cast<const size_t*>(&guid);

            // Piggy-back off of basic_string_view's has function
            std::basic_string_view<size_t> str(ptr, sizeof(GUID) / sizeof(size_t));
            return std::hash<std::basic_string_view<size_t>>()(str);
        }
    };

    template <>
    struct hash<dhorn::windows::guid> :
        public hash<GUID>
    {
    };

#pragma endregion
}

#endif

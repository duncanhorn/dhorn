/*
 * Duncan Horn
 *
 * UniqueTests.cpp
 *
 * Tests for the dhorn unique class
 */
#include "stdafx.h"

#include <dhorn/unique.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(UniqueTests)
    {
        template <typename CharT>
        struct unique_basic_string_traits
        {
            using value_type = std::basic_string<CharT>;

            static constexpr bool valid(const value_type& value) noexcept
            {
                return !value.empty();
            }

            static constexpr value_type default_value() noexcept
            {
                return value_type{};
            }

            static void destroy(value_type&) noexcept
            {
                // Strings delete themselves
            }
        };
        using unique_string = unique_any<std::string, unique_basic_string_traits<char>>;
        using unique_wstring = unique_any<std::wstring, unique_basic_string_traits<wchar_t>>;
    };
}

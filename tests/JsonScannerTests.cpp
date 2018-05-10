/*
* Duncan Horn
*
* JSONTests.cpp
*
* Tests for json.h
*/
#include "stdafx.h"

#include <dhorn/experimental/json/json.h>
#include <iterator>

using namespace dhorn::experimental;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::literals;

namespace dhorn::tests
{
    template <typename CharTy>
    struct test_handler
    {
        using value_type = std::pair<json::value_type, std::basic_string<CharTy>>;
        std::vector<value_type> expected_values;

        using string_type = std::basic_string<CharTy>;

        bool on_null()
        {
            return false;
        }

        bool on_boolean(bool /*value*/)
        {
            return false;
        }

        bool on_number(/*TODO: TYPE*/)
        {
            return false;
        }

        bool on_string(string_type& /*str*/)
        {
            return false;
        }

        bool on_object_begin()
        {
            return false;
        }

        bool on_object_key(string_type& /*key*/)
        {
            return false;
        }

        bool on_object_end()
        {
            return false;
        }

        bool on_array_begin()
        {
            return false;
        }

        bool on_array_end()
        {
            return false;
        }
    };

    template <typename CharTy>
    void expect_exception(
        std::basic_string_view<CharTy> str,
        std::size_t line,
        std::size_t col,
        std::size_t abs)
    {
        try
        {
            test_handler<CharTy> handler;
            json::basic_scanner<test_handler<CharTy>, CharTy> sc(handler);
            sc(begin(str), end(str));
            Assert::Fail(L"Expected an exception");
        }
        catch (const json::bad_json& err)
        {
            Assert::AreEqual(line, err.position().line);
            Assert::AreEqual(col, err.position().column);
            Assert::AreEqual(abs, err.position().absolute);
        }
    }

    template <typename CharTy>
    void expect_events(
        std::basic_string_view<CharTy> str,
        const std::vector<typename test_handler<CharTy>::value_type>& expectedValues)
    {
        test_handler<CharTy> handler{ expectedValues };
        json::basic_scanner<test_handler<CharTy>, CharTy> sc(handler);
        sc(begin(str), end(str));
    }

    std::u16string utf16_convert(std::string_view str)
    {
        // TODO: MAKE THIS EASIER
        std::u16string result;
        result.reserve(str.length());
        std::copy(
            make_utf_iterator(str.begin()),
            make_utf_iterator(str.end()),
            utf16_output_iterator(std::back_inserter(result)));

        return result;
    }

    std::u32string utf32_convert(std::string_view str)
    {
        return std::u32string(make_utf_iterator(str.begin()), make_utf_iterator(str.end()));
    }

    template <typename CharTy>
    auto utf_convert(std::string_view str)
    {
        if constexpr (std::is_same_v<CharTy, char>)
        {
            return str;
        }
        else if constexpr (std::is_same_v<CharTy, char16_t>)
        {
            return utf16_convert(str);
        }
        else
        {
            return utf32_convert(str);
        }
    }

    template <typename Func>
    void convert_all(std::string_view str, Func&& fn)
    {
        fn(str);
        fn(static_cast<std::u16string_view>(utf16_convert(str)));
        fn(static_cast<std::u32string_view>(utf32_convert(str)));
    }

    TEST_CLASS(JsonScannerTests)
    {
        TEST_METHOD(ScanNullRootTest)
        {
            auto invalid_test = [](std::string_view str, std::size_t row, std::size_t col, std::size_t abs)
            {
                convert_all(str, [&](auto value)
                {
                    expect_exception(value, row, col, abs);
                });
            };

            invalid_test(u8"n", 1, 2, 1);
            invalid_test(u8"nu", 1, 3, 2);
            invalid_test(u8"nul", 1, 4, 3);
            invalid_test(u8"NULL", 1, 1, 0);
            invalid_test(u8"null_", 1, 5, 4);
            invalid_test(u8"nu ll", 1, 3, 2);
            invalid_test(u8"nullnull", 1, 5, 4);

            auto valid_test = [](std::string_view str)
            {
                convert_all(str, [](auto value)
                {
                    using char_type = typename decltype(value)::value_type;
                    expect_events(value, { std::make_pair(json::value_type::null, std::basic_string<char_type>{}) });
                });
            };

            valid_test(u8"null");
            valid_test(u8"null ");
            valid_test(u8" null");
            valid_test(u8" null ");
            valid_test(u8"\r\n\tnull\t\r\n");
        }
    };
}

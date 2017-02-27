/*
 * Duncan Horn
 *
 * StringTests.cpp
 *
 * Tests for string.h
 */
#include "stdafx.h"

#include <dhorn/string.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(StringTests)
        {
#pragma region trim tests
            TEST_METHOD(TrimEmptyStringTest)
            {
                std::string str;
                str = dhorn::trim(str);
                Assert::IsTrue(str.empty());
                Assert::AreEqual(0u, str.size());

                std::wstring wstr;
                wstr = dhorn::trim(wstr);
                Assert::IsTrue(wstr.empty());
                Assert::AreEqual(0u, wstr.size());
            }

            TEST_METHOD(TrimAllWhitespaceTest)
            {
                std::string str("    \t\n");
                str = dhorn::trim(str);
                Assert::IsTrue(str.empty());
                Assert::AreEqual(0u, str.size());

                std::wstring wstr(L"    \t\n");
                wstr = dhorn::trim(wstr);
                Assert::IsTrue(wstr.empty());
                Assert::AreEqual(0u, wstr.size());
            }

            TEST_METHOD(TrimSingleWordTest)
            {
                std::string str("foo");
                str = dhorn::trim(str);
                Assert::IsTrue(str == "foo");

                std::wstring wstr(L"foo");
                wstr = dhorn::trim(wstr);
                Assert::IsTrue(wstr == L"foo");
            }

            TEST_METHOD(TrimSentenceTest)
            {
                std::string str("foo bar");
                str = dhorn::trim(str);
                Assert::IsTrue(str == "foo bar");

                std::wstring wstr(L"foo bar");
                wstr = dhorn::trim(wstr);
                Assert::IsTrue(wstr == L"foo bar");
            }

            TEST_METHOD(TrimLeadingWhitespaceTest)
            {
                std::string str("   \t\nfoo");
                str = dhorn::trim(str);
                Assert::IsTrue(str == "foo");

                std::wstring wstr(L"   \t\nfoo");
                wstr = dhorn::trim(wstr);
                Assert::IsTrue(wstr == L"foo");
            }

            TEST_METHOD(TrimTrailingWhitespaceTest)
            {
                std::string str("foo   \t\n");
                str = dhorn::trim(str);
                Assert::IsTrue(str == "foo");

                std::wstring wstr(L"foo   \t\n");
                wstr = dhorn::trim(wstr);
                Assert::IsTrue(wstr == L"foo");
            }

            TEST_METHOD(CustomTrimTest)
            {
                auto values = { 'a', 'e', 'i', 'o', 'u' };

                std::string str("aeioufoo baruoiea");
                str = dhorn::trim(str, std::begin(values), std::end(values));
                Assert::IsTrue(str == "foo bar");

                std::wstring wstr(L"aeioufoo baruoiea");
                wstr = dhorn::trim(wstr, std::begin(values), std::end(values));
                Assert::IsTrue(wstr == L"foo bar");
            }

#pragma endregion



#pragma region split tests

            TEST_METHOD(SplitEmptyStringTest)
            {
                std::string str;
                auto values = dhorn::split(str, ',');
                Assert::AreEqual(1u, values.size());
                Assert::IsTrue(values[0].empty());

                std::wstring wstr;
                auto wvalues = dhorn::split(wstr, L',');
                Assert::AreEqual(1u, wvalues.size());
                Assert::IsTrue(wvalues[0].empty());
            }

            TEST_METHOD(SplitEmptyValuesTest)
            {
                std::string str(",,,");
                auto values = dhorn::split(str, ',');
                Assert::AreEqual(4u, values.size());

                std::wstring wstr(L",,,");
                auto wvalues = dhorn::split(wstr, L',');
                Assert::AreEqual(4u, wvalues.size());
            }

            TEST_METHOD(NoSplitTest)
            {
                std::string str("foo");
                auto values = dhorn::split(str, ',');
                Assert::AreEqual(1u, values.size());
                Assert::IsTrue(values[0] == "foo");

                std::wstring wstr(L"foo");
                auto wvalues = dhorn::split(wstr, L',');
                Assert::AreEqual(1u, wvalues.size());
                Assert::IsTrue(wvalues[0] == L"foo");
            }

            TEST_METHOD(MultiValueSplitTest)
            {
                std::string str("foo,bar,,foo bar,");
                auto values = dhorn::split(str, ',');
                Assert::AreEqual(5u, values.size());
                Assert::IsTrue(values[0] == "foo");
                Assert::IsTrue(values[1] == "bar");
                Assert::IsTrue(values[2] == "");
                Assert::IsTrue(values[3] == "foo bar");
                Assert::IsTrue(values[4] == "");

                std::wstring wstr(L"foo,bar,,foo bar,");
                auto wvalues = dhorn::split(wstr, L',');
                Assert::AreEqual(5u, wvalues.size());
                Assert::IsTrue(wvalues[0] == L"foo");
                Assert::IsTrue(wvalues[1] == L"bar");
                Assert::IsTrue(wvalues[2] == L"");
                Assert::IsTrue(wvalues[3] == L"foo bar");
                Assert::IsTrue(wvalues[4] == L"");
            }

            TEST_METHOD(MultiCharacterSplitTest)
            {
                auto splits = { ',', ';', '\'', ':' };

                std::string str("foo,bar;'foo bar:");
                auto values = dhorn::split(str, std::begin(splits), std::end(splits));
                Assert::AreEqual(5u, values.size());
                Assert::IsTrue(values[0] == "foo");
                Assert::IsTrue(values[1] == "bar");
                Assert::IsTrue(values[2] == "");
                Assert::IsTrue(values[3] == "foo bar");
                Assert::IsTrue(values[4] == "");

                std::wstring wstr(L"foo,bar;'foo bar:");
                auto wvalues = dhorn::split(wstr, std::begin(splits), std::end(splits));
                Assert::AreEqual(5u, wvalues.size());
                Assert::IsTrue(wvalues[0] == L"foo");
                Assert::IsTrue(wvalues[1] == L"bar");
                Assert::IsTrue(wvalues[2] == L"");
                Assert::IsTrue(wvalues[3] == L"foo bar");
                Assert::IsTrue(wvalues[4] == L"");
            }

#pragma endregion



#pragma region starts_with tests

            TEST_METHOD(StartsWithIteratorTest)
            {
                std::string str = "foobar";
                std::string foo = "foo";
                std::string bar = "bar";

                Assert::IsTrue(starts_with(std::begin(str), std::end(str), std::begin(foo), std::end(foo)));
                Assert::IsFalse(starts_with(std::begin(str), std::end(str), std::begin(bar), std::end(bar)));

                Assert::IsTrue(starts_with(std::begin(str), std::end(str), std::begin(str), std::end(str)));
                Assert::IsFalse(starts_with(std::begin(foo), std::end(foo), std::begin(str), std::end(str)));

                std::string abc = "abc";
                std::string abd = "abd";
                Assert::IsFalse(starts_with(std::begin(abc), std::end(abc), std::begin(abd), std::end(abd)));
                Assert::IsFalse(starts_with(std::begin(abd), std::end(abd), std::begin(abc), std::end(abc)));

                std::string empty;
                Assert::IsTrue(starts_with(std::begin(str), std::end(str), std::begin(empty), std::end(empty)));
                Assert::IsFalse(starts_with(std::begin(empty), std::end(empty), std::begin(str), std::end(str)));
                Assert::IsTrue(starts_with(std::begin(empty), std::end(empty), std::begin(empty), std::end(empty)));
            }

            TEST_METHOD(StartsWithStringIteratorTest)
            {
                std::string str = "foobar";
                std::string foo = "foo";
                std::string bar = "bar";

                Assert::IsTrue(starts_with(str, std::begin(foo), std::end(foo)));
                Assert::IsFalse(starts_with(str, std::begin(bar), std::end(bar)));
                Assert::IsTrue(starts_with(str, std::begin(str), std::end(str)));
                Assert::IsFalse(starts_with(foo, std::begin(str), std::end(str)));

                std::string_view strView = "foobar";
                Assert::IsTrue(starts_with(strView, std::begin(foo), std::end(foo)));
                Assert::IsFalse(starts_with(strView, std::begin(bar), std::end(bar)));
                Assert::IsTrue(starts_with(strView, std::begin(str), std::end(str)));

                null_terminated_string nullStr = "foobar";
                Assert::IsTrue(starts_with(nullStr, std::begin(foo), std::end(foo)));
                Assert::IsFalse(starts_with(nullStr, std::begin(bar), std::end(bar)));
                Assert::IsTrue(starts_with(nullStr, std::begin(str), std::end(str)));
            }

            TEST_METHOD(StartsWithStringTest)
            {
                std::string str = "foobar";
                std::string foo = "foo";
                std::string bar = "bar";

                Assert::IsTrue(starts_with(str, foo));
                Assert::IsFalse(starts_with(str, bar));
                Assert::IsTrue(starts_with(str, str));
                Assert::IsFalse(starts_with(foo, str));

                std::string_view strView = "foobar";
                Assert::IsTrue(starts_with(strView, foo));
                Assert::IsFalse(starts_with(strView, bar));
                Assert::IsTrue(starts_with(strView, str));
                Assert::IsTrue(starts_with(str, strView));

                null_terminated_string nullStr = "foobar";
                Assert::IsTrue(starts_with(nullStr, foo));
                Assert::IsFalse(starts_with(nullStr, bar));
                Assert::IsTrue(starts_with(nullStr, str));
                Assert::IsTrue(starts_with(str, nullStr));
                Assert::IsTrue(starts_with(nullStr, strView));
                Assert::IsTrue(starts_with(strView, nullStr));
            }

            TEST_METHOD(StartsWithStringStringLiteralTest)
            {
                std::string str = "foobar";
                char* foo = "foo";
                const char* bar = "bar";

                Assert::IsTrue(starts_with(str, foo));
                Assert::IsFalse(starts_with(str, bar));

                std::string_view strView = "foobar";
                Assert::IsTrue(starts_with(strView, foo));
                Assert::IsFalse(starts_with(strView, bar));

                null_terminated_string nullStr = "foobar";
                Assert::IsTrue(starts_with(nullStr, foo));
                Assert::IsFalse(starts_with(nullStr, bar));
            }

            TEST_METHOD(StartsWithStringLiteralIteratorTest)
            {
                char* str = "foobar";
                const char* constStr = "foobar";
                std::string foo = "foo";
                std::string bar = "bar";
                std::string foobar = "foobar";

                Assert::IsTrue(starts_with(str, std::begin(foo), std::end(foo)));
                Assert::IsFalse(starts_with(str, std::begin(bar), std::end(bar)));
                Assert::IsTrue(starts_with(str, std::begin(foobar), std::end(foobar)));

                Assert::IsTrue(starts_with(constStr, std::begin(foo), std::end(foo)));
                Assert::IsFalse(starts_with(constStr, std::begin(bar), std::end(bar)));
                Assert::IsTrue(starts_with(constStr, std::begin(foobar), std::end(foobar)));
            }

            TEST_METHOD(StartsWithStringLiteralStringTest)
            {
                char* str = "foobar";
                const char* constStr = "foobar";
                std::string foo = "foo";
                std::string bar = "bar";
                std::string foobar = "foobar";

                Assert::IsTrue(starts_with(str, foo));
                Assert::IsFalse(starts_with(str, bar));
                Assert::IsTrue(starts_with(str, foobar));

                Assert::IsTrue(starts_with(constStr, foo));
                Assert::IsFalse(starts_with(constStr, bar));
                Assert::IsTrue(starts_with(constStr, foobar));
            }

            TEST_METHOD(StartsWithStringLiteralTest)
            {
                char* str = "foobar";
                const char* constStr = "foobar";
                char* foo = "foo";
                const char* bar = "bar";

                Assert::IsTrue(starts_with(str, foo));
                Assert::IsFalse(starts_with(str, bar));
                Assert::IsTrue(starts_with(str, constStr));

                Assert::IsTrue(starts_with(constStr, foo));
                Assert::IsFalse(starts_with(constStr, bar));
                Assert::IsTrue(starts_with(constStr, str));
            }

#pragma endregion
        };

        TEST_CLASS(NullTerminatedStringTests)
        {
            TEST_METHOD(AssignmentTest)
            {
                // NOTE: Strictly a "does it compile" test
                null_terminated_string str1;
                null_terminated_string str2 = "foo";
                str1 = str2;

                basic_null_terminated_string<const char> constStr;
                constStr = str1;
                constStr = static_cast<const null_terminated_string>(str2);

                basic_null_terminated_string<const char> otherConstStr;
                otherConstStr = constStr;

                str1 = static_cast<char*>("foobar");
                constStr = static_cast<char*>("foobar");
                constStr = static_cast<const char*>("foobar");
            }

            TEST_METHOD(IndexOperatorTest)
            {
                null_terminated_string str = "foo";
                Assert::AreEqual('f', str[0]);
                Assert::AreEqual('o', str[1]);
                Assert::AreEqual('o', str[2]);
                Assert::AreEqual('\0', str[3]);
            }

            TEST_METHOD(EmptyTest)
            {
                null_terminated_string str;
                Assert::IsTrue(str.empty());

                str = "";
                Assert::IsTrue(str.empty());

                str = "a";
                Assert::IsFalse(str.empty());
            }

            TEST_METHOD(IterationTest)
            {
                char* testString = "foobar";
                std::string str;
                for (auto ch : null_terminated_string(testString))
                {
                    str.push_back(ch);
                }

                Assert::IsTrue(str == testString);
            }

            template <typename CharT>
            void BasicIterationTestImpl(CharT* str)
            {
                std::basic_string<std::remove_const_t<CharT>> result;
                std::for_each(
                    basic_null_terminated_string_iterator<CharT>(str),
                    basic_null_terminated_string_iterator<CharT>(),
                    [&](CharT value)
                {
                    result.push_back(value);
                });

                Assert::IsTrue(str == result);
            }

            TEST_METHOD(BasicIterationTest)
            {
                BasicIterationTestImpl("foo");
                BasicIterationTestImpl(L"foo");
                BasicIterationTestImpl(u8"foo");
                BasicIterationTestImpl(u"foo");
                BasicIterationTestImpl(U"foo");
            }

            TEST_METHOD(IteratorConstConversionTest)
            {
                // Strictly a "does it compile" test
                basic_null_terminated_string_iterator<char> itr("foo");
                basic_null_terminated_string_iterator<const char> constItr = itr;
            }

            TEST_METHOD(IteratorConstComparisonTest)
            {
                char* str = "foo";
                basic_null_terminated_string_iterator<char> itr(str);
                basic_null_terminated_string_iterator<const char> constItr(str);

                for (; itr != null_terminated_string_iterator(); ++itr, ++constItr)
                {
                    Assert::IsTrue(itr == constItr);
                    Assert::IsTrue(constItr == itr);

                    Assert::IsFalse(itr != constItr);
                    Assert::IsFalse(constItr != itr);
                }
            }
        };
    }
}

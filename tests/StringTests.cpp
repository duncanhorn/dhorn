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



#pragma region starts_with tests

            TEST_METHOD(StartsWithIteratorTest)
            {
                std::string str = "foobar";
                std::string foo = "foo";
                std::string bar = "bar";

                Assert::IsTrue(starts_with(str.begin(), str.end(), foo.begin(), foo.end()));
                Assert::IsFalse(starts_with(str.begin(), str.end(), bar.begin(), bar.end()));

                Assert::IsTrue(starts_with(str.begin(), str.end(), str.begin(), str.end()));
                Assert::IsFalse(starts_with(foo.begin(), foo.end(), str.begin(), str.end()));

                std::string abc = "abc";
                std::string abd = "abd";
                Assert::IsFalse(starts_with(abc.begin(), abc.end(), abd.begin(), abd.end()));
                Assert::IsFalse(starts_with(abd.begin(), abd.end(), abc.begin(), abc.end()));

                std::string empty;
                Assert::IsTrue(starts_with(str.begin(), str.end(), empty.begin(), empty.end()));
                Assert::IsFalse(starts_with(empty.begin(), empty.end(), str.begin(), str.end()));
                Assert::IsTrue(starts_with(empty.begin(), empty.end(), empty.begin(), empty.end()));
            }

            TEST_METHOD(StartsWithStringIteratorTest)
            {
                std::string str = "foobar";
                std::string foo = "foo";
                std::string bar = "bar";

                Assert::IsTrue(starts_with(str, foo.begin(), foo.end()));
                Assert::IsFalse(starts_with(str, bar.begin(), bar.end()));
                Assert::IsTrue(starts_with(str, str.begin(), str.end()));
                Assert::IsFalse(starts_with(foo, str.begin(), str.end()));

                std::string_view strView = "foobar";
                Assert::IsTrue(starts_with(strView, foo.begin(), foo.end()));
                Assert::IsFalse(starts_with(strView, bar.begin(), bar.end()));
                Assert::IsTrue(starts_with(strView, str.begin(), str.end()));

                const_null_terminated_string nullStr = "foobar";
                Assert::IsTrue(starts_with(nullStr, foo.begin(), foo.end()));
                Assert::IsFalse(starts_with(nullStr, bar.begin(), bar.end()));
                Assert::IsTrue(starts_with(nullStr, str.begin(), str.end()));
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

                const_null_terminated_string nullStr = "foobar";
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
                char foo[] = "foo";
                const char* bar = "bar";

                Assert::IsTrue(starts_with(str, foo));
                Assert::IsFalse(starts_with(str, bar));

                std::string_view strView = "foobar";
                Assert::IsTrue(starts_with(strView, foo));
                Assert::IsFalse(starts_with(strView, bar));

                const_null_terminated_string nullStr = "foobar";
                Assert::IsTrue(starts_with(nullStr, foo));
                Assert::IsFalse(starts_with(nullStr, bar));
            }

            TEST_METHOD(StartsWithStringLiteralIteratorTest)
            {
                char str[] = "foobar";
                const char* constStr = "foobar";
                std::string foo = "foo";
                std::string bar = "bar";
                std::string foobar = "foobar";

                Assert::IsTrue(starts_with(str, foo.begin(), foo.end()));
                Assert::IsFalse(starts_with(str, bar.begin(), bar.end()));
                Assert::IsTrue(starts_with(str, foobar.begin(), foobar.end()));

                Assert::IsTrue(starts_with(constStr, foo.begin(), foo.end()));
                Assert::IsFalse(starts_with(constStr, bar.begin(), bar.end()));
                Assert::IsTrue(starts_with(constStr, foobar.begin(), foobar.end()));
            }

            TEST_METHOD(StartsWithStringLiteralStringTest)
            {
                char str[] = "foobar";
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
                char str[] = "foobar";
                const char* constStr = "foobar";
                char foo[] = "foo";
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
                char fooString[] = "foo";
                null_terminated_string str1;
                null_terminated_string str2 = fooString;
                str1 = str2;

                basic_null_terminated_string<const char> constStr;
                constStr = str1;
                constStr = static_cast<const null_terminated_string>(str2);

                basic_null_terminated_string<const char> otherConstStr;
                otherConstStr = constStr;

                str1 = const_cast<char*>("foobar");
                constStr = const_cast<char*>("foobar");
                constStr = static_cast<const char*>("foobar");
            }

            TEST_METHOD(IndexOperatorTest)
            {
                const_null_terminated_string str = "foo";
                Assert::AreEqual('f', str[0]);
                Assert::AreEqual('o', str[1]);
                Assert::AreEqual('o', str[2]);
                Assert::AreEqual('\0', str[3]);
            }

            TEST_METHOD(EmptyTest)
            {
                const_null_terminated_string str;
                Assert::IsTrue(str.empty());

                str = "";
                Assert::IsTrue(str.empty());

                str = "a";
                Assert::IsFalse(str.empty());
            }

            TEST_METHOD(IterationTest)
            {
                char testString[] = "foobar";
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
                char fooString[] = "foo";
                basic_null_terminated_string_iterator<char> itr(fooString);
                basic_null_terminated_string_iterator<const char> constItr = itr;
            }

            TEST_METHOD(IteratorConstComparisonTest)
            {
                char str[] = "foo";
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

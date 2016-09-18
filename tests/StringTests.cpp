/*
 * Duncan Horn
 *
 * StringTests.cpp
 *
 * Tests for string.h
 */
#include "stdafx.h"

#include <dhorn/experimental/string.h>

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
                dhorn::trim(str);
                Assert::IsTrue(str.empty());
                Assert::AreEqual(0u, str.size());

                std::wstring wstr;
                dhorn::trim(wstr);
                Assert::IsTrue(wstr.empty());
                Assert::AreEqual(0u, wstr.size());
            }

            TEST_METHOD(TrimAllWhitespaceTest)
            {
                std::string str("    \t\n");
                dhorn::trim(str);
                Assert::IsTrue(str.empty());
                Assert::AreEqual(0u, str.size());

                std::wstring wstr(L"    \t\n");
                dhorn::trim(wstr);
                Assert::IsTrue(wstr.empty());
                Assert::AreEqual(0u, wstr.size());
            }

            TEST_METHOD(TrimSingleWordTest)
            {
                std::string str("foo");
                dhorn::trim(str);
                Assert::IsTrue(str == "foo");

                std::wstring wstr(L"foo");
                dhorn::trim(wstr);
                Assert::IsTrue(wstr == L"foo");
            }

            TEST_METHOD(TrimSentenceTest)
            {
                std::string str("foo bar");
                dhorn::trim(str);
                Assert::IsTrue(str == "foo bar");

                std::wstring wstr(L"foo bar");
                dhorn::trim(wstr);
                Assert::IsTrue(wstr == L"foo bar");
            }

            TEST_METHOD(TrimLeadingWhitespaceTest)
            {
                std::string str("   \t\nfoo");
                dhorn::trim(str);
                Assert::IsTrue(str == "foo");

                std::wstring wstr(L"   \t\nfoo");
                dhorn::trim(wstr);
                Assert::IsTrue(wstr == L"foo");
            }

            TEST_METHOD(TrimTrailingWhitespaceTest)
            {
                std::string str("foo   \t\n");
                dhorn::trim(str);
                Assert::IsTrue(str == "foo");

                std::wstring wstr(L"foo   \t\n");
                dhorn::trim(wstr);
                Assert::IsTrue(wstr == L"foo");
            }

            TEST_METHOD(CustomTrimTest)
            {
                auto values = { 'a', 'e', 'i', 'o', 'u' };

                std::string str("aeioufoo baruoiea");
                dhorn::trim(str, std::begin(values), std::end(values));
                Assert::IsTrue(str == "foo bar");

                std::wstring wstr(L"aeioufoo baruoiea");
                dhorn::trim(wstr, std::begin(values), std::end(values));
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
        };
    }
}

/*
 * Duncan Horn
 *
 * TypeTraitsTests.cpp
 *
 * Tests for the type_traits.h functions
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/type_traits.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(IsCStringTests)
        {
            template <typename Ty>
            void DoFunctionTest(const Ty &, bool expected)
            {
                Assert::AreEqual(expected, is_c_string<Ty>::value);
            }

            TEST_METHOD(CharStringLiteralTest)
            {
                Assert::IsTrue(is_c_string<char *>::value);
                Assert::IsTrue(is_c_string<const char *>::value);
                Assert::IsTrue(is_c_string<char * const>::value);
                Assert::IsTrue(is_c_string<const char * const>::value);

                DoFunctionTest(static_cast<char *>("foo"), true);
                DoFunctionTest(static_cast<char *>(u8"foo"), true);
                DoFunctionTest(static_cast<const char *>("foo"), true);
                DoFunctionTest(static_cast<const char *>(u8"foo"), true);
            }

            TEST_METHOD(WCharStringLiteralTest)
            {
                Assert::IsTrue(is_c_string<wchar_t *>::value);
                Assert::IsTrue(is_c_string<const wchar_t *>::value);
                Assert::IsTrue(is_c_string<wchar_t * const>::value);
                Assert::IsTrue(is_c_string<const wchar_t * const>::value);

                DoFunctionTest(static_cast<wchar_t *>(L"foo"), true);
                DoFunctionTest(static_cast<wchar_t *>(L"foo"), true);
                DoFunctionTest(static_cast<const wchar_t *>(L"foo"), true);
                DoFunctionTest(static_cast<const wchar_t *>(L"foo"), true);
            }

            TEST_METHOD(Char16StringLiteralTest)
            {
                Assert::IsTrue(is_c_string<char16_t *>::value);
                Assert::IsTrue(is_c_string<const char16_t *>::value);
                Assert::IsTrue(is_c_string<char16_t * const>::value);
                Assert::IsTrue(is_c_string<const char16_t * const>::value);

                DoFunctionTest(static_cast<char16_t *>(u"foo"), true);
                DoFunctionTest(static_cast<char16_t *>(u"foo"), true);
                DoFunctionTest(static_cast<const char16_t *>(u"foo"), true);
                DoFunctionTest(static_cast<const char16_t *>(u"foo"), true);
            }

            TEST_METHOD(Char32StringLiteralTest)
            {
                Assert::IsTrue(is_c_string<char32_t *>::value);
                Assert::IsTrue(is_c_string<const char32_t *>::value);
                Assert::IsTrue(is_c_string<char32_t * const>::value);
                Assert::IsTrue(is_c_string<const char32_t * const>::value);

                DoFunctionTest(static_cast<char32_t *>(U"foo"), true);
                DoFunctionTest(static_cast<char32_t *>(U"foo"), true);
                DoFunctionTest(static_cast<const char32_t *>(U"foo"), true);
                DoFunctionTest(static_cast<const char32_t *>(U"foo"), true);
            }

            TEST_METHOD(VoidPointerTest)
            {
                Assert::IsFalse(is_c_string<void *>::value);
                DoFunctionTest(static_cast<void *>("foo"), false);
            }

            TEST_METHOD(CharArrayTest)
            {
                char arr[] = "foo";
                Assert::IsFalse(is_c_string<decltype(arr)>::value);
                DoFunctionTest(arr, false);
            }

            TEST_METHOD(ConstCharArrayTest)
            {
                const char arr[] = "foo";
                Assert::IsFalse(is_c_string<decltype(arr)>::value);
                DoFunctionTest(arr, false);
            }

            TEST_METHOD(ValueTest)
            {
                Assert::IsFalse(is_c_string<int>::value);
                DoFunctionTest(1, false);
            }
        };
    }
}

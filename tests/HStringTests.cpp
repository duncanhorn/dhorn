/*
 * Duncan Horn
 *
 * HStringTests.cpp
 *
 * Tests for the hstring.h functions
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/hstring.h>
#include <wrl/wrappers/corewrappers.h>

#pragma comment(lib, "runtimeobject")

using namespace Microsoft::WRL::Wrappers;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(HStringReferenceTests)
        {
            template <typename StringT>
            void AssertEquals(const hstring_reference &lhs, StringT&& rhs)
            {
                int result;
                Assert::IsTrue(SUCCEEDED(::WindowsCompareStringOrdinal(
                    lhs.get(),
                    HStringReference(rhs).Get(),
                    &result)));
                Assert::AreEqual(0, result);
            }

#pragma region constructor tests

            template <typename Ty, size_t Size>
            void DoConstructorTest(Ty&& value, const wchar_t (&expected)[Size])
            {
                hstring_reference str(std::forward<Ty>(value));

                Assert::AreEqual(Size - 1, ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(DefaultConstructorTest)
            {
                hstring_reference str;
                Assert::IsFalse(static_cast<bool>(str));
                Assert::IsTrue(str.get() == nullptr);
            }

            TEST_METHOD(WStringConstructorTest)
            {
                std::wstring wstr(L"foo\0bar", 7);
                DoConstructorTest(wstr, L"foo\0bar");
            }

            TEST_METHOD(ConstArrayConstructorTest)
            {
                static const wchar_t arr[] = L"foo\0bar";
                DoConstructorTest(arr, L"foo\0bar");
            }

            TEST_METHOD(NonConstArrayConstructorTest)
            {
                static wchar_t arr[] = L"foo\0bar";
                DoConstructorTest(arr, L"foo");
            }

            TEST_METHOD(StringLiteralConstructorTest)
            {
                static const wchar_t *cstr = L"foo\0bar";
                DoConstructorTest(cstr, L"foo");
            }

            TEST_METHOD(StringLiteralWithSizeConstructorTest)
            {
                static const wchar_t *cstr = L"foo\0bar";

                hstring_reference str(cstr, 7);
                Assert::AreEqual(7u, ::WindowsGetStringLen(str.get()));

                int result;
                Assert::IsTrue(SUCCEEDED(::WindowsCompareStringOrdinal(
                    HStringReference(cstr, 7).Get(),
                    str.get(),
                    &result)));
                Assert::AreEqual(0, result);
            }

            TEST_METHOD(CopyConstructorTest)
            {
                hstring_reference str1(L"foo\0bar");
                hstring_reference str2(str1);

                Assert::IsTrue(str2.get() != nullptr);

                int result;
                Assert::IsTrue(SUCCEEDED(::WindowsCompareStringOrdinal(str1.get(), str2.get(), &result)));
                Assert::AreEqual(0, result);
            }

#pragma endregion

#pragma region assignment operator tests

            template <typename Ty, size_t Size>
            void DoAssignmentOperatorTest(Ty&& value, const wchar_t (&expected)[Size])
            {
                hstring_reference str(L"initvalue");
                str = std::forward<Ty>(value);

                Assert::AreEqual(Size - 1, ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(WStringAssignmentOperatorTest)
            {
                std::wstring wstr(L"foo\0bar", 7);
                DoAssignmentOperatorTest(wstr, L"foo\0bar");
            }

            TEST_METHOD(ConstArrayAssignmentOperatorTest)
            {
                static const wchar_t arr[] = L"foo\0bar";
                DoAssignmentOperatorTest(arr, L"foo\0bar");
            }

            TEST_METHOD(NonConstArrayAssignmentOperatorTest)
            {
                static wchar_t arr[] = L"foo\0bar";
                DoAssignmentOperatorTest(arr, L"foo");
            }

            TEST_METHOD(StringLiteralAssignmentOperatorTest)
            {
                static const wchar_t *cstr = L"foo\0bar";
                DoAssignmentOperatorTest(cstr, L"foo");
            }

            TEST_METHOD(CopyAssignmentOperatorTest)
            {
                hstring_reference str1(L"foo\0bar");
                DoAssignmentOperatorTest(str1, L"foo\0bar");

                hstring_reference str;
                const wchar_t strArray[] = L"foo\0bar";
                {
                    hstring_reference exitingStr = strArray;
                    str = exitingStr;
                }

                Assert::AreEqual(7u, ::WindowsGetStringLen(str.get()));
                AssertEquals(str, strArray);
            }

#pragma endregion

#pragma region assign tests

            template <typename Ty, size_t Size>
            void DoAssignTest(Ty&& value, const wchar_t(&expected)[Size])
            {
                hstring_reference str(L"initvalue");
                str.assign(std::forward<Ty>(value));

                Assert::AreEqual(Size - 1, ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(WStringAssignTest)
            {
                std::wstring wstr(L"foo\0bar", 7);
                DoAssignTest(wstr, L"foo\0bar");
            }

            TEST_METHOD(ConstArrayAssignTest)
            {
                static const wchar_t arr[] = L"foo\0bar";
                DoAssignTest(arr, L"foo\0bar");
            }

            TEST_METHOD(NonConstArrayAssignTest)
            {
                static wchar_t arr[] = L"foo\0bar";
                DoAssignTest(arr, L"foo");
            }

            TEST_METHOD(StringLiteralAssignTest)
            {
                static const wchar_t *cstr = L"foo\0bar";
                DoAssignTest(cstr, L"foo");
            }

            TEST_METHOD(CopyAssignTest)
            {
                hstring_reference str1(L"foo\0bar");
                DoAssignTest(str1, L"foo\0bar");
            }

#pragma endregion

#pragma region function tests

            TEST_METHOD(OperatorBoolTest)
            {
                hstring_reference str;
                Assert::IsFalse(str);

                str = L"";
                Assert::IsFalse(str);

                str = L"foo";
                Assert::IsTrue(str);

                str.reset();
                Assert::IsFalse(str);
            }

            TEST_METHOD(CStrTest)
            {
                hstring_reference str = L"foo";
                Assert::AreEqual(0, wcscmp(str.c_str(), L"foo"));
            }

            TEST_METHOD(ClearTest)
            {
                hstring_reference str = L"foo";
                str.clear();
                Assert::IsFalse(str);
                Assert::IsTrue(str.get() == nullptr);
            }

            TEST_METHOD(CopyToTest)
            {
                hstring_reference str = L"foo\0bar";
                HString hstr;
                str.copy_to(hstr.GetAddressOf());

                int result;
                Assert::IsTrue(SUCCEEDED(::WindowsCompareStringOrdinal(str.get(), hstr.Get(), &result)));
                Assert::AreEqual(0, result);
            }

            TEST_METHOD(DataTest)
            {
                hstring_reference str = L"foo";
                Assert::AreEqual(0, wcscmp(str.data(), L"foo"));
            }

            TEST_METHOD(EmptyTest)
            {
                hstring_reference str;
                Assert::IsTrue(str.empty());

                str = L"foo";
                Assert::IsFalse(str.empty());

                str.clear();
                Assert::IsTrue(str.empty());

                str = L"";
                Assert::IsTrue(str.empty());
            }

            TEST_METHOD(LengthAndSizeTest)
            {
                hstring_reference str;
                Assert::AreEqual(0u, str.length());
                Assert::AreEqual(0u, str.size());

                str = L"";
                Assert::AreEqual(0u, str.length());
                Assert::AreEqual(0u, str.size());

                str = L"foo\0bar";
                Assert::AreEqual(7u, str.length());
                Assert::AreEqual(7u, str.size());

                str.clear();
                Assert::AreEqual(0u, str.length());
                Assert::AreEqual(0u, str.size());
            }

            TEST_METHOD(ResetTest)
            {
                hstring_reference str = L"foo";
                str.reset();
                Assert::IsFalse(str);
                Assert::IsTrue(str.get() == nullptr);
            }

            TEST_METHOD(SwapTest)
            {
                hstring_reference foo = L"foo";
                hstring_reference bar = L"bar";

                foo.swap(bar);
                AssertEquals(foo, L"bar");
                AssertEquals(bar, L"foo");
            }

#pragma endregion
        };







        // Values for comparison tests
        static const hstring hfoo = L"foo";
        static const hstring hbar = L"bar";
        static const hstring hfoobar = L"foo\0bar";
        static const hstring_reference rfoo = L"foo";
        static const hstring_reference rbar = L"bar";
        static const hstring_reference rfoobar = L"foo\0bar";

        TEST_CLASS(HStringEqualityOperatorTests)
        {
            template <typename LhsTy, typename RhsTy>
            void DoEqualityTest(LhsTy &lhs, RhsTy &rhs, _In_ bool expected)
            {
                Assert::AreEqual(expected, lhs == rhs);
            }

            TEST_METHOD(HStringEqualityTest)
            {
                DoEqualityTest(hfoo, hfoo, true);
                DoEqualityTest(hfoo, hbar, false);
                DoEqualityTest(hfoo, hfoobar, false);
                DoEqualityTest(hfoobar, hfoo, false);
                DoEqualityTest(hfoobar, hfoobar, true);
            }

            TEST_METHOD(HStringReferenceEqualityTest)
            {
                DoEqualityTest(rfoo, rfoo, true);
                DoEqualityTest(rfoo, rbar, false);
                DoEqualityTest(rfoo, rfoobar, false);
                DoEqualityTest(rfoobar, rfoo, false);
                DoEqualityTest(rfoobar, rfoobar, true);
            }

            TEST_METHOD(CrossHStringEqualityTest)
            {
                DoEqualityTest(hfoo, rfoo, true);
                DoEqualityTest(hfoo, rbar, false);
                DoEqualityTest(hfoo, rfoobar, false);
                DoEqualityTest(hfoobar, rfoo, false);
                DoEqualityTest(hfoobar, rfoobar, true);

                DoEqualityTest(rfoo, hfoo, true);
                DoEqualityTest(rfoo, hbar, false);
                DoEqualityTest(rfoo, hfoobar, false);
                DoEqualityTest(rfoobar, hfoo, false);
                DoEqualityTest(rfoobar, hfoobar, true);
            }

            TEST_METHOD(HSTRINGEqualityTest)
            {
                HString hstr;
                Assert::IsTrue(SUCCEEDED(hstr.Set(L"foo\0bar")));
                auto str = hstr.Get();

                DoEqualityTest(hfoobar, str, true);
                DoEqualityTest(hfoo, str, false);
                DoEqualityTest(str, hfoobar, true);
                DoEqualityTest(str, hfoo, false);

                DoEqualityTest(rfoobar, str, true);
                DoEqualityTest(rfoo, str, false);
                DoEqualityTest(str, rfoobar, true);
                DoEqualityTest(str, rfoo, false);
            }

            TEST_METHOD(CStringEqualityTest)
            {
                const wchar_t *cstr = L"foo\0bar";

                DoEqualityTest(hfoobar, cstr, false);
                DoEqualityTest(hfoo, cstr, true);
                DoEqualityTest(cstr, hfoobar, false);
                DoEqualityTest(cstr, hfoo, true);

                DoEqualityTest(rfoobar, cstr, false);
                DoEqualityTest(rfoo, cstr, true);
                DoEqualityTest(cstr, rfoobar, false);
                DoEqualityTest(cstr, rfoo, true);
            }

            TEST_METHOD(ConstArrayEqualityTest)
            {
                const wchar_t arr[] = L"foo\0bar";

                DoEqualityTest(hfoobar, arr, true);
                DoEqualityTest(hfoo, arr, false);
                DoEqualityTest(arr, hfoobar, true);
                DoEqualityTest(arr, hfoo, false);

                DoEqualityTest(rfoobar, arr, true);
                DoEqualityTest(rfoo, arr, false);
                DoEqualityTest(arr, rfoobar, true);
                DoEqualityTest(arr, rfoo, false);
            }

            TEST_METHOD(NonConstArrayEqualityTest)
            {
                wchar_t arr[] = L"foo\0bar";

                DoEqualityTest(hfoobar, arr, false);
                DoEqualityTest(hfoo, arr, true);
                DoEqualityTest(arr, hfoobar, false);
                DoEqualityTest(arr, hfoo, true);

                DoEqualityTest(rfoobar, arr, false);
                DoEqualityTest(rfoo, arr, true);
                DoEqualityTest(arr, rfoobar, false);
                DoEqualityTest(arr, rfoo, true);
            }

            TEST_METHOD(WStringEqualityTest)
            {
                std::wstring str(L"foo\0bar", 7);

                DoEqualityTest(hfoobar, str, true);
                DoEqualityTest(hfoo, str, false);
                DoEqualityTest(str, hfoobar, true);
                DoEqualityTest(str, hfoo, false);

                DoEqualityTest(rfoobar, str, true);
                DoEqualityTest(rfoo, str, false);
                DoEqualityTest(str, rfoobar, true);
                DoEqualityTest(str, rfoo, false);
            }
        };

        TEST_CLASS(HStringInequalityOperatorTests)
        {
            template <typename LhsTy, typename RhsTy>
            void DoEqualityTest(LhsTy &lhs, RhsTy &rhs, _In_ bool expected)
            {
                Assert::AreEqual(expected, lhs != rhs);
            }

            TEST_METHOD(HStringInequalityTest)
            {
                DoEqualityTest(hfoo, hfoo, false);
                DoEqualityTest(hfoo, hbar, true);
                DoEqualityTest(hfoo, hfoobar, true);
                DoEqualityTest(hfoobar, hfoo, true);
                DoEqualityTest(hfoobar, hfoobar, false);
            }

            TEST_METHOD(HStringReferenceInequalityTest)
            {
                DoEqualityTest(rfoo, rfoo, false);
                DoEqualityTest(rfoo, rbar, true);
                DoEqualityTest(rfoo, rfoobar, true);
                DoEqualityTest(rfoobar, rfoo, true);
                DoEqualityTest(rfoobar, rfoobar, false);
            }

            TEST_METHOD(CrossHStringInequalityTest)
            {
                DoEqualityTest(hfoo, rfoo, false);
                DoEqualityTest(hfoo, rbar, true);
                DoEqualityTest(hfoo, rfoobar, true);
                DoEqualityTest(hfoobar, rfoo, true);
                DoEqualityTest(hfoobar, rfoobar, false);

                DoEqualityTest(rfoo, hfoo, false);
                DoEqualityTest(rfoo, hbar, true);
                DoEqualityTest(rfoo, hfoobar, true);
                DoEqualityTest(rfoobar, hfoo, true);
                DoEqualityTest(rfoobar, hfoobar, false);
            }

            TEST_METHOD(HSTRINGInequalityTest)
            {
                HString hstr;
                Assert::IsTrue(SUCCEEDED(hstr.Set(L"foo\0bar")));
                auto str = hstr.Get();

                DoEqualityTest(hfoobar, str, false);
                DoEqualityTest(hfoo, str, true);
                DoEqualityTest(str, hfoobar, false);
                DoEqualityTest(str, hfoo, true);

                DoEqualityTest(rfoobar, str, false);
                DoEqualityTest(rfoo, str, true);
                DoEqualityTest(str, rfoobar, false);
                DoEqualityTest(str, rfoo, true);
            }

            TEST_METHOD(CStringInequalityTest)
            {
                const wchar_t *cstr = L"foo\0bar";

                DoEqualityTest(hfoobar, cstr, true);
                DoEqualityTest(hfoo, cstr, false);
                DoEqualityTest(cstr, hfoobar, true);
                DoEqualityTest(cstr, hfoo, false);

                DoEqualityTest(rfoobar, cstr, true);
                DoEqualityTest(rfoo, cstr, false);
                DoEqualityTest(cstr, rfoobar, true);
                DoEqualityTest(cstr, rfoo, false);
            }

            TEST_METHOD(ConstArrayInequalityTest)
            {
                const wchar_t arr[] = L"foo\0bar";

                DoEqualityTest(hfoobar, arr, false);
                DoEqualityTest(hfoo, arr, true);
                DoEqualityTest(arr, hfoobar, false);
                DoEqualityTest(arr, hfoo, true);

                DoEqualityTest(rfoobar, arr, false);
                DoEqualityTest(rfoo, arr, true);
                DoEqualityTest(arr, rfoobar, false);
                DoEqualityTest(arr, rfoo, true);
            }

            TEST_METHOD(NonConstArrayInequalityTest)
            {
                wchar_t arr[] = L"foo\0bar";

                DoEqualityTest(hfoobar, arr, true);
                DoEqualityTest(hfoo, arr, false);
                DoEqualityTest(arr, hfoobar, true);
                DoEqualityTest(arr, hfoo, false);

                DoEqualityTest(rfoobar, arr, true);
                DoEqualityTest(rfoo, arr, false);
                DoEqualityTest(arr, rfoobar, true);
                DoEqualityTest(arr, rfoo, false);
            }

            TEST_METHOD(WStringInequalityTest)
            {
                std::wstring str(L"foo\0bar", 7);

                DoEqualityTest(hfoobar, str, false);
                DoEqualityTest(hfoo, str, true);
                DoEqualityTest(str, hfoobar, false);
                DoEqualityTest(str, hfoo, true);

                DoEqualityTest(rfoobar, str, false);
                DoEqualityTest(rfoo, str, true);
                DoEqualityTest(str, rfoobar, false);
                DoEqualityTest(str, rfoo, true);
            }
        };

        TEST_CLASS(HStringLessThanOperatorTests)
        {
            template <typename LhsTy, typename RhsTy>
            void DoEqualityTest(LhsTy &lhs, RhsTy &rhs, _In_ bool expected)
            {
                Assert::AreEqual(expected, lhs < rhs);
            }

            TEST_METHOD(HStringLessThanTest)
            {
                DoEqualityTest(hfoo, hfoo, false);
                DoEqualityTest(hfoo, hbar, false);
                DoEqualityTest(hbar, hfoo, true);
                DoEqualityTest(hfoo, hfoobar, true);
                DoEqualityTest(hfoobar, hfoo, false);
                DoEqualityTest(hfoobar, hfoobar, false);
            }

            TEST_METHOD(HStringReferenceLessThanTest)
            {
                DoEqualityTest(rfoo, rfoo, false);
                DoEqualityTest(rfoo, rbar, false);
                DoEqualityTest(rbar, rfoo, true);
                DoEqualityTest(rfoo, rfoobar, true);
                DoEqualityTest(rfoobar, rfoo, false);
                DoEqualityTest(rfoobar, rfoobar, false);
            }

            TEST_METHOD(CrossHStringLessThanTest)
            {
                DoEqualityTest(hfoo, rfoo, false);
                DoEqualityTest(hfoo, rbar, false);
                DoEqualityTest(hbar, rfoo, true);
                DoEqualityTest(hfoo, rfoobar, true);
                DoEqualityTest(hfoobar, rfoo, false);
                DoEqualityTest(hfoobar, rfoobar, false);

                DoEqualityTest(rfoo, hfoo, false);
                DoEqualityTest(rfoo, hbar, false);
                DoEqualityTest(rbar, hfoo, true);
                DoEqualityTest(rfoo, hfoobar, true);
                DoEqualityTest(rfoobar, hfoo, false);
                DoEqualityTest(rfoobar, hfoobar, false);
            }

            TEST_METHOD(HSTRINGLessThanTest)
            {
                HString hstr;
                Assert::IsTrue(SUCCEEDED(hstr.Set(L"foo\0bar")));
                auto str = hstr.Get();

                DoEqualityTest(hfoobar, str, false);
                DoEqualityTest(hfoo, str, true);
                DoEqualityTest(str, hfoobar, false);
                DoEqualityTest(str, hfoo, false);

                DoEqualityTest(rfoobar, str, false);
                DoEqualityTest(rfoo, str, true);
                DoEqualityTest(str, rfoobar, false);
                DoEqualityTest(str, rfoo, false);
            }

            TEST_METHOD(CStringLessThanTest)
            {
                const wchar_t *cstr = L"foo\0bar";

                DoEqualityTest(hfoobar, cstr, false);
                DoEqualityTest(hfoo, cstr, false);
                DoEqualityTest(cstr, hfoobar, true);
                DoEqualityTest(cstr, hfoo, false);

                DoEqualityTest(rfoobar, cstr, false);
                DoEqualityTest(rfoo, cstr, false);
                DoEqualityTest(cstr, rfoobar, true);
                DoEqualityTest(cstr, rfoo, false);
            }

            TEST_METHOD(ConstArrayLessThanTest)
            {
                const wchar_t arr[] = L"foo\0bar";

                DoEqualityTest(hfoobar, arr, false);
                DoEqualityTest(hfoo, arr, true);
                DoEqualityTest(arr, hfoobar, false);
                DoEqualityTest(arr, hfoo, false);

                DoEqualityTest(rfoobar, arr, false);
                DoEqualityTest(rfoo, arr, true);
                DoEqualityTest(arr, rfoobar, false);
                DoEqualityTest(arr, rfoo, false);
            }

            TEST_METHOD(NonConstArrayLessThanTest)
            {
                wchar_t arr[] = L"foo\0bar";

                DoEqualityTest(hfoobar, arr, false);
                DoEqualityTest(hfoo, arr, false);
                DoEqualityTest(arr, hfoobar, true);
                DoEqualityTest(arr, hfoo, false);

                DoEqualityTest(rfoobar, arr, false);
                DoEqualityTest(rfoo, arr, false);
                DoEqualityTest(arr, rfoobar, true);
                DoEqualityTest(arr, rfoo, false);
            }

            TEST_METHOD(WStringLessThanTest)
            {
                std::wstring str(L"foo\0bar", 7);

                DoEqualityTest(hfoobar, str, false);
                DoEqualityTest(hfoo, str, true);
                DoEqualityTest(str, hfoobar, false);
                DoEqualityTest(str, hfoo, false);

                DoEqualityTest(rfoobar, str, false);
                DoEqualityTest(rfoo, str, true);
                DoEqualityTest(str, rfoobar, false);
                DoEqualityTest(str, rfoo, false);
            }
        };
    }
}

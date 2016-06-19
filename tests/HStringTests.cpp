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

        TEST_CLASS(HStringTests)
        {
            template <typename StringT>
            void AssertEquals(const hstring &lhs, StringT&& rhs)
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
            void DoConstructorTest(Ty&& value, const wchar_t(&expected)[Size])
            {
                hstring str(std::forward<Ty>(value));

                Assert::AreEqual(Size - 1, ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(DefaultConstructorTest)
            {
                hstring str;
                Assert::IsFalse(static_cast<bool>(str));
                Assert::IsTrue(str.get() == nullptr);
            }

            TEST_METHOD(HStringConstructorTest)
            {
                HSTRING hstr;
                Assert::IsTrue(SUCCEEDED(::WindowsCreateString(L"foo\0bar", 7, &hstr)));

                DoConstructorTest(hstr, L"foo\0bar");

                ::WindowsDeleteString(hstr);
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

                hstring str(cstr, 7);
                Assert::AreEqual(7u, ::WindowsGetStringLen(str.get()));

                int result;
                Assert::IsTrue(SUCCEEDED(::WindowsCompareStringOrdinal(
                    HStringReference(cstr, 7).Get(),
                    str.get(),
                    &result)));
                Assert::AreEqual(0, result);
            }

            TEST_METHOD(IteratorConstructorTest)
            {
                std::wstring wstr(L"foo\0bar", 7);

                hstring str(std::begin(wstr), std::end(wstr));
                Assert::AreEqual(7u, ::WindowsGetStringLen(str.get()));

                int result;
                Assert::IsTrue(SUCCEEDED(::WindowsCompareStringOrdinal(
                    HStringReference(wstr.c_str(), wstr.length()).Get(),
                    str.get(),
                    &result)));
                Assert::AreEqual(0, result);
            }

            TEST_METHOD(CopyConstructorTest)
            {
                hstring str1(L"foo\0bar");
                hstring str2(str1);

                Assert::IsTrue(str2.get() != nullptr);

                int result;
                Assert::IsTrue(SUCCEEDED(::WindowsCompareStringOrdinal(str1.get(), str2.get(), &result)));
                Assert::AreEqual(0, result);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                hstring str1(L"foo\0bar");
                auto hstr = str1.get();

                hstring str2(std::move(str1));
                Assert::IsTrue(hstr == str2.get());
                Assert::IsTrue(str1.get() == nullptr);
            }

#pragma endregion

#pragma region assignment operator tests

            template <typename Ty, size_t Size>
            void DoAssignmentOperatorTest(Ty&& value, const wchar_t(&expected)[Size])
            {
                hstring str(L"initvalue");
                str = std::forward<Ty>(value);

                Assert::AreEqual(Size - 1, ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(HStringAssignmentOperatorTest)
            {
                HSTRING hstr;
                Assert::IsTrue(SUCCEEDED(::WindowsCreateString(L"foo\0bar", 7, &hstr)));

                DoAssignmentOperatorTest(hstr, L"foo\0bar");

                ::WindowsDeleteString(hstr);
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
                hstring str1(L"foo\0bar");
                DoAssignmentOperatorTest(str1, L"foo\0bar");

                hstring str;
                {
                    hstring exitingStr = L"foo\0bar";
                    str = exitingStr;
                }

                Assert::AreEqual(7u, ::WindowsGetStringLen(str.get()));
            }

            TEST_METHOD(MoveAssignmentOperatorTest)
            {
                hstring str1(L"foo\0bar");
                auto hstr = str1.get();

                hstring str2(L"initvalue");
                str2 = std::move(str1);

                Assert::IsTrue(hstr == str2.get());

                hstring str;
                {
                    hstring exitingStr = L"foo\0bar";
                    str = std::move(exitingStr);
                }

                Assert::AreEqual(7u, ::WindowsGetStringLen(str.get()));
            }

#pragma endregion

#pragma region assign tests

            template <typename Ty, size_t Size>
            void DoAssignTest(Ty&& value, const wchar_t(&expected)[Size])
            {
                hstring str(L"initvalue");
                str.assign(std::forward<Ty>(value));

                Assert::AreEqual(Size - 1, ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(HStringAssignTest)
            {
                HSTRING hstr;
                Assert::IsTrue(SUCCEEDED(::WindowsCreateString(L"foo\0bar", 7, &hstr)));

                DoAssignTest(hstr, L"foo\0bar");

                ::WindowsDeleteString(hstr);
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
                hstring str1(L"foo\0bar");
                DoAssignTest(str1, L"foo\0bar");
            }

            TEST_METHOD(MoveAssignTest)
            {
                hstring str1(L"foo\0bar");
                auto hstr = str1.get();

                hstring str2(L"initvalue");
                str2.assign(std::move(str1));

                Assert::IsTrue(hstr == str2.get());
            }

#pragma endregion

#pragma region append operator tests

            template <typename Ty, size_t Size>
            void DoAppendOperatorTest(Ty&& value, const wchar_t(&expected)[Size])
            {
                hstring str(L"foo");
                str += std::forward<Ty>(value);

                Assert::AreEqual(Size - 1, ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(HStringAppendOperatorTest)
            {
                HSTRING hstr;
                Assert::IsTrue(SUCCEEDED(::WindowsCreateString(L"bar\0car", 7, &hstr)));

                DoAppendOperatorTest(hstr, L"foobar\0car");

                ::WindowsDeleteString(hstr);
            }

            TEST_METHOD(WStringAppendOperatorTest)
            {
                std::wstring wstr(L"bar\0car", 7);
                DoAppendOperatorTest(wstr, L"foobar\0car");
            }

            TEST_METHOD(ConstArrayAppendOperatorTest)
            {
                static const wchar_t arr[] = L"bar\0car";
                DoAppendOperatorTest(arr, L"foobar\0car");
            }

            TEST_METHOD(NonConstArrayAppendOperatorTest)
            {
                static wchar_t arr[] = L"bar\0car";
                DoAppendOperatorTest(arr, L"foobar");
            }

            TEST_METHOD(StringLiteralAppendOperatorTest)
            {
                static const wchar_t *cstr = L"bar\0car";
                DoAppendOperatorTest(cstr, L"foobar");
            }

            TEST_METHOD(WrlppHStringAppendOperatorTest)
            {
                hstring str(L"bar\0car");
                DoAppendOperatorTest(str, L"foobar\0car");
            }

            TEST_METHOD(SelfAppendOperatorTest)
            {
                hstring str(L"foo");
                str += str;
                AssertEquals(str, L"foofoo");
            }

#pragma endregion

#pragma region append tests

            template <typename Ty, size_t Size>
            void DoAppendTest(Ty&& value, const wchar_t(&expected)[Size])
            {
                hstring str(L"foo");
                str.append(std::forward<Ty>(value));

                Assert::AreEqual(Size - 1, ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(HStringAppendTest)
            {
                HSTRING hstr;
                Assert::IsTrue(SUCCEEDED(::WindowsCreateString(L"bar\0car", 7, &hstr)));

                DoAppendTest(hstr, L"foobar\0car");

                ::WindowsDeleteString(hstr);
            }

            TEST_METHOD(WStringAppendTest)
            {
                std::wstring wstr(L"bar\0car", 7);
                DoAppendTest(wstr, L"foobar\0car");
            }

            TEST_METHOD(ConstArrayAppendTest)
            {
                static const wchar_t arr[] = L"bar\0car";
                DoAppendTest(arr, L"foobar\0car");
            }

            TEST_METHOD(NonConstArrayAppendTest)
            {
                static wchar_t arr[] = L"bar\0car";
                DoAppendTest(arr, L"foobar");
            }

            TEST_METHOD(StringLiteralAppendTest)
            {
                static const wchar_t *cstr = L"bar\0car";
                DoAppendTest(cstr, L"foobar");
            }

            TEST_METHOD(StringWithLengthAppendTest)
            {
                static const wchar_t *cstr = L"bar\0car";

                hstring str(L"foo");
                str.append(cstr, 7);

                Assert::AreEqual(10u, ::WindowsGetStringLen(str.get()));
                AssertEquals(str, L"foobar\0car");
            }

            TEST_METHOD(WrlppHStringAppendTest)
            {
                hstring str(L"bar\0car");
                DoAppendTest(str, L"foobar\0car");
            }

            TEST_METHOD(SelfAppendTest)
            {
                hstring str(L"foo");
                str.append(str);
                AssertEquals(str, L"foofoo");
            }

#pragma endregion

#pragma region other operator tests

            TEST_METHOD(AddressOfOperatorTest)
            {
                hstring str(L"foo");
                auto ptr = &str;

                Assert::IsTrue(std::is_same<decltype(ptr), HSTRING *>::value);
                Assert::IsTrue(*ptr == str.get());
            }

            TEST_METHOD(OperatorBoolTest)
            {
                hstring str;
                Assert::IsFalse(str);

                str = L"";
                Assert::IsFalse(str);

                str = L"foo";
                Assert::IsTrue(str);

                str.reset();
                Assert::IsFalse(str);
            }

#pragma endregion

#pragma region function tests

            TEST_METHOD(AttachTest)
            {
                HSTRING hstr;
                Assert::IsTrue(SUCCEEDED(::WindowsCreateString(L"foo", 3, &hstr)));

                hstring str;
                str.attach(hstr);
                Assert::IsTrue(str.get() == hstr);
            }

            TEST_METHOD(CStrTest)
            {
                hstring str = L"foo";
                Assert::AreEqual(0, wcscmp(str.c_str(), L"foo"));
            }

            TEST_METHOD(ClearTest)
            {
                hstring str = L"foo";
                str.clear();
                Assert::IsFalse(str);
                Assert::IsTrue(str.get() == nullptr);
            }

            TEST_METHOD(CopyToTest)
            {
                hstring str = L"foo\0bar";
                HString hstr;
                str.copy_to(hstr.GetAddressOf());

                int result;
                Assert::IsTrue(SUCCEEDED(::WindowsCompareStringOrdinal(str.get(), hstr.Get(), &result)));
                Assert::AreEqual(0, result);
            }

            TEST_METHOD(DataTest)
            {
                hstring str = L"foo";
                Assert::AreEqual(0, wcscmp(str.data(), L"foo"));
            }

            TEST_METHOD(DetachTest)
            {
                hstring str = L"foo";
                auto value = str.get();

                auto hstr = str.detach();
                Assert::IsTrue(hstr == value);

                ::WindowsDeleteString(hstr);
            }

            TEST_METHOD(EmptyTest)
            {
                hstring str;
                Assert::IsTrue(str.empty());

                str = L"foo";
                Assert::IsFalse(str.empty());

                str.clear();
                Assert::IsTrue(str.empty());

                str = L"";
                Assert::IsTrue(str.empty());

                str = L"foo";
                auto hstr = str.detach();
                Assert::IsTrue(str.empty());

                ::WindowsDeleteString(hstr);
            }

            TEST_METHOD(LengthAndSizeTest)
            {
                hstring str;
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

                str = L"foo";
                auto hstr = str.detach();
                Assert::AreEqual(0u, str.length());
                Assert::AreEqual(0u, str.size());

                ::WindowsDeleteString(hstr);
            }

            TEST_METHOD(ReleaseTest)
            {
                hstring str = L"foo";
                auto value = str.get();

                auto hstr = str.release();
                Assert::IsTrue(hstr == value);

                ::WindowsDeleteString(hstr);
            }

            TEST_METHOD(ResetTest)
            {
                hstring str = L"foo";
                str.reset();
                Assert::IsFalse(str);
                Assert::IsTrue(str.get() == nullptr);
            }

            TEST_METHOD(SwapTest)
            {
                hstring foo = L"foo";
                hstring bar = L"bar";

                foo.swap(bar);
                AssertEquals(foo, L"bar");
                AssertEquals(bar, L"foo");
            }

#pragma endregion
        };



        // Values for comparison tests
        static hstring hfoo = L"foo";
        static hstring hbar = L"bar";
        static hstring hfoobar = L"foo\0bar";
        static hstring_reference rfoo = L"foo";
        static hstring_reference rbar = L"bar";
        static hstring_reference rfoobar = L"foo\0bar";

        TEST_CLASS(HStringEqualityOperatorTests)
        {
            template <typename LhsTy, typename RhsTy>
            void DoComparisonTest(LhsTy &&lhs, RhsTy &&rhs, _In_ bool expected)
            {
                Assert::AreEqual(expected, lhs == rhs);

                if (!std::is_array<std::remove_reference_t<LhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<LhsTy>>::value)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoComparisonTest(other, rhs, expected);
                }

                if (!std::is_array<std::remove_reference_t<RhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<RhsTy>>::value)
                {
                    const std::remove_reference_t<RhsTy> &other = rhs;
                    DoComparisonTest(lhs, other, expected);
                }
            }

            TEST_METHOD(HStringEqualityTest)
            {
                DoComparisonTest(hfoo, hfoo, true);
                DoComparisonTest(hfoo, hbar, false);
                DoComparisonTest(hfoo, hfoobar, false);
                DoComparisonTest(hfoobar, hfoo, false);
                DoComparisonTest(hfoobar, hfoobar, true);
            }

            TEST_METHOD(HStringReferenceEqualityTest)
            {
                DoComparisonTest(rfoo, rfoo, true);
                DoComparisonTest(rfoo, rbar, false);
                DoComparisonTest(rfoo, rfoobar, false);
                DoComparisonTest(rfoobar, rfoo, false);
                DoComparisonTest(rfoobar, rfoobar, true);
            }

            TEST_METHOD(CrossHStringEqualityTest)
            {
                DoComparisonTest(hfoo, rfoo, true);
                DoComparisonTest(hfoo, rbar, false);
                DoComparisonTest(hfoo, rfoobar, false);
                DoComparisonTest(hfoobar, rfoo, false);
                DoComparisonTest(hfoobar, rfoobar, true);

                DoComparisonTest(rfoo, hfoo, true);
                DoComparisonTest(rfoo, hbar, false);
                DoComparisonTest(rfoo, hfoobar, false);
                DoComparisonTest(rfoobar, hfoo, false);
                DoComparisonTest(rfoobar, hfoobar, true);
            }

            TEST_METHOD(HSTRINGEqualityTest)
            {
                HString hstr;
                Assert::IsTrue(SUCCEEDED(hstr.Set(L"foo\0bar")));
                auto str = hstr.Get();

                DoComparisonTest(hfoobar, str, true);
                DoComparisonTest(hfoo, str, false);
                DoComparisonTest(str, hfoobar, true);
                DoComparisonTest(str, hfoo, false);

                DoComparisonTest(rfoobar, str, true);
                DoComparisonTest(rfoo, str, false);
                DoComparisonTest(str, rfoobar, true);
                DoComparisonTest(str, rfoo, false);
            }

            TEST_METHOD(CStringEqualityTest)
            {
                const wchar_t *cstr = L"foo\0bar";

                DoComparisonTest(hfoobar, cstr, false);
                DoComparisonTest(hfoo, cstr, true);
                DoComparisonTest(cstr, hfoobar, false);
                DoComparisonTest(cstr, hfoo, true);

                DoComparisonTest(rfoobar, cstr, false);
                DoComparisonTest(rfoo, cstr, true);
                DoComparisonTest(cstr, rfoobar, false);
                DoComparisonTest(cstr, rfoo, true);
            }

            TEST_METHOD(ConstArrayEqualityTest)
            {
                const wchar_t arr[] = L"foo\0bar";

                DoComparisonTest(hfoobar, arr, true);
                DoComparisonTest(hfoo, arr, false);
                DoComparisonTest(arr, hfoobar, true);
                DoComparisonTest(arr, hfoo, false);

                DoComparisonTest(rfoobar, arr, true);
                DoComparisonTest(rfoo, arr, false);
                DoComparisonTest(arr, rfoobar, true);
                DoComparisonTest(arr, rfoo, false);
            }

            TEST_METHOD(NonConstArrayEqualityTest)
            {
                wchar_t arr[] = L"foo\0bar";

                DoComparisonTest(hfoobar, arr, false);
                DoComparisonTest(hfoo, arr, true);
                DoComparisonTest(arr, hfoobar, false);
                DoComparisonTest(arr, hfoo, true);

                DoComparisonTest(rfoobar, arr, false);
                DoComparisonTest(rfoo, arr, true);
                DoComparisonTest(arr, rfoobar, false);
                DoComparisonTest(arr, rfoo, true);
            }

            TEST_METHOD(WStringEqualityTest)
            {
                std::wstring str(L"foo\0bar", 7);

                DoComparisonTest(hfoobar, str, true);
                DoComparisonTest(hfoo, str, false);
                DoComparisonTest(str, hfoobar, true);
                DoComparisonTest(str, hfoo, false);

                DoComparisonTest(rfoobar, str, true);
                DoComparisonTest(rfoo, str, false);
                DoComparisonTest(str, rfoobar, true);
                DoComparisonTest(str, rfoo, false);
            }
        };

        TEST_CLASS(HStringInequalityOperatorTests)
        {
            template <typename LhsTy, typename RhsTy>
            void DoComparisonTest(LhsTy &&lhs, RhsTy &&rhs, _In_ bool expected)
            {
                Assert::AreEqual(expected, lhs != rhs);

                if (!std::is_array<std::remove_reference_t<LhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<LhsTy>>::value)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoComparisonTest(other, rhs, expected);
                }

                if (!std::is_array<std::remove_reference_t<RhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<RhsTy>>::value)
                {
                    const std::remove_reference_t<RhsTy> &other = rhs;
                    DoComparisonTest(lhs, other, expected);
                }
            }

            TEST_METHOD(HStringInequalityTest)
            {
                DoComparisonTest(hfoo, hfoo, false);
                DoComparisonTest(hfoo, hbar, true);
                DoComparisonTest(hfoo, hfoobar, true);
                DoComparisonTest(hfoobar, hfoo, true);
                DoComparisonTest(hfoobar, hfoobar, false);
            }

            TEST_METHOD(HStringReferenceInequalityTest)
            {
                DoComparisonTest(rfoo, rfoo, false);
                DoComparisonTest(rfoo, rbar, true);
                DoComparisonTest(rfoo, rfoobar, true);
                DoComparisonTest(rfoobar, rfoo, true);
                DoComparisonTest(rfoobar, rfoobar, false);
            }

            TEST_METHOD(CrossHStringInequalityTest)
            {
                DoComparisonTest(hfoo, rfoo, false);
                DoComparisonTest(hfoo, rbar, true);
                DoComparisonTest(hfoo, rfoobar, true);
                DoComparisonTest(hfoobar, rfoo, true);
                DoComparisonTest(hfoobar, rfoobar, false);

                DoComparisonTest(rfoo, hfoo, false);
                DoComparisonTest(rfoo, hbar, true);
                DoComparisonTest(rfoo, hfoobar, true);
                DoComparisonTest(rfoobar, hfoo, true);
                DoComparisonTest(rfoobar, hfoobar, false);
            }

            TEST_METHOD(HSTRINGInequalityTest)
            {
                HString hstr;
                Assert::IsTrue(SUCCEEDED(hstr.Set(L"foo\0bar")));
                auto str = hstr.Get();

                DoComparisonTest(hfoobar, str, false);
                DoComparisonTest(hfoo, str, true);
                DoComparisonTest(str, hfoobar, false);
                DoComparisonTest(str, hfoo, true);

                DoComparisonTest(rfoobar, str, false);
                DoComparisonTest(rfoo, str, true);
                DoComparisonTest(str, rfoobar, false);
                DoComparisonTest(str, rfoo, true);
            }

            TEST_METHOD(CStringInequalityTest)
            {
                const wchar_t *cstr = L"foo\0bar";

                DoComparisonTest(hfoobar, cstr, true);
                DoComparisonTest(hfoo, cstr, false);
                DoComparisonTest(cstr, hfoobar, true);
                DoComparisonTest(cstr, hfoo, false);

                DoComparisonTest(rfoobar, cstr, true);
                DoComparisonTest(rfoo, cstr, false);
                DoComparisonTest(cstr, rfoobar, true);
                DoComparisonTest(cstr, rfoo, false);
            }

            TEST_METHOD(ConstArrayInequalityTest)
            {
                const wchar_t arr[] = L"foo\0bar";

                DoComparisonTest(hfoobar, arr, false);
                DoComparisonTest(hfoo, arr, true);
                DoComparisonTest(arr, hfoobar, false);
                DoComparisonTest(arr, hfoo, true);

                DoComparisonTest(rfoobar, arr, false);
                DoComparisonTest(rfoo, arr, true);
                DoComparisonTest(arr, rfoobar, false);
                DoComparisonTest(arr, rfoo, true);
            }

            TEST_METHOD(NonConstArrayInequalityTest)
            {
                wchar_t arr[] = L"foo\0bar";

                DoComparisonTest(hfoobar, arr, true);
                DoComparisonTest(hfoo, arr, false);
                DoComparisonTest(arr, hfoobar, true);
                DoComparisonTest(arr, hfoo, false);

                DoComparisonTest(rfoobar, arr, true);
                DoComparisonTest(rfoo, arr, false);
                DoComparisonTest(arr, rfoobar, true);
                DoComparisonTest(arr, rfoo, false);
            }

            TEST_METHOD(WStringInequalityTest)
            {
                std::wstring str(L"foo\0bar", 7);

                DoComparisonTest(hfoobar, str, false);
                DoComparisonTest(hfoo, str, true);
                DoComparisonTest(str, hfoobar, false);
                DoComparisonTest(str, hfoo, true);

                DoComparisonTest(rfoobar, str, false);
                DoComparisonTest(rfoo, str, true);
                DoComparisonTest(str, rfoobar, false);
                DoComparisonTest(str, rfoo, true);
            }
        };

        TEST_CLASS(HStringLessThanOperatorTests)
        {
            template <typename LhsTy, typename RhsTy>
            void DoComparisonTest(LhsTy &&lhs, RhsTy &&rhs, _In_ bool expected)
            {
                Assert::AreEqual(expected, lhs < rhs);

                if (!std::is_array<std::remove_reference_t<LhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<LhsTy>>::value)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoComparisonTest(other, rhs, expected);
                }

                if (!std::is_array<std::remove_reference_t<RhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<RhsTy>>::value)
                {
                    const std::remove_reference_t<RhsTy> &other = rhs;
                    DoComparisonTest(lhs, other, expected);
                }
            }

            TEST_METHOD(HStringLessThanTest)
            {
                DoComparisonTest(hfoo, hfoo, false);
                DoComparisonTest(hfoo, hbar, false);
                DoComparisonTest(hbar, hfoo, true);
                DoComparisonTest(hfoo, hfoobar, true);
                DoComparisonTest(hfoobar, hfoo, false);
                DoComparisonTest(hfoobar, hfoobar, false);
            }

            TEST_METHOD(HStringReferenceLessThanTest)
            {
                DoComparisonTest(rfoo, rfoo, false);
                DoComparisonTest(rfoo, rbar, false);
                DoComparisonTest(rbar, rfoo, true);
                DoComparisonTest(rfoo, rfoobar, true);
                DoComparisonTest(rfoobar, rfoo, false);
                DoComparisonTest(rfoobar, rfoobar, false);
            }

            TEST_METHOD(CrossHStringLessThanTest)
            {
                DoComparisonTest(hfoo, rfoo, false);
                DoComparisonTest(hfoo, rbar, false);
                DoComparisonTest(hbar, rfoo, true);
                DoComparisonTest(hfoo, rfoobar, true);
                DoComparisonTest(hfoobar, rfoo, false);
                DoComparisonTest(hfoobar, rfoobar, false);

                DoComparisonTest(rfoo, hfoo, false);
                DoComparisonTest(rfoo, hbar, false);
                DoComparisonTest(rbar, hfoo, true);
                DoComparisonTest(rfoo, hfoobar, true);
                DoComparisonTest(rfoobar, hfoo, false);
                DoComparisonTest(rfoobar, hfoobar, false);
            }

            TEST_METHOD(HSTRINGLessThanTest)
            {
                HString hstr;
                Assert::IsTrue(SUCCEEDED(hstr.Set(L"foo\0bar")));
                auto str = hstr.Get();

                DoComparisonTest(hfoobar, str, false);
                DoComparisonTest(hfoo, str, true);
                DoComparisonTest(str, hfoobar, false);
                DoComparisonTest(str, hfoo, false);

                DoComparisonTest(rfoobar, str, false);
                DoComparisonTest(rfoo, str, true);
                DoComparisonTest(str, rfoobar, false);
                DoComparisonTest(str, rfoo, false);
            }

            TEST_METHOD(CStringLessThanTest)
            {
                const wchar_t *cstr = L"foo\0bar";

                DoComparisonTest(hfoobar, cstr, false);
                DoComparisonTest(hfoo, cstr, false);
                DoComparisonTest(cstr, hfoobar, true);
                DoComparisonTest(cstr, hfoo, false);

                DoComparisonTest(rfoobar, cstr, false);
                DoComparisonTest(rfoo, cstr, false);
                DoComparisonTest(cstr, rfoobar, true);
                DoComparisonTest(cstr, rfoo, false);
            }

            TEST_METHOD(ConstArrayLessThanTest)
            {
                const wchar_t arr[] = L"foo\0bar";

                DoComparisonTest(hfoobar, arr, false);
                DoComparisonTest(hfoo, arr, true);
                DoComparisonTest(arr, hfoobar, false);
                DoComparisonTest(arr, hfoo, false);

                DoComparisonTest(rfoobar, arr, false);
                DoComparisonTest(rfoo, arr, true);
                DoComparisonTest(arr, rfoobar, false);
                DoComparisonTest(arr, rfoo, false);
            }

            TEST_METHOD(NonConstArrayLessThanTest)
            {
                wchar_t arr[] = L"foo\0bar";

                DoComparisonTest(hfoobar, arr, false);
                DoComparisonTest(hfoo, arr, false);
                DoComparisonTest(arr, hfoobar, true);
                DoComparisonTest(arr, hfoo, false);

                DoComparisonTest(rfoobar, arr, false);
                DoComparisonTest(rfoo, arr, false);
                DoComparisonTest(arr, rfoobar, true);
                DoComparisonTest(arr, rfoo, false);
            }

            TEST_METHOD(WStringLessThanTest)
            {
                std::wstring str(L"foo\0bar", 7);

                DoComparisonTest(hfoobar, str, false);
                DoComparisonTest(hfoo, str, true);
                DoComparisonTest(str, hfoobar, false);
                DoComparisonTest(str, hfoo, false);

                DoComparisonTest(rfoobar, str, false);
                DoComparisonTest(rfoo, str, true);
                DoComparisonTest(str, rfoobar, false);
                DoComparisonTest(str, rfoo, false);
            }
        };

        TEST_CLASS(HStringLessThanOrEqualsOperatorTests)
        {
            template <typename LhsTy, typename RhsTy>
            void DoComparisonTest(LhsTy &&lhs, RhsTy &&rhs, _In_ bool expected)
            {
                Assert::AreEqual(expected, lhs <= rhs);

                if (!std::is_array<std::remove_reference_t<LhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<LhsTy>>::value)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoComparisonTest(other, rhs, expected);
                }

                if (!std::is_array<std::remove_reference_t<RhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<RhsTy>>::value)
                {
                    const std::remove_reference_t<RhsTy> &other = rhs;
                    DoComparisonTest(lhs, other, expected);
                }
            }

            TEST_METHOD(HStringLessThanOrEqualsTest)
            {
                DoComparisonTest(hfoo, hfoo, true);
                DoComparisonTest(hfoo, hbar, false);
                DoComparisonTest(hbar, hfoo, true);
                DoComparisonTest(hfoo, hfoobar, true);
                DoComparisonTest(hfoobar, hfoo, false);
                DoComparisonTest(hfoobar, hfoobar, true);
            }

            TEST_METHOD(HStringReferenceLessThanOrEqualsTest)
            {
                DoComparisonTest(rfoo, rfoo, true);
                DoComparisonTest(rfoo, rbar, false);
                DoComparisonTest(rbar, rfoo, true);
                DoComparisonTest(rfoo, rfoobar, true);
                DoComparisonTest(rfoobar, rfoo, false);
                DoComparisonTest(rfoobar, rfoobar, true);
            }

            TEST_METHOD(CrossHStringLessThanOrEqualsTest)
            {
                DoComparisonTest(hfoo, rfoo, true);
                DoComparisonTest(hfoo, rbar, false);
                DoComparisonTest(hbar, rfoo, true);
                DoComparisonTest(hfoo, rfoobar, true);
                DoComparisonTest(hfoobar, rfoo, false);
                DoComparisonTest(hfoobar, rfoobar, true);

                DoComparisonTest(rfoo, hfoo, true);
                DoComparisonTest(rfoo, hbar, false);
                DoComparisonTest(rbar, hfoo, true);
                DoComparisonTest(rfoo, hfoobar, true);
                DoComparisonTest(rfoobar, hfoo, false);
                DoComparisonTest(rfoobar, hfoobar, true);
            }

            TEST_METHOD(HSTRINGLessThanOrEqualsTest)
            {
                HString hstr;
                Assert::IsTrue(SUCCEEDED(hstr.Set(L"foo\0bar")));
                auto str = hstr.Get();

                DoComparisonTest(hfoobar, str, true);
                DoComparisonTest(hfoo, str, true);
                DoComparisonTest(str, hfoobar, true);
                DoComparisonTest(str, hfoo, false);

                DoComparisonTest(rfoobar, str, true);
                DoComparisonTest(rfoo, str, true);
                DoComparisonTest(str, rfoobar, true);
                DoComparisonTest(str, rfoo, false);
            }

            TEST_METHOD(CStringLessThanOrEqualsTest)
            {
                const wchar_t *cstr = L"foo\0bar";

                DoComparisonTest(hfoobar, cstr, false);
                DoComparisonTest(hfoo, cstr, true);
                DoComparisonTest(cstr, hfoobar, true);
                DoComparisonTest(cstr, hfoo, true);

                DoComparisonTest(rfoobar, cstr, false);
                DoComparisonTest(rfoo, cstr, true);
                DoComparisonTest(cstr, rfoobar, true);
                DoComparisonTest(cstr, rfoo, true);
            }

            TEST_METHOD(ConstArrayLessThanOrEqualsTest)
            {
                const wchar_t arr[] = L"foo\0bar";

                DoComparisonTest(hfoobar, arr, true);
                DoComparisonTest(hfoo, arr, true);
                DoComparisonTest(arr, hfoobar, true);
                DoComparisonTest(arr, hfoo, false);

                DoComparisonTest(rfoobar, arr, true);
                DoComparisonTest(rfoo, arr, true);
                DoComparisonTest(arr, rfoobar, true);
                DoComparisonTest(arr, rfoo, false);
            }

            TEST_METHOD(NonConstArrayLessThanOrEqualsTest)
            {
                wchar_t arr[] = L"foo\0bar";

                DoComparisonTest(hfoobar, arr, false);
                DoComparisonTest(hfoo, arr, true);
                DoComparisonTest(arr, hfoobar, true);
                DoComparisonTest(arr, hfoo, true);

                DoComparisonTest(rfoobar, arr, false);
                DoComparisonTest(rfoo, arr, true);
                DoComparisonTest(arr, rfoobar, true);
                DoComparisonTest(arr, rfoo, true);
            }

            TEST_METHOD(WStringLessThanOrEqualsTest)
            {
                std::wstring str(L"foo\0bar", 7);

                DoComparisonTest(hfoobar, str, true);
                DoComparisonTest(hfoo, str, true);
                DoComparisonTest(str, hfoobar, true);
                DoComparisonTest(str, hfoo, false);

                DoComparisonTest(rfoobar, str, true);
                DoComparisonTest(rfoo, str, true);
                DoComparisonTest(str, rfoobar, true);
                DoComparisonTest(str, rfoo, false);
            }
        };

        TEST_CLASS(HStringGreaterThanOperatorTests)
        {
            template <typename LhsTy, typename RhsTy>
            void DoComparisonTest(LhsTy &&lhs, RhsTy &&rhs, _In_ bool expected)
            {
                Assert::AreEqual(expected, lhs > rhs);

                if (!std::is_array<std::remove_reference_t<LhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<LhsTy>>::value)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoComparisonTest(other, rhs, expected);
                }

                if (!std::is_array<std::remove_reference_t<RhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<RhsTy>>::value)
                {
                    const std::remove_reference_t<RhsTy> &other = rhs;
                    DoComparisonTest(lhs, other, expected);
                }
            }

            TEST_METHOD(HStringGreaterThanTest)
            {
                DoComparisonTest(hfoo, hfoo, false);
                DoComparisonTest(hfoo, hbar, true);
                DoComparisonTest(hbar, hfoo, false);
                DoComparisonTest(hfoo, hfoobar, false);
                DoComparisonTest(hfoobar, hfoo, true);
                DoComparisonTest(hfoobar, hfoobar, false);
            }

            TEST_METHOD(HStringReferenceGreaterThanTest)
            {
                DoComparisonTest(rfoo, rfoo, false);
                DoComparisonTest(rfoo, rbar, true);
                DoComparisonTest(rbar, rfoo, false);
                DoComparisonTest(rfoo, rfoobar, false);
                DoComparisonTest(rfoobar, rfoo, true);
                DoComparisonTest(rfoobar, rfoobar, false);
            }

            TEST_METHOD(CrossHStringGreaterThanTest)
            {
                DoComparisonTest(hfoo, rfoo, false);
                DoComparisonTest(hfoo, rbar, true);
                DoComparisonTest(hbar, rfoo, false);
                DoComparisonTest(hfoo, rfoobar, false);
                DoComparisonTest(hfoobar, rfoo, true);
                DoComparisonTest(hfoobar, rfoobar, false);

                DoComparisonTest(rfoo, hfoo, false);
                DoComparisonTest(rfoo, hbar, true);
                DoComparisonTest(rbar, hfoo, false);
                DoComparisonTest(rfoo, hfoobar, false);
                DoComparisonTest(rfoobar, hfoo, true);
                DoComparisonTest(rfoobar, hfoobar, false);
            }

            TEST_METHOD(HSTRINGGreaterThanTest)
            {
                HString hstr;
                Assert::IsTrue(SUCCEEDED(hstr.Set(L"foo\0bar")));
                auto str = hstr.Get();

                DoComparisonTest(hfoobar, str, false);
                DoComparisonTest(hfoo, str, false);
                DoComparisonTest(str, hfoobar, false);
                DoComparisonTest(str, hfoo, true);

                DoComparisonTest(rfoobar, str, false);
                DoComparisonTest(rfoo, str, false);
                DoComparisonTest(str, rfoobar, false);
                DoComparisonTest(str, rfoo, true);
            }

            TEST_METHOD(CStringGreaterThanTest)
            {
                const wchar_t *cstr = L"foo\0bar";

                DoComparisonTest(hfoobar, cstr, true);
                DoComparisonTest(hfoo, cstr, false);
                DoComparisonTest(cstr, hfoobar, false);
                DoComparisonTest(cstr, hfoo, false);

                DoComparisonTest(rfoobar, cstr, true);
                DoComparisonTest(rfoo, cstr, false);
                DoComparisonTest(cstr, rfoobar, false);
                DoComparisonTest(cstr, rfoo, false);
            }

            TEST_METHOD(ConstArrayGreaterThanTest)
            {
                const wchar_t arr[] = L"foo\0bar";

                DoComparisonTest(hfoobar, arr, false);
                DoComparisonTest(hfoo, arr, false);
                DoComparisonTest(arr, hfoobar, false);
                DoComparisonTest(arr, hfoo, true);

                DoComparisonTest(rfoobar, arr, false);
                DoComparisonTest(rfoo, arr, false);
                DoComparisonTest(arr, rfoobar, false);
                DoComparisonTest(arr, rfoo, true);
            }

            TEST_METHOD(NonConstArrayGreaterThanTest)
            {
                wchar_t arr[] = L"foo\0bar";

                DoComparisonTest(hfoobar, arr, true);
                DoComparisonTest(hfoo, arr, false);
                DoComparisonTest(arr, hfoobar, false);
                DoComparisonTest(arr, hfoo, false);

                DoComparisonTest(rfoobar, arr, true);
                DoComparisonTest(rfoo, arr, false);
                DoComparisonTest(arr, rfoobar, false);
                DoComparisonTest(arr, rfoo, false);
            }

            TEST_METHOD(WStringGreaterThanTest)
            {
                std::wstring str(L"foo\0bar", 7);

                DoComparisonTest(hfoobar, str, false);
                DoComparisonTest(hfoo, str, false);
                DoComparisonTest(str, hfoobar, false);
                DoComparisonTest(str, hfoo, true);

                DoComparisonTest(rfoobar, str, false);
                DoComparisonTest(rfoo, str, false);
                DoComparisonTest(str, rfoobar, false);
                DoComparisonTest(str, rfoo, true);
            }
        };

        TEST_CLASS(HStringGreaterThanOrEqualsOperatorTests)
        {
            template <typename LhsTy, typename RhsTy>
            void DoComparisonTest(LhsTy &&lhs, RhsTy &&rhs, _In_ bool expected)
            {
                Assert::AreEqual(expected, lhs >= rhs);

                if (!std::is_array<std::remove_reference_t<LhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<LhsTy>>::value)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoComparisonTest(other, rhs, expected);
                }

                if (!std::is_array<std::remove_reference_t<RhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<RhsTy>>::value)
                {
                    const std::remove_reference_t<RhsTy> &other = rhs;
                    DoComparisonTest(lhs, other, expected);
                }
            }

            TEST_METHOD(HStringGreaterThanOrEqualsTest)
            {
                DoComparisonTest(hfoo, hfoo, true);
                DoComparisonTest(hfoo, hbar, true);
                DoComparisonTest(hbar, hfoo, false);
                DoComparisonTest(hfoo, hfoobar, false);
                DoComparisonTest(hfoobar, hfoo, true);
                DoComparisonTest(hfoobar, hfoobar, true);
            }

            TEST_METHOD(HStringReferenceGreaterThanOrEqualsTest)
            {
                DoComparisonTest(rfoo, rfoo, true);
                DoComparisonTest(rfoo, rbar, true);
                DoComparisonTest(rbar, rfoo, false);
                DoComparisonTest(rfoo, rfoobar, false);
                DoComparisonTest(rfoobar, rfoo, true);
                DoComparisonTest(rfoobar, rfoobar, true);
            }

            TEST_METHOD(CrossHStringGreaterThanOrEqualsTest)
            {
                DoComparisonTest(hfoo, rfoo, true);
                DoComparisonTest(hfoo, rbar, true);
                DoComparisonTest(hbar, rfoo, false);
                DoComparisonTest(hfoo, rfoobar, false);
                DoComparisonTest(hfoobar, rfoo, true);
                DoComparisonTest(hfoobar, rfoobar, true);

                DoComparisonTest(rfoo, hfoo, true);
                DoComparisonTest(rfoo, hbar, true);
                DoComparisonTest(rbar, hfoo, false);
                DoComparisonTest(rfoo, hfoobar, false);
                DoComparisonTest(rfoobar, hfoo, true);
                DoComparisonTest(rfoobar, hfoobar, true);
            }

            TEST_METHOD(HSTRINGGreaterThanOrEqualsTest)
            {
                HString hstr;
                Assert::IsTrue(SUCCEEDED(hstr.Set(L"foo\0bar")));
                auto str = hstr.Get();

                DoComparisonTest(hfoobar, str, true);
                DoComparisonTest(hfoo, str, false);
                DoComparisonTest(str, hfoobar, true);
                DoComparisonTest(str, hfoo, true);

                DoComparisonTest(rfoobar, str, true);
                DoComparisonTest(rfoo, str, false);
                DoComparisonTest(str, rfoobar, true);
                DoComparisonTest(str, rfoo, true);
            }

            TEST_METHOD(CStringGreaterThanOrEqualsTest)
            {
                const wchar_t *cstr = L"foo\0bar";

                DoComparisonTest(hfoobar, cstr, true);
                DoComparisonTest(hfoo, cstr, true);
                DoComparisonTest(cstr, hfoobar, false);
                DoComparisonTest(cstr, hfoo, true);

                DoComparisonTest(rfoobar, cstr, true);
                DoComparisonTest(rfoo, cstr, true);
                DoComparisonTest(cstr, rfoobar, false);
                DoComparisonTest(cstr, rfoo, true);
            }

            TEST_METHOD(ConstArrayGreaterThanOrEqualsTest)
            {
                const wchar_t arr[] = L"foo\0bar";

                DoComparisonTest(hfoobar, arr, true);
                DoComparisonTest(hfoo, arr, false);
                DoComparisonTest(arr, hfoobar, true);
                DoComparisonTest(arr, hfoo, true);

                DoComparisonTest(rfoobar, arr, true);
                DoComparisonTest(rfoo, arr, false);
                DoComparisonTest(arr, rfoobar, true);
                DoComparisonTest(arr, rfoo, true);
            }

            TEST_METHOD(NonConstArrayGreaterThanOrEqualsTest)
            {
                wchar_t arr[] = L"foo\0bar";

                DoComparisonTest(hfoobar, arr, true);
                DoComparisonTest(hfoo, arr, true);
                DoComparisonTest(arr, hfoobar, false);
                DoComparisonTest(arr, hfoo, true);

                DoComparisonTest(rfoobar, arr, true);
                DoComparisonTest(rfoo, arr, true);
                DoComparisonTest(arr, rfoobar, false);
                DoComparisonTest(arr, rfoo, true);
            }

            TEST_METHOD(WStringGreaterThanOrEqualsTest)
            {
                std::wstring str(L"foo\0bar", 7);

                DoComparisonTest(hfoobar, str, true);
                DoComparisonTest(hfoo, str, false);
                DoComparisonTest(str, hfoobar, true);
                DoComparisonTest(str, hfoo, true);

                DoComparisonTest(rfoobar, str, true);
                DoComparisonTest(rfoo, str, false);
                DoComparisonTest(str, rfoobar, true);
                DoComparisonTest(str, rfoo, true);
            }
        };

        // Expected results for append tests
        static const wchar_t fooBarFooBar[] = L"foo\0barfoo\0bar";
        static const wchar_t fooBarFoo[] = L"foo\0barfoo";
        static const wchar_t fooFooBar[] = L"foofoo\0bar";

        // Other values for append tests
        static const wchar_t fooBarConstArray[] = L"foo\0bar";
        static wchar_t fooBarArray[] = L"foo\0bar";
        static const wchar_t *fooBarString = L"foo\0bar";
        static std::wstring wfoobar(fooBarConstArray, 7);

        TEST_CLASS(HStringAppendOperatorTests)
        {
            template <typename LhsTy, typename RhsTy, typename ResultTy>
            void DoAppendTest(LhsTy &&lhs, RhsTy &&rhs, _In_ ResultTy &&expected)
            {
                auto result = lhs + rhs;
                Assert::IsTrue(std::is_same<decltype(result), hstring>::value);
                Assert::IsTrue(result == expected);

                if (!std::is_array<std::remove_reference_t<LhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<LhsTy>>::value)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoAppendTest(other, rhs, expected);
                }

                if (!std::is_array<std::remove_reference_t<RhsTy>>::value &&
                    !std::is_const<std::remove_reference_t<RhsTy>>::value)
                {
                    const std::remove_reference_t<RhsTy> &other = rhs;
                    DoAppendTest(lhs, other, expected);
                }
            }

            TEST_METHOD(AppendToHStringTest)
            {
                HString hstr;
                Assert::IsTrue(SUCCEEDED(hstr.Set(fooBarConstArray)));

                DoAppendTest(hfoobar, hfoobar, fooBarFooBar);
                DoAppendTest(hfoobar, rfoobar, fooBarFooBar);
                DoAppendTest(hfoobar, fooBarConstArray, fooBarFooBar);
                DoAppendTest(hfoobar, fooBarArray, fooBarFoo);
                DoAppendTest(hfoobar, fooBarString, fooBarFoo);
                DoAppendTest(hfoobar, wfoobar, fooBarFooBar);
                DoAppendTest(hfoobar, hstr.Get(), fooBarFooBar);

                DoAppendTest(rfoobar, hfoobar, fooBarFooBar);
                DoAppendTest(fooBarConstArray, hfoobar, fooBarFooBar);
                DoAppendTest(fooBarArray, hfoobar, fooFooBar);
                DoAppendTest(fooBarString, hfoobar, fooFooBar);
                DoAppendTest(wfoobar, hfoobar, fooBarFooBar);
                DoAppendTest(hstr.Get(), hfoobar, fooBarFooBar);
            }

            TEST_METHOD(AppendToHStringReferenceTest)
            {
                HString hstr;
                Assert::IsTrue(SUCCEEDED(hstr.Set(fooBarConstArray)));

                DoAppendTest(rfoobar, rfoobar, fooBarFooBar);
                DoAppendTest(rfoobar, fooBarConstArray, fooBarFooBar);
                DoAppendTest(rfoobar, fooBarArray, fooBarFoo);
                DoAppendTest(rfoobar, fooBarString, fooBarFoo);
                DoAppendTest(rfoobar, wfoobar, fooBarFooBar);
                DoAppendTest(rfoobar, hstr.Get(), fooBarFooBar);

                DoAppendTest(fooBarConstArray, rfoobar, fooBarFooBar);
                DoAppendTest(fooBarArray, rfoobar, fooFooBar);
                DoAppendTest(fooBarString, rfoobar, fooFooBar);
                DoAppendTest(wfoobar, rfoobar, fooBarFooBar);
                DoAppendTest(hstr.Get(), rfoobar, fooBarFooBar);
            }
        };
    }
}

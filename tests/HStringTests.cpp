/*
 * Duncan Horn
 *
 * HStringTests.cpp
 *
 * Tests for the dhorn::experimental::hstring.h functions
 */
#include "stdafx.h"

#include <wrl/wrappers/corewrappers.h>

#include <dhorn/experimental/hstring.h>

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
            void AssertEquals(const dhorn::experimental::hstring_reference &lhs, StringT&& rhs)
            {
                int result;
                ASSERT_TRUE(SUCCEEDED(::WindowsCompareStringOrdinal(
                    lhs.get(),
                    HStringReference(rhs).Get(),
                    &result)));
                ASSERT_EQ(0, result);
            }

#pragma region constructor tests

            template <typename Ty, std::size_t Size>
            void DoConstructorTest(Ty&& value, const wchar_t (&expected)[Size])
            {
                dhorn::experimental::hstring_reference str(std::forward<Ty>(value));

                ASSERT_EQ(static_cast<UINT32>(Size - 1), ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(DefaultConstructorTest)
            {
                dhorn::experimental::hstring_reference str;
                ASSERT_FALSE(static_cast<bool>(str));
                ASSERT_TRUE(str.get() == nullptr);
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

                dhorn::experimental::hstring_reference str(cstr, 7);
                ASSERT_EQ(7u, ::WindowsGetStringLen(str.get()));

                int result;
                ASSERT_TRUE(SUCCEEDED(::WindowsCompareStringOrdinal(
                    HStringReference(cstr, 7).Get(),
                    str.get(),
                    &result)));
                ASSERT_EQ(0, result);
            }

            TEST_METHOD(CopyConstructorTest)
            {
                dhorn::experimental::hstring_reference str1(L"foo\0bar");
                dhorn::experimental::hstring_reference str2(str1);

                ASSERT_TRUE(str2.get() != nullptr);

                int result;
                ASSERT_TRUE(SUCCEEDED(::WindowsCompareStringOrdinal(str1.get(), str2.get(), &result)));
                ASSERT_EQ(0, result);
            }

#pragma endregion

#pragma region assignment operator tests

            template <typename Ty, std::size_t Size>
            void DoAssignmentOperatorTest(Ty&& value, const wchar_t (&expected)[Size])
            {
                dhorn::experimental::hstring_reference str(L"initvalue");
                str = std::forward<Ty>(value);

                ASSERT_EQ(static_cast<UINT32>(Size - 1), ::WindowsGetStringLen(str.get()));
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
                dhorn::experimental::hstring_reference str1(L"foo\0bar");
                DoAssignmentOperatorTest(str1, L"foo\0bar");

                dhorn::experimental::hstring_reference str;
                const wchar_t strArray[] = L"foo\0bar";
                {
                    dhorn::experimental::hstring_reference exitingStr = strArray;
                    str = exitingStr;
                }

                ASSERT_EQ(7u, ::WindowsGetStringLen(str.get()));
                AssertEquals(str, strArray);
            }

#pragma endregion

#pragma region assign tests

            template <typename Ty, std::size_t Size>
            void DoAssignTest(Ty&& value, const wchar_t(&expected)[Size])
            {
                dhorn::experimental::hstring_reference str(L"initvalue");
                str.assign(std::forward<Ty>(value));

                ASSERT_EQ(static_cast<UINT32>(Size - 1), ::WindowsGetStringLen(str.get()));
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
                dhorn::experimental::hstring_reference str1(L"foo\0bar");
                DoAssignTest(str1, L"foo\0bar");
            }

#pragma endregion

#pragma region function tests

            TEST_METHOD(OperatorBoolTest)
            {
                dhorn::experimental::hstring_reference str;
                ASSERT_FALSE(str);

                str = L"";
                ASSERT_FALSE(str);

                str = L"foo";
                ASSERT_TRUE(str);

                str.reset();
                ASSERT_FALSE(str);
            }

            TEST_METHOD(CStrTest)
            {
                dhorn::experimental::hstring_reference str = L"foo";
                ASSERT_EQ(0, wcscmp(str.c_str(), L"foo"));
            }

            TEST_METHOD(ClearTest)
            {
                dhorn::experimental::hstring_reference str = L"foo";
                str.clear();
                ASSERT_FALSE(str);
                ASSERT_TRUE(str.get() == nullptr);
            }

            TEST_METHOD(CopyToTest)
            {
                dhorn::experimental::hstring_reference str = L"foo\0bar";
                HString hstr;
                str.copy_to(hstr.GetAddressOf());

                int result;
                ASSERT_TRUE(SUCCEEDED(::WindowsCompareStringOrdinal(str.get(), hstr.Get(), &result)));
                ASSERT_EQ(0, result);
            }

            TEST_METHOD(DataTest)
            {
                dhorn::experimental::hstring_reference str = L"foo";
                ASSERT_EQ(0, wcscmp(str.data(), L"foo"));
            }

            TEST_METHOD(EmptyTest)
            {
                dhorn::experimental::hstring_reference str;
                ASSERT_TRUE(str.empty());

                str = L"foo";
                ASSERT_FALSE(str.empty());

                str.clear();
                ASSERT_TRUE(str.empty());

                str = L"";
                ASSERT_TRUE(str.empty());
            }

            TEST_METHOD(LengthAndSizeTest)
            {
                dhorn::experimental::hstring_reference str;
                ASSERT_EQ(static_cast<std::size_t>(0), str.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str.size());

                str = L"";
                ASSERT_EQ(static_cast<std::size_t>(0), str.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str.size());

                str = L"foo\0bar";
                ASSERT_EQ(static_cast<std::size_t>(7), str.length());
                ASSERT_EQ(static_cast<std::size_t>(7), str.size());

                str.clear();
                ASSERT_EQ(static_cast<std::size_t>(0), str.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str.size());
            }

            TEST_METHOD(ResetTest)
            {
                dhorn::experimental::hstring_reference str = L"foo";
                str.reset();
                ASSERT_FALSE(str);
                ASSERT_TRUE(str.get() == nullptr);
            }

            TEST_METHOD(SwapTest)
            {
                dhorn::experimental::hstring_reference foo = L"foo";
                dhorn::experimental::hstring_reference bar = L"bar";

                foo.swap(bar);
                AssertEquals(foo, L"bar");
                AssertEquals(bar, L"foo");
            }

#pragma endregion
        };

        TEST_CLASS(HStringTests)
        {
            template <typename StringT>
            void AssertEquals(const dhorn::experimental::hstring &lhs, StringT&& rhs)
            {
                int result;
                ASSERT_TRUE(SUCCEEDED(::WindowsCompareStringOrdinal(
                    lhs.get(),
                    HStringReference(rhs).Get(),
                    &result)));
                ASSERT_EQ(0, result);
            }

#pragma region constructor tests

            template <typename Ty, std::size_t Size>
            void DoConstructorTest(Ty&& value, const wchar_t(&expected)[Size])
            {
                dhorn::experimental::hstring str(std::forward<Ty>(value));

                ASSERT_EQ(static_cast<UINT32>(Size - 1), ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(DefaultConstructorTest)
            {
                dhorn::experimental::hstring str;
                ASSERT_FALSE(static_cast<bool>(str));
                ASSERT_TRUE(str.get() == nullptr);
            }

            TEST_METHOD(HStringConstructorTest)
            {
                HSTRING hstr;
                ASSERT_TRUE(SUCCEEDED(::WindowsCreateString(L"foo\0bar", 7, &hstr)));

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

                dhorn::experimental::hstring str(cstr, 7);
                ASSERT_EQ(7u, ::WindowsGetStringLen(str.get()));

                int result;
                ASSERT_TRUE(SUCCEEDED(::WindowsCompareStringOrdinal(
                    HStringReference(cstr, 7).Get(),
                    str.get(),
                    &result)));
                ASSERT_EQ(0, result);
            }

            TEST_METHOD(IteratorConstructorTest)
            {
                std::wstring wstr(L"foo\0bar", 7);

                dhorn::experimental::hstring str(wstr.begin(), wstr.end());
                ASSERT_EQ(7u, ::WindowsGetStringLen(str.get()));

                int result;
                ASSERT_TRUE(SUCCEEDED(::WindowsCompareStringOrdinal(
                    HStringReference(wstr.c_str(), static_cast<unsigned int>(wstr.length())).Get(),
                    str.get(),
                    &result)));
                ASSERT_EQ(0, result);
            }

            TEST_METHOD(CopyConstructorTest)
            {
                dhorn::experimental::hstring str1(L"foo\0bar");
                dhorn::experimental::hstring str2(str1);

                ASSERT_TRUE(str2.get() != nullptr);

                int result;
                ASSERT_TRUE(SUCCEEDED(::WindowsCompareStringOrdinal(str1.get(), str2.get(), &result)));
                ASSERT_EQ(0, result);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                dhorn::experimental::hstring str1(L"foo\0bar");
                auto hstr = str1.get();

                dhorn::experimental::hstring str2(std::move(str1));
                ASSERT_TRUE(hstr == str2.get());
                ASSERT_TRUE(str1.get() == nullptr);
            }

#pragma endregion

#pragma region assignment operator tests

            template <typename Ty, std::size_t Size>
            void DoAssignmentOperatorTest(Ty&& value, const wchar_t(&expected)[Size])
            {
                dhorn::experimental::hstring str(L"initvalue");
                str = std::forward<Ty>(value);

                ASSERT_EQ(static_cast<UINT32>(Size - 1), ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(HStringAssignmentOperatorTest)
            {
                HSTRING hstr;
                ASSERT_TRUE(SUCCEEDED(::WindowsCreateString(L"foo\0bar", 7, &hstr)));

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
                dhorn::experimental::hstring str1(L"foo\0bar");
                DoAssignmentOperatorTest(str1, L"foo\0bar");

                dhorn::experimental::hstring str;
                {
                    dhorn::experimental::hstring exitingStr = L"foo\0bar";
                    str = exitingStr;
                }

                ASSERT_EQ(7u, ::WindowsGetStringLen(str.get()));
            }

            TEST_METHOD(MoveAssignmentOperatorTest)
            {
                dhorn::experimental::hstring str1(L"foo\0bar");
                auto hstr = str1.get();

                dhorn::experimental::hstring str2(L"initvalue");
                str2 = std::move(str1);

                ASSERT_TRUE(hstr == str2.get());

                dhorn::experimental::hstring str;
                {
                    dhorn::experimental::hstring exitingStr = L"foo\0bar";
                    str = std::move(exitingStr);
                }

                ASSERT_EQ(7u, ::WindowsGetStringLen(str.get()));
            }

#pragma endregion

#pragma region assign tests

            template <typename Ty, std::size_t Size>
            void DoAssignTest(Ty&& value, const wchar_t(&expected)[Size])
            {
                dhorn::experimental::hstring str(L"initvalue");
                str.assign(std::forward<Ty>(value));

                ASSERT_EQ(static_cast<UINT32>(Size - 1), ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(HStringAssignTest)
            {
                HSTRING hstr;
                ASSERT_TRUE(SUCCEEDED(::WindowsCreateString(L"foo\0bar", 7, &hstr)));

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
                dhorn::experimental::hstring str1(L"foo\0bar");
                DoAssignTest(str1, L"foo\0bar");
            }

            TEST_METHOD(MoveAssignTest)
            {
                dhorn::experimental::hstring str1(L"foo\0bar");
                auto hstr = str1.get();

                dhorn::experimental::hstring str2(L"initvalue");
                str2.assign(std::move(str1));

                ASSERT_TRUE(hstr == str2.get());
            }

#pragma endregion

#pragma region append operator tests

            template <typename Ty, std::size_t Size>
            void DoAppendOperatorTest(Ty&& value, const wchar_t(&expected)[Size])
            {
                dhorn::experimental::hstring str(L"foo");
                str += std::forward<Ty>(value);

                ASSERT_EQ(static_cast<UINT32>(Size - 1), ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(HStringAppendOperatorTest)
            {
                HSTRING hstr;
                ASSERT_TRUE(SUCCEEDED(::WindowsCreateString(L"bar\0car", 7, &hstr)));

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
                dhorn::experimental::hstring str(L"bar\0car");
                DoAppendOperatorTest(str, L"foobar\0car");
            }

            TEST_METHOD(SelfAppendOperatorTest)
            {
                dhorn::experimental::hstring str(L"foo");
                str += str;
                AssertEquals(str, L"foofoo");
            }

#pragma endregion

#pragma region append tests

            template <typename Ty, std::size_t Size>
            void DoAppendTest(Ty&& value, const wchar_t(&expected)[Size])
            {
                dhorn::experimental::hstring str(L"foo");
                str.append(std::forward<Ty>(value));

                ASSERT_EQ(static_cast<UINT32>(Size - 1), ::WindowsGetStringLen(str.get()));
                AssertEquals(str, expected);
            }

            TEST_METHOD(HStringAppendTest)
            {
                HSTRING hstr;
                ASSERT_TRUE(SUCCEEDED(::WindowsCreateString(L"bar\0car", 7, &hstr)));

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

                dhorn::experimental::hstring str(L"foo");
                str.append(cstr, 7);

                ASSERT_EQ(10u, ::WindowsGetStringLen(str.get()));
                AssertEquals(str, L"foobar\0car");
            }

            TEST_METHOD(WrlppHStringAppendTest)
            {
                dhorn::experimental::hstring str(L"bar\0car");
                DoAppendTest(str, L"foobar\0car");
            }

            TEST_METHOD(SelfAppendTest)
            {
                dhorn::experimental::hstring str(L"foo");
                str.append(str);
                AssertEquals(str, L"foofoo");
            }

#pragma endregion

#pragma region other operator tests

            TEST_METHOD(AddressOfOperatorTest)
            {
                dhorn::experimental::hstring str(L"foo");
                auto ptr = &str;

                ASSERT_TRUE(std::is_same_v<decltype(ptr), HSTRING *>);
                ASSERT_TRUE(*ptr == str.get());
            }

            TEST_METHOD(OperatorBoolTest)
            {
                dhorn::experimental::hstring str;
                ASSERT_FALSE(str);

                str = L"";
                ASSERT_FALSE(str);

                str = L"foo";
                ASSERT_TRUE(str);

                str.reset();
                ASSERT_FALSE(str);
            }

#pragma endregion

#pragma region function tests

            TEST_METHOD(AttachTest)
            {
                HSTRING hstr;
                ASSERT_TRUE(SUCCEEDED(::WindowsCreateString(L"foo", 3, &hstr)));

                dhorn::experimental::hstring str;
                str.attach(hstr);
                ASSERT_TRUE(str.get() == hstr);
            }

            TEST_METHOD(CStrTest)
            {
                dhorn::experimental::hstring str = L"foo";
                ASSERT_EQ(0, wcscmp(str.c_str(), L"foo"));
            }

            TEST_METHOD(ClearTest)
            {
                dhorn::experimental::hstring str = L"foo";
                str.clear();
                ASSERT_FALSE(str);
                ASSERT_TRUE(str.get() == nullptr);
            }

            TEST_METHOD(CopyToTest)
            {
                dhorn::experimental::hstring str = L"foo\0bar";
                HString hstr;
                str.copy_to(hstr.GetAddressOf());

                int result;
                ASSERT_TRUE(SUCCEEDED(::WindowsCompareStringOrdinal(str.get(), hstr.Get(), &result)));
                ASSERT_EQ(0, result);
            }

            TEST_METHOD(DataTest)
            {
                dhorn::experimental::hstring str = L"foo";
                ASSERT_EQ(0, wcscmp(str.data(), L"foo"));
            }

            TEST_METHOD(DetachTest)
            {
                dhorn::experimental::hstring str = L"foo";
                auto value = str.get();

                auto hstr = str.detach();
                ASSERT_TRUE(hstr == value);

                ::WindowsDeleteString(hstr);
            }

            TEST_METHOD(EmptyTest)
            {
                dhorn::experimental::hstring str;
                ASSERT_TRUE(str.empty());

                str = L"foo";
                ASSERT_FALSE(str.empty());

                str.clear();
                ASSERT_TRUE(str.empty());

                str = L"";
                ASSERT_TRUE(str.empty());

                str = L"foo";
                auto hstr = str.detach();
                ASSERT_TRUE(str.empty());

                ::WindowsDeleteString(hstr);
            }

            TEST_METHOD(LengthAndSizeTest)
            {
                dhorn::experimental::hstring str;
                ASSERT_EQ(static_cast<std::size_t>(0), str.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str.size());

                str = L"";
                ASSERT_EQ(static_cast<std::size_t>(0), str.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str.size());

                str = L"foo\0bar";
                ASSERT_EQ(static_cast<std::size_t>(7), str.length());
                ASSERT_EQ(static_cast<std::size_t>(7), str.size());

                str.clear();
                ASSERT_EQ(static_cast<std::size_t>(0), str.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str.size());

                str = L"foo";
                auto hstr = str.detach();
                ASSERT_EQ(static_cast<std::size_t>(0), str.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str.size());

                ::WindowsDeleteString(hstr);
            }

            TEST_METHOD(ReleaseTest)
            {
                dhorn::experimental::hstring str = L"foo";
                auto value = str.get();

                auto hstr = str.release();
                ASSERT_TRUE(hstr == value);

                ::WindowsDeleteString(hstr);
            }

            TEST_METHOD(ResetTest)
            {
                dhorn::experimental::hstring str = L"foo";
                str.reset();
                ASSERT_FALSE(str);
                ASSERT_TRUE(str.get() == nullptr);
            }

            TEST_METHOD(SwapTest)
            {
                dhorn::experimental::hstring foo = L"foo";
                dhorn::experimental::hstring bar = L"bar";

                foo.swap(bar);
                AssertEquals(foo, L"bar");
                AssertEquals(bar, L"foo");
            }

#pragma endregion
        };



        // Values for comparison tests
        static dhorn::experimental::hstring hfoo = L"foo";
        static dhorn::experimental::hstring hbar = L"bar";
        static dhorn::experimental::hstring hfoobar = L"foo\0bar";
        static dhorn::experimental::hstring_reference rfoo = L"foo";
        static dhorn::experimental::hstring_reference rbar = L"bar";
        static dhorn::experimental::hstring_reference rfoobar = L"foo\0bar";

        TEST_CLASS(HStringEqualityOperatorTests)
        {
            template <typename LhsTy, typename RhsTy>
            void DoComparisonTest(LhsTy &&lhs, RhsTy &&rhs, bool expected)
            {
                ASSERT_EQ(expected, lhs == rhs);

                if constexpr (!std::is_array_v<std::remove_reference_t<LhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<LhsTy>>)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoComparisonTest(other, rhs, expected);
                }

                if constexpr (!std::is_array_v<std::remove_reference_t<RhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<RhsTy>>)
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
                ASSERT_TRUE(SUCCEEDED(hstr.Set(L"foo\0bar")));
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
            void DoComparisonTest(LhsTy &&lhs, RhsTy &&rhs, bool expected)
            {
                ASSERT_EQ(expected, lhs != rhs);

                if constexpr (!std::is_array_v<std::remove_reference_t<LhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<LhsTy>>)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoComparisonTest(other, rhs, expected);
                }

                if constexpr (!std::is_array_v<std::remove_reference_t<RhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<RhsTy>>)
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
                ASSERT_TRUE(SUCCEEDED(hstr.Set(L"foo\0bar")));
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
            void DoComparisonTest(LhsTy &&lhs, RhsTy &&rhs, bool expected)
            {
                ASSERT_EQ(expected, lhs < rhs);

                if constexpr (!std::is_array_v<std::remove_reference_t<LhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<LhsTy>>)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoComparisonTest(other, rhs, expected);
                }

                if constexpr (!std::is_array_v<std::remove_reference_t<RhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<RhsTy>>)
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
                ASSERT_TRUE(SUCCEEDED(hstr.Set(L"foo\0bar")));
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
            void DoComparisonTest(LhsTy &&lhs, RhsTy &&rhs, bool expected)
            {
                ASSERT_EQ(expected, lhs <= rhs);

                if constexpr (!std::is_array_v<std::remove_reference_t<LhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<LhsTy>>)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoComparisonTest(other, rhs, expected);
                }

                if constexpr (!std::is_array_v<std::remove_reference_t<RhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<RhsTy>>)
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
                ASSERT_TRUE(SUCCEEDED(hstr.Set(L"foo\0bar")));
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
            void DoComparisonTest(LhsTy &&lhs, RhsTy &&rhs, bool expected)
            {
                ASSERT_EQ(expected, lhs > rhs);

                if constexpr (!std::is_array_v<std::remove_reference_t<LhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<LhsTy>>)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoComparisonTest(other, rhs, expected);
                }

                if constexpr (!std::is_array_v<std::remove_reference_t<RhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<RhsTy>>)
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
                ASSERT_TRUE(SUCCEEDED(hstr.Set(L"foo\0bar")));
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
            void DoComparisonTest(LhsTy &&lhs, RhsTy &&rhs, bool expected)
            {
                ASSERT_EQ(expected, lhs >= rhs);

                if constexpr (!std::is_array_v<std::remove_reference_t<LhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<LhsTy>>)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoComparisonTest(other, rhs, expected);
                }

                if constexpr (!std::is_array_v<std::remove_reference_t<RhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<RhsTy>>)
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
                ASSERT_TRUE(SUCCEEDED(hstr.Set(L"foo\0bar")));
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
            void DoAppendTest(LhsTy &&lhs, RhsTy &&rhs, ResultTy &&expected)
            {
                auto result = lhs + rhs;
                ASSERT_TRUE(std::is_same_v<decltype(result), dhorn::experimental::hstring>);
                ASSERT_TRUE(result == expected);

                if constexpr (!std::is_array_v<std::remove_reference_t<LhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<LhsTy>>)
                {
                    const std::remove_reference_t<LhsTy> &other = lhs;
                    DoAppendTest(other, rhs, expected);
                }

                if constexpr (!std::is_array_v<std::remove_reference_t<RhsTy>> &&
                    !std::is_const_v<std::remove_reference_t<RhsTy>>)
                {
                    const std::remove_reference_t<RhsTy> &other = rhs;
                    DoAppendTest(lhs, other, expected);
                }
            }

            TEST_METHOD(AppendToHStringTest)
            {
                HString hstr;
                ASSERT_TRUE(SUCCEEDED(hstr.Set(fooBarConstArray)));

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
                ASSERT_TRUE(SUCCEEDED(hstr.Set(fooBarConstArray)));

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

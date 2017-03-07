/*
 * Duncan Horn
 *
 * TypeTraitsTests.cpp
 *
 * Tests for the type_traits.h functions
 */
#include "stdafx.h"

#include <dhorn/type_traits.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        struct comp1
        {
            int value;
        };

        struct comp2
        {
            int value;
        };

        inline constexpr bool operator==(const comp1& lhs, const comp2& rhs) { return lhs.value == rhs.value; }
        inline constexpr bool operator<(const comp1& lhs, const comp2& rhs) { return lhs.value < rhs.value; }

        TEST_CLASS(TypeTraitsTests)
        {
            TEST_METHOD(EqualityComparableTest)
            {
                Assert::IsTrue(dhorn::is_comparable_v<comp1, comp2>);

                Assert::IsFalse(dhorn::is_comparable_v<comp1, comp1>);
                Assert::IsFalse(dhorn::is_comparable_v<comp2, comp2>);
                Assert::IsFalse(dhorn::is_comparable_v<comp2, comp1>);
            }

            TEST_METHOD(LessThanComparableTest)
            {
                Assert::IsTrue(dhorn::is_less_than_comparable_v<comp1, comp2>);

                Assert::IsFalse(dhorn::is_less_than_comparable_v<comp1, comp1>);
                Assert::IsFalse(dhorn::is_less_than_comparable_v<comp2, comp2>);
                Assert::IsFalse(dhorn::is_less_than_comparable_v<comp2, comp1>);
            }

            TEST_METHOD(ByteOffsetTest)
            {
                struct foo
                {
                    int32_t int32;
                    uint32_t uint32;
                    char ch;
                };

                // NOTE: The compiler can arrange these how it wants and sometimes inserts extra space for alignment
                // resons. Thus, we compare to offsetof
                Assert::AreEqual(dhorn::byte_offset(&foo::int32), offsetof(foo, int32));
                Assert::AreEqual(dhorn::byte_offset(&foo::uint32), offsetof(foo, uint32));
                Assert::AreEqual(dhorn::byte_offset(&foo::ch), offsetof(foo, ch));
            }
        };

        TEST_CLASS(AnyBaseOfTests)
        {
            struct foo {};
            struct bar {};
            struct foobar : public foo, public bar {};

            TEST_METHOD(SingleTypeTest)
            {
                Assert::IsTrue(any_base_of<foo, foo>::value);
                Assert::IsFalse(any_base_of<foo, bar>::value);
                Assert::IsFalse(any_base_of<bar, foo>::value);

                Assert::IsTrue(any_base_of<foo, foobar>::value);
                Assert::IsTrue(any_base_of<bar, foobar>::value);
                Assert::IsFalse(any_base_of<foobar, foo>::value);
                Assert::IsFalse(any_base_of<foobar, bar>::value);
            }

            TEST_METHOD(MultipleTypeTest)
            {
                Assert::IsTrue(any_base_of<foo, bar, foobar>::value);
                Assert::IsTrue(any_base_of<foo, foobar, bar>::value);
                Assert::IsTrue(any_base_of<bar, foo, foobar>::value);
                Assert::IsTrue(any_base_of<bar, foobar, foo>::value);

                Assert::IsFalse(any_base_of<foobar, foo, bar>::value);

                Assert::IsFalse(any_base_of<int, foo, bar, foobar>::value);
                Assert::IsFalse(any_base_of<void, int, float, double, foo, bar, foobar>::value);
            }
        };

        TEST_CLASS(AllBaseOfTests)
        {
            struct foo {};
            struct bar {};
            struct foobar : public foo, public bar {};

            TEST_METHOD(SingleTypeTest)
            {
                Assert::IsTrue(all_base_of<foo, foo>::value);
                Assert::IsFalse(all_base_of<foo, bar>::value);
                Assert::IsFalse(all_base_of<bar, foo>::value);

                Assert::IsTrue(all_base_of<foo, foobar>::value);
                Assert::IsTrue(all_base_of<bar, foobar>::value);
                Assert::IsFalse(all_base_of<foobar, foo>::value);
                Assert::IsFalse(all_base_of<foobar, bar>::value);
            }

            TEST_METHOD(MultipleTypeTest)
            {
                Assert::IsFalse(all_base_of<foo, bar, foobar>::value);
                Assert::IsFalse(all_base_of<foo, foobar, bar>::value);
                Assert::IsFalse(all_base_of<bar, foo, foobar>::value);
                Assert::IsFalse(all_base_of<bar, foobar, foo>::value);

                Assert::IsTrue(all_base_of<foo, foo, foobar>::value);
                Assert::IsTrue(all_base_of<foo, foobar, foo>::value);
                Assert::IsTrue(all_base_of<bar, bar, foobar>::value);
                Assert::IsTrue(all_base_of<bar, foobar, bar>::value);

                Assert::IsFalse(all_base_of<foobar, foo, bar>::value);
            }
        };

        TEST_CLASS(IsCStringTests)
        {
            template <typename Ty>
            void DoFunctionTest(const Ty &, bool expected)
            {
                Assert::AreEqual(expected, dhorn::is_c_string<Ty>::value);
            }

            TEST_METHOD(CharStringLiteralTest)
            {
                Assert::IsTrue(dhorn::is_c_string<char *>::value);
                Assert::IsTrue(dhorn::is_c_string<const char *>::value);
                Assert::IsTrue(dhorn::is_c_string<char * const>::value);
                Assert::IsTrue(dhorn::is_c_string<const char * const>::value);

                DoFunctionTest(const_cast<char *>("foo"), true);
                DoFunctionTest(const_cast<char *>(u8"foo"), true);
                DoFunctionTest(static_cast<const char *>("foo"), true);
                DoFunctionTest(static_cast<const char *>(u8"foo"), true);
            }

            TEST_METHOD(WCharStringLiteralTest)
            {
                Assert::IsTrue(dhorn::is_c_string<wchar_t *>::value);
                Assert::IsTrue(dhorn::is_c_string<const wchar_t *>::value);
                Assert::IsTrue(dhorn::is_c_string<wchar_t * const>::value);
                Assert::IsTrue(dhorn::is_c_string<const wchar_t * const>::value);

                DoFunctionTest(const_cast<wchar_t *>(L"foo"), true);
                DoFunctionTest(const_cast<wchar_t *>(L"foo"), true);
                DoFunctionTest(static_cast<const wchar_t *>(L"foo"), true);
                DoFunctionTest(static_cast<const wchar_t *>(L"foo"), true);
            }

            TEST_METHOD(Char16StringLiteralTest)
            {
                Assert::IsTrue(dhorn::is_c_string<char16_t *>::value);
                Assert::IsTrue(dhorn::is_c_string<const char16_t *>::value);
                Assert::IsTrue(dhorn::is_c_string<char16_t * const>::value);
                Assert::IsTrue(dhorn::is_c_string<const char16_t * const>::value);

                DoFunctionTest(const_cast<char16_t *>(u"foo"), true);
                DoFunctionTest(const_cast<char16_t *>(u"foo"), true);
                DoFunctionTest(static_cast<const char16_t *>(u"foo"), true);
                DoFunctionTest(static_cast<const char16_t *>(u"foo"), true);
            }

            TEST_METHOD(Char32StringLiteralTest)
            {
                Assert::IsTrue(dhorn::is_c_string<char32_t *>::value);
                Assert::IsTrue(dhorn::is_c_string<const char32_t *>::value);
                Assert::IsTrue(dhorn::is_c_string<char32_t * const>::value);
                Assert::IsTrue(dhorn::is_c_string<const char32_t * const>::value);

                DoFunctionTest(const_cast<char32_t *>(U"foo"), true);
                DoFunctionTest(const_cast<char32_t *>(U"foo"), true);
                DoFunctionTest(static_cast<const char32_t *>(U"foo"), true);
                DoFunctionTest(static_cast<const char32_t *>(U"foo"), true);
            }

            TEST_METHOD(VoidPointerTest)
            {
                Assert::IsFalse(dhorn::is_c_string<void *>::value);
                DoFunctionTest(static_cast<void *>(const_cast<char *>("foo")), false);
            }

            TEST_METHOD(CharArrayTest)
            {
                char arr[] = "foo";
                Assert::IsFalse(dhorn::is_c_string<decltype(arr)>::value);
                DoFunctionTest(arr, false);
            }

            TEST_METHOD(ConstCharArrayTest)
            {
                const char arr[] = "foo";
                Assert::IsFalse(dhorn::is_c_string<decltype(arr)>::value);
                DoFunctionTest(arr, false);
            }

            TEST_METHOD(ValueTest)
            {
                Assert::IsFalse(dhorn::is_c_string<int>::value);
                DoFunctionTest(1, false);
            }
        };
    }
}

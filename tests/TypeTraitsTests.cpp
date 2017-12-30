/*
 * Duncan Horn
 *
 * TypeTraitsTests.cpp
 *
 * Tests for the type_traits.h functions
 */
#include "stdafx.h"

#include <dhorn/type_traits.h>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
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
        template <typename Ty>
        struct dummy
        {
            template <
                typename Type = Ty,
                std::enable_if_t<std::is_same_v<first_t<Ty, Type>, int>, int> = 0,
                std::enable_if_t<std::is_same_v<first_t<Ty, Type>, Ty>, int> = 0>
            void doit() {}
        };

        TEST_METHOD(FirstTypeTest)
        {
            Assert::IsTrue(std::is_same_v<int, first_t<int>>);
            Assert::IsTrue(std::is_same_v<int, first_t<int, float>>);
            Assert::IsTrue(std::is_same_v<int, first_t<int, float, double>>);

            [[maybe_unused]] dummy<float> di;
        }

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

        TEST_METHOD(IsImplicitlyDefaultConstructibleTest)
        {
            struct test_1 {};
            struct test_2 { test_2() = default; };
            struct test_3 { test_3() {} };
            struct test_4
            {
                explicit test_4() {}

                test_4& operator=(const test_4&) = default;
                test_4& operator=(test_4&&) = default;
            };
            struct test_5 { test_5(int) {} };
            struct test_6
            {
                test_6() = default;
                test_6(test_6&&) = default;

                test_6& operator=(const test_6&) = delete;
            };

            Assert::IsTrue(is_implicitly_default_constructible_v<test_1>);
            Assert::IsTrue(is_implicitly_default_constructible_v<test_2>);
            Assert::IsTrue(is_implicitly_default_constructible_v<test_3>);
            Assert::IsFalse(is_implicitly_default_constructible_v<test_4>);
            Assert::IsFalse(is_implicitly_default_constructible_v<test_5>);
            Assert::IsTrue(is_implicitly_default_constructible_v<test_6>);

            // Some common types
            Assert::IsTrue(is_implicitly_default_constructible_v<int>);
            Assert::IsTrue(is_implicitly_default_constructible_v<double>);
            Assert::IsTrue(is_implicitly_default_constructible_v<char*>);

            // Some STL types
            Assert::IsTrue(is_implicitly_default_constructible_v<std::string>);
            Assert::IsTrue(is_implicitly_default_constructible_v<std::vector<int>>);
        }

        template <typename Ty, typename ExpectedTy = Ty>
        static void DoDecayRefTest()
        {
            Assert::IsTrue(std::is_same_v<ExpectedTy, decay_ref_t<Ty>>);
            Assert::IsTrue(std::is_same_v<ExpectedTy, decay_ref_t<const Ty>>);
            Assert::IsTrue(std::is_same_v<ExpectedTy, decay_ref_t<volatile Ty>>);
            Assert::IsTrue(std::is_same_v<ExpectedTy, decay_ref_t<const volatile Ty>>);

            Assert::IsTrue(std::is_same_v<ExpectedTy, decay_ref_t<Ty&>>);
            Assert::IsTrue(std::is_same_v<ExpectedTy, decay_ref_t<const Ty&>>);
            Assert::IsTrue(std::is_same_v<ExpectedTy, decay_ref_t<volatile Ty&>>);
            Assert::IsTrue(std::is_same_v<ExpectedTy, decay_ref_t<const volatile Ty&>>);

            Assert::IsTrue(std::is_same_v<ExpectedTy, decay_ref_t<Ty&&>>);
            Assert::IsTrue(std::is_same_v<ExpectedTy, decay_ref_t<const Ty&&>>);
            Assert::IsTrue(std::is_same_v<ExpectedTy, decay_ref_t<volatile Ty&&>>);
            Assert::IsTrue(std::is_same_v<ExpectedTy, decay_ref_t<const volatile Ty&&>>);
        }

        TEST_METHOD(DecayRefTest)
        {
            DoDecayRefTest<int>();
            DoDecayRefTest<std::string>();
            DoDecayRefTest<void*>();

            DoDecayRefTest<std::reference_wrapper<int>, int&>();
            DoDecayRefTest<std::reference_wrapper<const int>, const int&>();
            DoDecayRefTest<std::reference_wrapper<volatile int>, volatile int&>();
            DoDecayRefTest<std::reference_wrapper<const volatile int>, const volatile int&>();
            DoDecayRefTest<std::reference_wrapper<std::string>, std::string&>();
        }

        TEST_METHOD(ByteOffsetTest)
        {
            struct foo
            {
                std::int32_t int32;
                std::uint32_t uint32;
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

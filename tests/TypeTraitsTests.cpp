/*
 * Duncan Horn
 *
 * TypeTraitsTests.cpp
 *
 * Tests for the type_traits.h functions
 */

#include <dhorn/type_traits.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

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

template <typename Ty>
struct dummy
{
    template <typename Type = Ty, std::enable_if_t<std::is_same_v<dhorn::first_t<Ty, Type>, int>, int> = 0>
    void doit() {}
};

TEST(TypeTraitsTests, FirstTypeTest)
{
    ASSERT_TRUE((std::is_same_v<int, dhorn::first_t<int>>));
    ASSERT_TRUE((std::is_same_v<int, dhorn::first_t<int, float>>));
    ASSERT_TRUE((std::is_same_v<int, dhorn::first_t<int, float, double>>));

    [[maybe_unused]] dummy<float> di;
}

TEST(TypeTraitsTests, EqualityComparableTest)
{
    ASSERT_TRUE((dhorn::is_comparable_v<comp1, comp2>));

    ASSERT_FALSE((dhorn::is_comparable_v<comp1, comp1>));
    ASSERT_FALSE((dhorn::is_comparable_v<comp2, comp2>));
    ASSERT_FALSE((dhorn::is_comparable_v<comp2, comp1>));
}

TEST(TypeTraitsTests, LessThanComparableTest)
{
    ASSERT_TRUE((dhorn::is_less_than_comparable_v<comp1, comp2>));

    ASSERT_FALSE((dhorn::is_less_than_comparable_v<comp1, comp1>));
    ASSERT_FALSE((dhorn::is_less_than_comparable_v<comp2, comp2>));
    ASSERT_FALSE((dhorn::is_less_than_comparable_v<comp2, comp1>));
}

TEST(TypeTraitsTests, IsImplicitlyDefaultConstructibleTest)
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

    ASSERT_TRUE((dhorn::is_implicitly_default_constructible_v<test_1>));
    ASSERT_TRUE((dhorn::is_implicitly_default_constructible_v<test_2>));
    ASSERT_TRUE((dhorn::is_implicitly_default_constructible_v<test_3>));
    ASSERT_FALSE((dhorn::is_implicitly_default_constructible_v<test_4>));
    ASSERT_FALSE((dhorn::is_implicitly_default_constructible_v<test_5>));
    ASSERT_TRUE((dhorn::is_implicitly_default_constructible_v<test_6>));

    // Some common types
    ASSERT_TRUE(dhorn::is_implicitly_default_constructible_v<int>);
    ASSERT_TRUE(dhorn::is_implicitly_default_constructible_v<double>);
    ASSERT_TRUE(dhorn::is_implicitly_default_constructible_v<char*>);

    // Some STL types
    ASSERT_TRUE(dhorn::is_implicitly_default_constructible_v<std::string>);
    ASSERT_TRUE(dhorn::is_implicitly_default_constructible_v<std::vector<int>>);
}

template <typename Ty, typename ExpectedTy = Ty>
static void DoDecayRefTest()
{
    ASSERT_TRUE((std::is_same_v<ExpectedTy, dhorn::decay_ref_t<Ty>>));
    ASSERT_TRUE((std::is_same_v<ExpectedTy, dhorn::decay_ref_t<const Ty>>));
    ASSERT_TRUE((std::is_same_v<ExpectedTy, dhorn::decay_ref_t<volatile Ty>>));
    ASSERT_TRUE((std::is_same_v<ExpectedTy, dhorn::decay_ref_t<const volatile Ty>>));

    ASSERT_TRUE((std::is_same_v<ExpectedTy, dhorn::decay_ref_t<Ty&>>));
    ASSERT_TRUE((std::is_same_v<ExpectedTy, dhorn::decay_ref_t<const Ty&>>));
    ASSERT_TRUE((std::is_same_v<ExpectedTy, dhorn::decay_ref_t<volatile Ty&>>));
    ASSERT_TRUE((std::is_same_v<ExpectedTy, dhorn::decay_ref_t<const volatile Ty&>>));

    ASSERT_TRUE((std::is_same_v<ExpectedTy, dhorn::decay_ref_t<Ty&&>>));
    ASSERT_TRUE((std::is_same_v<ExpectedTy, dhorn::decay_ref_t<const Ty&&>>));
    ASSERT_TRUE((std::is_same_v<ExpectedTy, dhorn::decay_ref_t<volatile Ty&&>>));
    ASSERT_TRUE((std::is_same_v<ExpectedTy, dhorn::decay_ref_t<const volatile Ty&&>>));
}

TEST(TypeTraitsTests, DecayRefTest)
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

TEST(TypeTraitsTests, ByteOffsetTest)
{
    struct foo
    {
        std::int32_t int32;
        std::uint32_t uint32;
        char ch;
    };

    // NOTE: The compiler can arrange these how it wants and sometimes inserts extra space for alignment
    // resons. Thus, we compare to offsetof
    ASSERT_EQ(dhorn::byte_offset(&foo::int32), offsetof(foo, int32));
    ASSERT_EQ(dhorn::byte_offset(&foo::uint32), offsetof(foo, uint32));
    ASSERT_EQ(dhorn::byte_offset(&foo::ch), offsetof(foo, ch));
}

struct AnyBaseOfTests : testing::Test
{
    struct foo {};
    struct bar {};
    struct foobar : public foo, public bar {};
};

TEST_F(AnyBaseOfTests, SingleTypeTest)
{
    ASSERT_TRUE((dhorn::any_base_of_v<foo, foo>));
    ASSERT_FALSE((dhorn::any_base_of_v<foo, bar>));
    ASSERT_FALSE((dhorn::any_base_of_v<bar, foo>));

    ASSERT_TRUE((dhorn::any_base_of_v<foo, foobar>));
    ASSERT_TRUE((dhorn::any_base_of_v<bar, foobar>));
    ASSERT_FALSE((dhorn::any_base_of_v<foobar, foo>));
    ASSERT_FALSE((dhorn::any_base_of_v<foobar, bar>));
}

TEST_F(AnyBaseOfTests, MultipleTypeTest)
{
    ASSERT_TRUE((dhorn::any_base_of_v<foo, bar, foobar>));
    ASSERT_TRUE((dhorn::any_base_of_v<foo, foobar, bar>));
    ASSERT_TRUE((dhorn::any_base_of_v<bar, foo, foobar>));
    ASSERT_TRUE((dhorn::any_base_of_v<bar, foobar, foo>));

    ASSERT_FALSE((dhorn::any_base_of_v<foobar, foo, bar>));

    ASSERT_FALSE((dhorn::any_base_of_v<int, foo, bar, foobar>));
    ASSERT_FALSE((dhorn::any_base_of_v<void, int, float, double, foo, bar, foobar>));
}

struct AllBaseOfTests : testing::Test
{
    struct foo {};
    struct bar {};
    struct foobar : public foo, public bar {};
};

TEST_F(AllBaseOfTests, SingleTypeTest)
{
    ASSERT_TRUE((dhorn::all_base_of_v<foo, foo>));
    ASSERT_FALSE((dhorn::all_base_of_v<foo, bar>));
    ASSERT_FALSE((dhorn::all_base_of_v<bar, foo>));

    ASSERT_TRUE((dhorn::all_base_of_v<foo, foobar>));
    ASSERT_TRUE((dhorn::all_base_of_v<bar, foobar>));
    ASSERT_FALSE((dhorn::all_base_of_v<foobar, foo>));
    ASSERT_FALSE((dhorn::all_base_of_v<foobar, bar>));
}

TEST_F(AllBaseOfTests, MultipleTypeTest)
{
    ASSERT_FALSE((dhorn::all_base_of_v<foo, bar, foobar>));
    ASSERT_FALSE((dhorn::all_base_of_v<foo, foobar, bar>));
    ASSERT_FALSE((dhorn::all_base_of_v<bar, foo, foobar>));
    ASSERT_FALSE((dhorn::all_base_of_v<bar, foobar, foo>));

    ASSERT_TRUE((dhorn::all_base_of_v<foo, foo, foobar>));
    ASSERT_TRUE((dhorn::all_base_of_v<foo, foobar, foo>));
    ASSERT_TRUE((dhorn::all_base_of_v<bar, bar, foobar>));
    ASSERT_TRUE((dhorn::all_base_of_v<bar, foobar, bar>));

    ASSERT_FALSE((dhorn::all_base_of_v<foobar, foo, bar>));
}

struct IsCStringTests : testing::Test
{
    template <typename Ty>
    void DoFunctionTest(const Ty &, bool expected)
    {
        ASSERT_EQ(expected, dhorn::is_c_string_v<Ty>);
    }
};

TEST_F(IsCStringTests, CharStringLiteralTest)
{
    ASSERT_TRUE(dhorn::is_c_string_v<char *>);
    ASSERT_TRUE(dhorn::is_c_string_v<const char *>);
    ASSERT_TRUE(dhorn::is_c_string_v<char * const>);
    ASSERT_TRUE(dhorn::is_c_string_v<const char * const>);

    DoFunctionTest(const_cast<char *>("foo"), true);
    DoFunctionTest(const_cast<char *>(u8"foo"), true);
    DoFunctionTest(static_cast<const char *>("foo"), true);
    DoFunctionTest(static_cast<const char *>(u8"foo"), true);
}

TEST_F(IsCStringTests, WCharStringLiteralTest)
{
    ASSERT_TRUE(dhorn::is_c_string_v<wchar_t *>);
    ASSERT_TRUE(dhorn::is_c_string_v<const wchar_t *>);
    ASSERT_TRUE(dhorn::is_c_string_v<wchar_t * const>);
    ASSERT_TRUE(dhorn::is_c_string_v<const wchar_t * const>);

    DoFunctionTest(const_cast<wchar_t *>(L"foo"), true);
    DoFunctionTest(const_cast<wchar_t *>(L"foo"), true);
    DoFunctionTest(static_cast<const wchar_t *>(L"foo"), true);
    DoFunctionTest(static_cast<const wchar_t *>(L"foo"), true);
}

TEST_F(IsCStringTests, Char16StringLiteralTest)
{
    ASSERT_TRUE(dhorn::is_c_string_v<char16_t *>);
    ASSERT_TRUE(dhorn::is_c_string_v<const char16_t *>);
    ASSERT_TRUE(dhorn::is_c_string_v<char16_t * const>);
    ASSERT_TRUE(dhorn::is_c_string_v<const char16_t * const>);

    DoFunctionTest(const_cast<char16_t *>(u"foo"), true);
    DoFunctionTest(const_cast<char16_t *>(u"foo"), true);
    DoFunctionTest(static_cast<const char16_t *>(u"foo"), true);
    DoFunctionTest(static_cast<const char16_t *>(u"foo"), true);
}

TEST_F(IsCStringTests, Char32StringLiteralTest)
{
    ASSERT_TRUE(dhorn::is_c_string_v<char32_t *>);
    ASSERT_TRUE(dhorn::is_c_string_v<const char32_t *>);
    ASSERT_TRUE(dhorn::is_c_string_v<char32_t * const>);
    ASSERT_TRUE(dhorn::is_c_string_v<const char32_t * const>);

    DoFunctionTest(const_cast<char32_t *>(U"foo"), true);
    DoFunctionTest(const_cast<char32_t *>(U"foo"), true);
    DoFunctionTest(static_cast<const char32_t *>(U"foo"), true);
    DoFunctionTest(static_cast<const char32_t *>(U"foo"), true);
}

TEST_F(IsCStringTests, VoidPointerTest)
{
    ASSERT_FALSE(dhorn::is_c_string_v<void *>);
    DoFunctionTest(static_cast<void *>(const_cast<char *>("foo")), false);
}

TEST_F(IsCStringTests, CharArrayTest)
{
    char arr[] = "foo";
    ASSERT_FALSE(dhorn::is_c_string_v<decltype(arr)>);
    DoFunctionTest(arr, false);
}

TEST_F(IsCStringTests, ConstCharArrayTest)
{
    const char arr[] = "foo";
    ASSERT_FALSE(dhorn::is_c_string_v<decltype(arr)>);
    DoFunctionTest(arr, false);
}

TEST_F(IsCStringTests, ValueTest)
{
    ASSERT_FALSE(dhorn::is_c_string_v<int>);
    DoFunctionTest(1, false);
}

/*
 * Duncan Horn
 *
 * UtilityTests.cpp
 *
 * Tests for the utility.h header file
 */

#include <dhorn/utility.h>
#include <gtest/gtest.h>

using namespace dhorn::literals;

TEST(JoinIntegerSequenceTests, JoinEmptySequencesTest)
{
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<>,
        dhorn::join_integer_sequence_t<std::index_sequence<>, std::index_sequence<>>>));
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<1>,
        dhorn::join_integer_sequence_t<std::index_sequence<1>, std::index_sequence<>>>));
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<1>,
        dhorn::join_integer_sequence_t<std::index_sequence<>, std::index_sequence<1>>>));
}

TEST(JoinIntegerSequenceTests, JoinNonEmptySequencesTest)
{
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<0, 1>,
        dhorn::join_integer_sequence_t<std::index_sequence<0>, std::index_sequence<1>>>));
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<0, 1, 2, 3>,
        dhorn::join_integer_sequence_t<std::index_sequence<0>, std::index_sequence<1, 2, 3>>>));
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<0, 1, 2, 3>,
        dhorn::join_integer_sequence_t<std::index_sequence<0, 1, 2>, std::index_sequence<3>>>));
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<0, 1, 2, 3, 4, 5>,
        dhorn::join_integer_sequence_t<std::index_sequence<0, 1, 2>, std::index_sequence<3, 4, 5>>>));
}

TEST(MakeReverseIntegerSequenceTests, MakeEmptySequenceTest)
{
    ASSERT_TRUE((std::is_same_v<std::integer_sequence<int>, dhorn::make_reverse_integer_sequence<int, 0>>));
    ASSERT_TRUE((std::is_same_v<std::index_sequence<>, dhorn::make_reverse_index_sequence<0>>));
}

TEST(MakeReverseIntegerSequenceTests, MakeSingleElementSequenceTest)
{
    ASSERT_TRUE((std::is_same_v<std::integer_sequence<int, 0>, dhorn::make_reverse_integer_sequence<int, 1>>));
    ASSERT_TRUE((std::is_same_v<std::index_sequence<0>, dhorn::make_reverse_index_sequence<1>>));
}

TEST(MakeReverseIntegerSequenceTests, MakeMultipleElementSequenceTest)
{
    ASSERT_TRUE((std::is_same_v<
        std::integer_sequence<int, 5, 4, 3, 2, 1, 0>,
        dhorn::make_reverse_integer_sequence<int, 6>>));
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<5, 4, 3, 2, 1, 0>,
        dhorn::make_reverse_index_sequence<6>>));
}

TEST(ReverseIntegerSequenceTests, ReverseEmptySequenceTest)
{
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<>,
        dhorn::reverse_integer_sequence_t<std::index_sequence<>>>));
}

TEST(ReverseIntegerSequenceTests, ReverseSingleElementSequenceTest)
{
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<0>,
        dhorn::reverse_integer_sequence_t<std::index_sequence<0>>>));

    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<8>,
        dhorn::reverse_integer_sequence_t<std::index_sequence<8>>>));
}

TEST(ReverseIntegerSequenceTests, ReverseMultipleElementSequenceTest)
{
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<0, 1>,
        dhorn::reverse_integer_sequence_t<std::index_sequence<1, 0>>>));
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<0, 1, 2, 3, 4, 5>,
        dhorn::reverse_integer_sequence_t<std::index_sequence<5, 4, 3, 2, 1, 0>>>));
    ASSERT_TRUE((std::is_same_v<
        std::make_index_sequence<10>,
        dhorn::reverse_integer_sequence_t<dhorn::reverse_integer_sequence_t<std::make_index_sequence<10>>>>));

    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<8, 42, 99, 0>,
        dhorn::reverse_integer_sequence_t<std::index_sequence<0, 99, 42, 8>>>));
}

TEST(IncrementDecrementIntegerSequenceTests, IncrementEmptySequenceTest)
{
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<>,
        dhorn::increment_integer_sequence_t<std::index_sequence<>, 0>>));
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<>,
        dhorn::increment_integer_sequence_t<std::index_sequence<>, 42>>));
}

TEST(IncrementDecrementIntegerSequenceTests, DecrementEmptySequenceTest)
{
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<>,
        dhorn::decrement_integer_sequence_t<std::index_sequence<>, 0>>));
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<>,
        dhorn::decrement_integer_sequence_t<std::index_sequence<>, 42>>));
}

TEST(IncrementDecrementIntegerSequenceTests, IncrementSingleElementSequenceTest)
{
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<42>,
        dhorn::increment_integer_sequence_t<std::index_sequence<42>, 0>>));
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<42>,
        dhorn::increment_integer_sequence_t<std::index_sequence<0>, 42>>));
}

TEST(IncrementDecrementIntegerSequenceTests, DecrementSingleElementSequenceTest)
{
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<42>,
        dhorn::decrement_integer_sequence_t<std::index_sequence<42>, 0>>));
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<0>,
        dhorn::decrement_integer_sequence_t<std::index_sequence<42>, 42>>));
}

TEST(IncrementDecrementIntegerSequenceTests, IncrementMultipleElementsSequenceTest)
{
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<10, 11, 12, 13, 14>,
        dhorn::increment_integer_sequence_t<std::make_index_sequence<5>, 10>>));
    ASSERT_TRUE((std::is_same_v<
        std::integer_sequence<int, 0, 1, 2, 3, 4>,
        dhorn::increment_integer_sequence_t<std::integer_sequence<int, 10, 11, 12, 13, 14>, -10>>));
}

TEST(IncrementDecrementIntegerSequenceTests, DecrementMultipleElementsSequenceTest)
{
    ASSERT_TRUE((std::is_same_v<
        std::index_sequence<0, 1, 2, 3, 4>,
        dhorn::decrement_integer_sequence_t<std::index_sequence<10, 11, 12, 13, 14>, 10>>));
    ASSERT_TRUE((std::is_same_v<
        std::integer_sequence<int, -10, -9, -8, -7, -6>,
        dhorn::decrement_integer_sequence_t<std::make_integer_sequence<int, 5>, 10>>));
}

TEST(GetByteTests, Unsigned32BitValueTest)
{
    std::uint32_t value = 0;
    ASSERT_EQ(static_cast<std::uint8_t>(0), dhorn::get_byte<0>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0), dhorn::get_byte<1>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0), dhorn::get_byte<2>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0), dhorn::get_byte<3>(value));

    value = 0xAABBCCDD;
    ASSERT_EQ(static_cast<std::uint8_t>(0xDD), dhorn::get_byte<0>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0xCC), dhorn::get_byte<1>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0xBB), dhorn::get_byte<2>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0xAA), dhorn::get_byte<3>(value));

    value = 0x11223344;
    ASSERT_EQ(static_cast<std::uint8_t>(0x44), dhorn::get_byte<0>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0x33), dhorn::get_byte<1>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0x22), dhorn::get_byte<2>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0x11), dhorn::get_byte<3>(value));

    value = 0xFFFFFFFF;
    ASSERT_EQ(static_cast<std::uint8_t>(0xFF), dhorn::get_byte<0>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0xFF), dhorn::get_byte<1>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0xFF), dhorn::get_byte<2>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0xFF), dhorn::get_byte<3>(value));
}

TEST(GetByteTests, Signed32BitValueTest)
{
    std::uint32_t value = 0;
    ASSERT_EQ(static_cast<std::uint8_t>(0), dhorn::get_byte<0>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0), dhorn::get_byte<1>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0), dhorn::get_byte<2>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0), dhorn::get_byte<3>(value));

    value = 0xAABBCCDD;
    ASSERT_EQ(static_cast<std::uint8_t>(0xDD), dhorn::get_byte<0>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0xCC), dhorn::get_byte<1>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0xBB), dhorn::get_byte<2>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0xAA), dhorn::get_byte<3>(value));

    value = 0x11223344;
    ASSERT_EQ(static_cast<std::uint8_t>(0x44), dhorn::get_byte<0>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0x33), dhorn::get_byte<1>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0x22), dhorn::get_byte<2>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0x11), dhorn::get_byte<3>(value));

    value = 0xFFFFFFFF;
    ASSERT_EQ(static_cast<std::uint8_t>(0xFF), dhorn::get_byte<0>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0xFF), dhorn::get_byte<1>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0xFF), dhorn::get_byte<2>(value));
    ASSERT_EQ(static_cast<std::uint8_t>(0xFF), dhorn::get_byte<3>(value));
}

TEST(GetByteTests, Unsigned8BitValueTest)
{
    std::uint8_t value = 0;
    ASSERT_EQ(static_cast<std::uint8_t>(0), dhorn::get_byte<0>(value));

    value = 0xAA;
    ASSERT_EQ(static_cast<std::uint8_t>(0xAA), dhorn::get_byte<0>(value));

    value = 0x11;
    ASSERT_EQ(static_cast<std::uint8_t>(0x11), dhorn::get_byte<0>(value));

    value = 0xFF;
    ASSERT_EQ(static_cast<std::uint8_t>(0xFF), dhorn::get_byte<0>(value));
}

TEST(GetByteTests, Signed8BitValueTest)
{
    std::int8_t value = 0;
    ASSERT_EQ(static_cast<std::uint8_t>(0), dhorn::get_byte<0>(value));

    value = static_cast<std::uint8_t>(0xAA);
    ASSERT_EQ(static_cast<std::uint8_t>(0xAA), dhorn::get_byte<0>(value));

    value = 0x11;
    ASSERT_EQ(static_cast<std::uint8_t>(0x11), dhorn::get_byte<0>(value));

    value = static_cast<std::uint8_t>(0xFF);
    ASSERT_EQ(static_cast<std::uint8_t>(0xFF), dhorn::get_byte<0>(value));
}

TEST(VariantIndexTests, CompilationErrorsTest)
{
    // NOTE: Compilation test only; each should fail
    //dhorn::variant_index_v<int, std::variant<>>;
    //dhorn::variant_index_v<int, std::variant<int&>>;
}

TEST(VariantIndexTests, SingleTypeTest)
{
    ASSERT_EQ(0_sz, (dhorn::variant_index_v<int, std::variant<int>>));
}

TEST(VariantIndexTests, MultipleTypeTest)
{
    using variant_type = std::variant<int, float, double, std::string, char, unsigned char>;

    ASSERT_EQ(0_sz, (dhorn::variant_index_v<int, variant_type>));
    ASSERT_EQ(1_sz, (dhorn::variant_index_v<float, variant_type>));
    ASSERT_EQ(2_sz, (dhorn::variant_index_v<double, variant_type>));
    ASSERT_EQ(3_sz, (dhorn::variant_index_v<std::string, variant_type>));
    ASSERT_EQ(4_sz, (dhorn::variant_index_v<char, variant_type>));
    ASSERT_EQ(5_sz, (dhorn::variant_index_v<unsigned char, variant_type>));
}

TEST(VariantIndexTests, WithReferencesTest)
{
    using variant_type = std::variant<int, int&, const int&, volatile int, const volatile int&>;

    ASSERT_EQ(0_sz, (dhorn::variant_index_v<int, variant_type>));
    ASSERT_EQ(1_sz, (dhorn::variant_index_v<int&, variant_type>));
    ASSERT_EQ(2_sz, (dhorn::variant_index_v<const int&, variant_type>));
    ASSERT_EQ(3_sz, (dhorn::variant_index_v<volatile int, variant_type>));
    ASSERT_EQ(4_sz, (dhorn::variant_index_v<const volatile int&, variant_type>));
}

#define CONSTEXPR_ASSERT_TRUE(expr) \
{ \
    constexpr bool result = (expr); \
    ASSERT_TRUE(result); \
}

#define CONSTEXPR_ASSERT_FALSE(expr) \
{ \
    constexpr bool result = (expr); \
    ASSERT_FALSE(result); \
}

TEST(AnyOfTests, CompareWithEmptyTest)
{
    CONSTEXPR_ASSERT_FALSE(0 == dhorn::any_of());
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of() == 0);
    CONSTEXPR_ASSERT_FALSE(0 != dhorn::any_of());
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of() != 0);
    CONSTEXPR_ASSERT_FALSE(0 < dhorn::any_of());
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of() < 0);
    CONSTEXPR_ASSERT_FALSE(0 > dhorn::any_of());
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of() > 0);
    CONSTEXPR_ASSERT_FALSE(0 <= dhorn::any_of());
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of() <= 0);
    CONSTEXPR_ASSERT_FALSE(0 >= dhorn::any_of());
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of() >= 0);
}

TEST(AnyOfTests, CompareWithSingleValueTest)
{
    CONSTEXPR_ASSERT_FALSE(0 == dhorn::any_of(1));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(1) == 0);
    CONSTEXPR_ASSERT_TRUE(0 == dhorn::any_of(0));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(0) == 0);

    CONSTEXPR_ASSERT_TRUE(0 != dhorn::any_of(1));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1) != 0);
    CONSTEXPR_ASSERT_FALSE(0 != dhorn::any_of(0));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(0) != 0);

    CONSTEXPR_ASSERT_TRUE(0 < dhorn::any_of(1));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(1) < 0);
    CONSTEXPR_ASSERT_FALSE(0 < dhorn::any_of(0));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(0) < 0);

    CONSTEXPR_ASSERT_FALSE(0 > dhorn::any_of(1));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1) > 0);
    CONSTEXPR_ASSERT_FALSE(0 > dhorn::any_of(0));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(0) > 0);

    CONSTEXPR_ASSERT_TRUE(0 <= dhorn::any_of(1));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(1) <= 0);
    CONSTEXPR_ASSERT_TRUE(0 <= dhorn::any_of(0));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(0) <= 0);

    CONSTEXPR_ASSERT_FALSE(0 >= dhorn::any_of(1));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1) >= 0);
    CONSTEXPR_ASSERT_TRUE(0 >= dhorn::any_of(0));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(0) >= 0);
}

TEST(AnyOfTests, CompareWithMultipleValuesTest)
{
    CONSTEXPR_ASSERT_FALSE(0 == dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(1, 2, 3, 4, 5) == 0);
    CONSTEXPR_ASSERT_TRUE(1 == dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) == 1);
    CONSTEXPR_ASSERT_TRUE(2 == dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) == 2);
    CONSTEXPR_ASSERT_TRUE(3 == dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) == 3);
    CONSTEXPR_ASSERT_TRUE(4 == dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) == 4);
    CONSTEXPR_ASSERT_TRUE(5 == dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) == 5);
    CONSTEXPR_ASSERT_FALSE(6 == dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(1, 2, 3, 4, 5) == 6);

    CONSTEXPR_ASSERT_TRUE(0 != dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) != 0);
    CONSTEXPR_ASSERT_TRUE(1 != dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) != 1);
    CONSTEXPR_ASSERT_TRUE(2 != dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) != 2);
    CONSTEXPR_ASSERT_TRUE(3 != dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) != 3);
    CONSTEXPR_ASSERT_TRUE(4 != dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) != 4);
    CONSTEXPR_ASSERT_TRUE(5 != dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) != 5);
    CONSTEXPR_ASSERT_TRUE(6 != dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) != 6);

    CONSTEXPR_ASSERT_TRUE(0 < dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(1, 2, 3, 4, 5) < 0);
    CONSTEXPR_ASSERT_TRUE(1 < dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(1, 2, 3, 4, 5) < 1);
    CONSTEXPR_ASSERT_TRUE(2 < dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) < 2);
    CONSTEXPR_ASSERT_TRUE(3 < dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) < 3);
    CONSTEXPR_ASSERT_TRUE(4 < dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) < 4);
    CONSTEXPR_ASSERT_FALSE(5 < dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) < 5);
    CONSTEXPR_ASSERT_FALSE(6 < dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) < 6);

    CONSTEXPR_ASSERT_FALSE(0 > dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) > 0);
    CONSTEXPR_ASSERT_FALSE(1 > dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) > 1);
    CONSTEXPR_ASSERT_TRUE(2 > dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) > 2);
    CONSTEXPR_ASSERT_TRUE(3 > dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) > 3);
    CONSTEXPR_ASSERT_TRUE(4 > dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) > 4);
    CONSTEXPR_ASSERT_TRUE(5 > dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(1, 2, 3, 4, 5) > 5);
    CONSTEXPR_ASSERT_TRUE(6 > dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(1, 2, 3, 4, 5) > 6);

    CONSTEXPR_ASSERT_TRUE(0 <= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(1, 2, 3, 4, 5) <= 0);
    CONSTEXPR_ASSERT_TRUE(1 <= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) <= 1);
    CONSTEXPR_ASSERT_TRUE(2 <= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) <= 2);
    CONSTEXPR_ASSERT_TRUE(3 <= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) <= 3);
    CONSTEXPR_ASSERT_TRUE(4 <= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) <= 4);
    CONSTEXPR_ASSERT_TRUE(5 <= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) <= 5);
    CONSTEXPR_ASSERT_FALSE(6 <= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) <= 6);

    CONSTEXPR_ASSERT_FALSE(0 >= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) >= 0);
    CONSTEXPR_ASSERT_TRUE(1 >= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) >= 1);
    CONSTEXPR_ASSERT_TRUE(2 >= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) >= 2);
    CONSTEXPR_ASSERT_TRUE(3 >= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) >= 3);
    CONSTEXPR_ASSERT_TRUE(4 >= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) >= 4);
    CONSTEXPR_ASSERT_TRUE(5 >= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::any_of(1, 2, 3, 4, 5) >= 5);
    CONSTEXPR_ASSERT_TRUE(6 >= dhorn::any_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::any_of(1, 2, 3, 4, 5) >= 6);
}

TEST(AllOfTests, CompareWithEmptyTest)
{
    CONSTEXPR_ASSERT_TRUE(0 == dhorn::all_of());
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of() == 0);
    CONSTEXPR_ASSERT_TRUE(0 != dhorn::all_of());
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of() != 0);
    CONSTEXPR_ASSERT_TRUE(0 < dhorn::all_of());
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of() < 0);
    CONSTEXPR_ASSERT_TRUE(0 > dhorn::all_of());
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of() > 0);
    CONSTEXPR_ASSERT_TRUE(0 <= dhorn::all_of());
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of() <= 0);
    CONSTEXPR_ASSERT_TRUE(0 >= dhorn::all_of());
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of() >= 0);
}

TEST(AllOfTests, CompareWithSingleValueTest)
{
    CONSTEXPR_ASSERT_FALSE(0 == dhorn::all_of(1));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1) == 0);
    CONSTEXPR_ASSERT_TRUE(0 == dhorn::all_of(0));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(0) == 0);

    CONSTEXPR_ASSERT_TRUE(0 != dhorn::all_of(1));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(1) != 0);
    CONSTEXPR_ASSERT_FALSE(0 != dhorn::all_of(0));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(0) != 0);

    CONSTEXPR_ASSERT_TRUE(0 < dhorn::all_of(1));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1) < 0);
    CONSTEXPR_ASSERT_FALSE(0 < dhorn::all_of(0));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(0) < 0);

    CONSTEXPR_ASSERT_FALSE(0 > dhorn::all_of(1));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(1) > 0);
    CONSTEXPR_ASSERT_FALSE(0 > dhorn::all_of(0));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(0) > 0);

    CONSTEXPR_ASSERT_TRUE(0 <= dhorn::all_of(1));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1) <= 0);
    CONSTEXPR_ASSERT_TRUE(0 <= dhorn::all_of(0));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(0) <= 0);

    CONSTEXPR_ASSERT_FALSE(0 >= dhorn::all_of(1));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(1) >= 0);
    CONSTEXPR_ASSERT_TRUE(0 >= dhorn::all_of(0));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(0) >= 0);
}

TEST(AllOfTests, CompareWithMultipleValuesTest)
{
    CONSTEXPR_ASSERT_FALSE(0 == dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) == 0);
    CONSTEXPR_ASSERT_FALSE(1 == dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) == 1);
    CONSTEXPR_ASSERT_FALSE(2 == dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) == 2);
    CONSTEXPR_ASSERT_FALSE(3 == dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) == 3);
    CONSTEXPR_ASSERT_FALSE(4 == dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) == 4);
    CONSTEXPR_ASSERT_FALSE(5 == dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) == 5);
    CONSTEXPR_ASSERT_FALSE(6 == dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) == 6);
    CONSTEXPR_ASSERT_TRUE(0 == dhorn::all_of(0, 0, 0, 0, 0));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(0, 0, 0, 0, 0) == 0);

    CONSTEXPR_ASSERT_TRUE(0 != dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(1, 2, 3, 4, 5) != 0);
    CONSTEXPR_ASSERT_FALSE(1 != dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) != 1);
    CONSTEXPR_ASSERT_FALSE(2 != dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) != 2);
    CONSTEXPR_ASSERT_FALSE(3 != dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) != 3);
    CONSTEXPR_ASSERT_FALSE(4 != dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) != 4);
    CONSTEXPR_ASSERT_FALSE(5 != dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) != 5);
    CONSTEXPR_ASSERT_TRUE(6 != dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(1, 2, 3, 4, 5) != 6);

    CONSTEXPR_ASSERT_TRUE(0 < dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) < 0);
    CONSTEXPR_ASSERT_FALSE(1 < dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) < 1);
    CONSTEXPR_ASSERT_FALSE(2 < dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) < 2);
    CONSTEXPR_ASSERT_FALSE(3 < dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) < 3);
    CONSTEXPR_ASSERT_FALSE(4 < dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) < 4);
    CONSTEXPR_ASSERT_FALSE(5 < dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) < 5);
    CONSTEXPR_ASSERT_FALSE(6 < dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(1, 2, 3, 4, 5) < 6);

    CONSTEXPR_ASSERT_FALSE(0 > dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(1, 2, 3, 4, 5) > 0);
    CONSTEXPR_ASSERT_FALSE(1 > dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) > 1);
    CONSTEXPR_ASSERT_FALSE(2 > dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) > 2);
    CONSTEXPR_ASSERT_FALSE(3 > dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) > 3);
    CONSTEXPR_ASSERT_FALSE(4 > dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) > 4);
    CONSTEXPR_ASSERT_FALSE(5 > dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) > 5);
    CONSTEXPR_ASSERT_TRUE(6 > dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) > 6);

    CONSTEXPR_ASSERT_TRUE(0 <= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) <= 0);
    CONSTEXPR_ASSERT_TRUE(1 <= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) <= 1);
    CONSTEXPR_ASSERT_FALSE(2 <= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) <= 2);
    CONSTEXPR_ASSERT_FALSE(3 <= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) <= 3);
    CONSTEXPR_ASSERT_FALSE(4 <= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) <= 4);
    CONSTEXPR_ASSERT_FALSE(5 <= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(1, 2, 3, 4, 5) <= 5);
    CONSTEXPR_ASSERT_FALSE(6 <= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(1, 2, 3, 4, 5) <= 6);

    CONSTEXPR_ASSERT_FALSE(0 >= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(1, 2, 3, 4, 5) >= 0);
    CONSTEXPR_ASSERT_FALSE(1 >= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_TRUE(dhorn::all_of(1, 2, 3, 4, 5) >= 1);
    CONSTEXPR_ASSERT_FALSE(2 >= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) >= 2);
    CONSTEXPR_ASSERT_FALSE(3 >= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) >= 3);
    CONSTEXPR_ASSERT_FALSE(4 >= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) >= 4);
    CONSTEXPR_ASSERT_TRUE(5 >= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) >= 5);
    CONSTEXPR_ASSERT_TRUE(6 >= dhorn::all_of(1, 2, 3, 4, 5));
    CONSTEXPR_ASSERT_FALSE(dhorn::all_of(1, 2, 3, 4, 5) >= 6);
}

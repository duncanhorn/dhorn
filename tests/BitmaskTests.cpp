/*
 * Duncan Horn
 *
 * BitmaskTests.cpp
 *
 * Tests for the bitmask.h header file
 */

#include <dhorn/bitmask.h>
#include <gtest/gtest.h>

enum class test_enum
{
    invalid = -1,
    value0 = 0x00,
    value1 = 0x01,
    value2 = 0x02,
    value3 = 0x03,
    value4 = 0x04,
};
DHORN_DECLARE_BITMASK_OPERATORS(test_enum);

TEST(BitmaskTests, SetFlagTest)
{
    int value = 0;
    value = dhorn::set_flag(value, 0x01);
    ASSERT_EQ(0x01, value);

    // Setting twice shouldn't do anything
    value = dhorn::set_flag(value, 0x01);
    ASSERT_EQ(0x01, value);

    // Signed/unsigned difference
    value = dhorn::set_flag(value, 0x02u);
    ASSERT_EQ(0x03, value);

    // constexpr tests
    static_assert(dhorn::set_flags(0x55, 0xAAu) == 0xFF);
}

TEST(BitmaskTests, ClearFlagTest)
{
    int value = 0x03;
    value = dhorn::clear_flag(value, 0x01);
    ASSERT_EQ(0x02, value);

    // Clearing twice shouldn't do anything
    value = dhorn::clear_flag(value, 0x01);
    ASSERT_EQ(0x02, value);

    // Clearing a non-set flag should be okay
    value = dhorn::clear_flag(value, 0x04);
    ASSERT_EQ(0x02, value);

    value = dhorn::clear_flag(value, 0x0F);
    ASSERT_EQ(0, value);

    // Signed/unsigned difference
    value = 0x03;
    value = dhorn::clear_flag(value, 0x02u);
    ASSERT_EQ(0x01, value);

    // constexpr tests
    static_assert(dhorn::clear_flags(0xAA, 0xCCu) == 0x22);
}

TEST(BitmaskTests, ToggleFlagTest)
{
    int value = 0;
    value = dhorn::toggle_flag(value, 0x05);
    ASSERT_EQ(0x05, value);
    value = dhorn::toggle_flag(value, 0x0A);
    ASSERT_EQ(0x0F, value);

    value = dhorn::toggle_flag(value, 0x05);
    ASSERT_EQ(0x0A, value);
    value = dhorn::toggle_flag(value, 0x0A);
    ASSERT_EQ(0x00, value);

    // Signed/unsigned difference
    value = dhorn::toggle_flag(value, 0xA5u);
    ASSERT_EQ(0xA5, value);

    // constexpr tests
    static_assert(dhorn::toggle_flags(0xAA, 0xFFu) == 0x55);
}

TEST(BitmaskTests, IsAnyFlagSetTest)
{
    int value = 0x03;
    ASSERT_TRUE(dhorn::is_any_flag_set(value, 0x01));
    ASSERT_TRUE(dhorn::is_any_flag_set(value, 0x02));
    ASSERT_TRUE(dhorn::is_any_flag_set(value, 0x03));
    ASSERT_FALSE(dhorn::is_any_flag_set(value, 0x04));
    ASSERT_FALSE(dhorn::is_any_flag_set(value, 0x00));
    ASSERT_TRUE(dhorn::is_any_flag_set(value, 0xFF));

    // Signed/unsigned difference
    ASSERT_TRUE(dhorn::is_any_flag_set(value, 0xAAu));
}

TEST(BitmaskTests, AreAllFlagsSetTest)
{
    int value = 0x03;
    ASSERT_TRUE(dhorn::are_all_flags_set(value, 0x01));
    ASSERT_TRUE(dhorn::are_all_flags_set(value, 0x02));
    ASSERT_TRUE(dhorn::are_all_flags_set(value, 0x03));
    ASSERT_TRUE(dhorn::are_all_flags_set(value, 0x00));
    ASSERT_FALSE(dhorn::are_all_flags_set(value, 0x04));
    ASSERT_FALSE(dhorn::are_all_flags_set(value, 0x07));
    ASSERT_FALSE(dhorn::are_all_flags_set(value, 0xFF));

    // Signed/unsigned difference
    ASSERT_FALSE(dhorn::are_all_flags_set(value, 0xAAu));
}

TEST(BitmaskTests, AreAllFlagsClearTest)
{
    int value = 0x03;
    ASSERT_FALSE(dhorn::are_all_flags_clear(value, 0x03));
    ASSERT_FALSE(dhorn::are_all_flags_clear(value, 0x01));
    ASSERT_FALSE(dhorn::are_all_flags_clear(value, 0x02));
    ASSERT_TRUE(dhorn::are_all_flags_clear(value, 0x00));
    ASSERT_TRUE(dhorn::are_all_flags_clear(value, 0xFC));
    ASSERT_TRUE(dhorn::are_all_flags_clear(value, 0xF0));

    // Signed/unsigned difference
    ASSERT_FALSE(dhorn::are_all_flags_clear(value, 0xAAu));
}

TEST(BitmaskTests, EnumOperatorOrTest)
{
    ASSERT_TRUE((test_enum::value1 | test_enum::value2) == test_enum::value3);
    static_assert((test_enum::value1 | test_enum::value2) == test_enum::value3, "Should be static_assert-able");

    ASSERT_TRUE((test_enum::value1 | test_enum::value0) == test_enum::value1);
    ASSERT_TRUE((test_enum::value2 | test_enum::value0) == test_enum::value2);
    ASSERT_TRUE((test_enum::value3 | test_enum::value0) == test_enum::value3);
    ASSERT_TRUE((test_enum::value4 | test_enum::value0) == test_enum::value4);
}

TEST(BitmaskTests, EnumOperatorAndTest)
{
    ASSERT_TRUE((test_enum::value3 & test_enum::value1) == test_enum::value1);
    static_assert((test_enum::value3 & test_enum::value1) == test_enum::value1, "Should be static_assert-able");

    ASSERT_TRUE((test_enum::value1 & test_enum::value0) == test_enum::value0);
    ASSERT_TRUE((test_enum::value2 & test_enum::value0) == test_enum::value0);
    ASSERT_TRUE((test_enum::value3 & test_enum::value0) == test_enum::value0);
    ASSERT_TRUE((test_enum::value4 & test_enum::value0) == test_enum::value0);
}

TEST(BitmaskTests, EnumOperatorXorTest)
{
    ASSERT_TRUE((test_enum::value3 ^ test_enum::value1) == test_enum::value2);
    static_assert((test_enum::value3 ^ test_enum::value1) == test_enum::value2, "Should be static_assert-able");

    ASSERT_TRUE((test_enum::value1 ^ test_enum::value0) == test_enum::value1);
    ASSERT_TRUE((test_enum::value2 ^ test_enum::value0) == test_enum::value2);
    ASSERT_TRUE((test_enum::value3 ^ test_enum::value0) == test_enum::value3);
    ASSERT_TRUE((test_enum::value4 ^ test_enum::value0) == test_enum::value4);
}

TEST(BitmaskTests, EnumOperatorBitwiseComplementTest)
{
    ASSERT_TRUE(~test_enum::value0 == test_enum::invalid);
    static_assert(~test_enum::value0 == test_enum::invalid, "Should be static_assert-able");

    // clear_flags uses operator~
    ASSERT_TRUE(dhorn::clear_flags(test_enum::value3, test_enum::value2) == test_enum::value1);
}

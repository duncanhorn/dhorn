/*
 * Duncan Horn
 *
 * BitmaskTests.cpp
 *
 * Tests for the various animations functions/types
 */
#include "stdafx.h"

#include <dhorn/experimental/bitmask.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
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

        TEST_CLASS(BitmaskTests)
        {
            TEST_METHOD(SetFlagTest)
            {
                int value = 0;
                value = dhorn::set_flag(value, 0x01);
                Assert::AreEqual(0x01, value);

                // Setting twice shouldn't do anything
                value = dhorn::set_flag(value, 0x01);
                Assert::AreEqual(0x01, value);
            }

            TEST_METHOD(ClearFlagTest)
            {
                int value = 0x03;
                value = dhorn::clear_flag(value, 0x01);
                Assert::AreEqual(0x02, value);

                // Clearing twice shouldn't do anything
                value = dhorn::clear_flag(value, 0x01);
                Assert::AreEqual(0x02, value);

                // Clearing a non-set flag should be okay
                value = dhorn::clear_flag(value, 0x04);
                Assert::AreEqual(0x02, value);

                value = dhorn::clear_flag(value, 0x0F);
                Assert::AreEqual(0, value);
            }

            TEST_METHOD(ToggleFlagTest)
            {
                int value = 0;
                value = dhorn::toggle_flag(value, 0x05);
                Assert::AreEqual(0x05, value);
                value = dhorn::toggle_flag(value, 0x0A);
                Assert::AreEqual(0x0F, value);

                value = dhorn::toggle_flag(value, 0x05);
                Assert::AreEqual(0x0A, value);
                value = dhorn::toggle_flag(value, 0x0A);
                Assert::AreEqual(0x00, value);
            }

            TEST_METHOD(IsAnyFlagSetTest)
            {
                int value = 0x03;
                Assert::IsTrue(dhorn::is_any_flag_set(value, 0x01));
                Assert::IsTrue(dhorn::is_any_flag_set(value, 0x02));
                Assert::IsTrue(dhorn::is_any_flag_set(value, 0x03));
                Assert::IsFalse(dhorn::is_any_flag_set(value, 0x04));
                Assert::IsFalse(dhorn::is_any_flag_set(value, 0x00));
                Assert::IsTrue(dhorn::is_any_flag_set(value, 0xFF));
            }

            TEST_METHOD(AreAllFlagsSetTest)
            {
                int value = 0x03;
                Assert::IsTrue(dhorn::are_all_flags_set(value, 0x01));
                Assert::IsTrue(dhorn::are_all_flags_set(value, 0x02));
                Assert::IsTrue(dhorn::are_all_flags_set(value, 0x03));
                Assert::IsTrue(dhorn::are_all_flags_set(value, 0x00));
                Assert::IsFalse(dhorn::are_all_flags_set(value, 0x04));
                Assert::IsFalse(dhorn::are_all_flags_set(value, 0x07));
                Assert::IsFalse(dhorn::are_all_flags_set(value, 0xFF));
            }

            TEST_METHOD(AreAllFlagsClearTest)
            {
                int value = 0x03;
                Assert::IsFalse(dhorn::are_all_flags_clear(value, 0x03));
                Assert::IsFalse(dhorn::are_all_flags_clear(value, 0x01));
                Assert::IsFalse(dhorn::are_all_flags_clear(value, 0x02));
                Assert::IsTrue(dhorn::are_all_flags_clear(value, 0x00));
                Assert::IsTrue(dhorn::are_all_flags_clear(value, 0xFC));
                Assert::IsTrue(dhorn::are_all_flags_clear(value, 0xF0));
            }

            TEST_METHOD(EnumOperatorOrTest)
            {
                Assert::IsTrue((test_enum::value1 | test_enum::value2) == test_enum::value3);
                static_assert((test_enum::value1 | test_enum::value2) == test_enum::value3,
                    "Should be static_assert-able");

                Assert::IsTrue((test_enum::value1 | test_enum::value0) == test_enum::value1);
                Assert::IsTrue((test_enum::value2 | test_enum::value0) == test_enum::value2);
                Assert::IsTrue((test_enum::value3 | test_enum::value0) == test_enum::value3);
                Assert::IsTrue((test_enum::value4 | test_enum::value0) == test_enum::value4);
            }

            TEST_METHOD(EnumOperatorAndTest)
            {
                Assert::IsTrue((test_enum::value3 & test_enum::value1) == test_enum::value1);
                static_assert((test_enum::value3 & test_enum::value1) == test_enum::value1,
                    "Should be static_assert-able");

                Assert::IsTrue((test_enum::value1 & test_enum::value0) == test_enum::value0);
                Assert::IsTrue((test_enum::value2 & test_enum::value0) == test_enum::value0);
                Assert::IsTrue((test_enum::value3 & test_enum::value0) == test_enum::value0);
                Assert::IsTrue((test_enum::value4 & test_enum::value0) == test_enum::value0);
            }

            TEST_METHOD(EnumOperatorXorTest)
            {
                Assert::IsTrue((test_enum::value3 ^ test_enum::value1) == test_enum::value2);
                static_assert((test_enum::value3 ^ test_enum::value1) == test_enum::value2,
                    "Should be static_assert-able");

                Assert::IsTrue((test_enum::value1 ^ test_enum::value0) == test_enum::value1);
                Assert::IsTrue((test_enum::value2 ^ test_enum::value0) == test_enum::value2);
                Assert::IsTrue((test_enum::value3 ^ test_enum::value0) == test_enum::value3);
                Assert::IsTrue((test_enum::value4 ^ test_enum::value0) == test_enum::value4);
            }

            TEST_METHOD(EnumOperatorBitwiseComplementTest)
            {
                Assert::IsTrue(~test_enum::value0 == test_enum::invalid);
                static_assert(~test_enum::value0 == test_enum::invalid, "Should be static_assert-able");

                // clear_flags uses operator~
                Assert::IsTrue(dhorn::clear_flags(test_enum::value3, test_enum::value2) ==
                    test_enum::value1);
            }
        };
    }
}

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
        TEST_CLASS(BitmaskTests)
        {
            TEST_METHOD(SetFlagTest)
            {
                int value = 0;
                value = dhorn::experimental::set_flag(value, 0x01);
                Assert::AreEqual(0x01, value);

                // Setting twice shouldn't do anything
                value = dhorn::experimental::set_flag(value, 0x01);
                Assert::AreEqual(0x01, value);
            }

            TEST_METHOD(ClearFlagTest)
            {
                int value = 0x03;
                value = dhorn::experimental::clear_flag(value, 0x01);
                Assert::AreEqual(0x02, value);

                // Clearing twice shouldn't do anything
                value = dhorn::experimental::clear_flag(value, 0x01);
                Assert::AreEqual(0x02, value);

                // Clearing a non-set flag should be okay
                value = dhorn::experimental::clear_flag(value, 0x04);
                Assert::AreEqual(0x02, value);

                value = dhorn::experimental::clear_flag(value, 0x0F);
                Assert::AreEqual(0, value);
            }

            TEST_METHOD(ToggleFlagTest)
            {
                int value = 0;
                value = dhorn::experimental::toggle_flag(value, 0x05);
                Assert::AreEqual(0x05, value);
                value = dhorn::experimental::toggle_flag(value, 0x0A);
                Assert::AreEqual(0x0F, value);

                value = dhorn::experimental::toggle_flag(value, 0x05);
                Assert::AreEqual(0x0A, value);
                value = dhorn::experimental::toggle_flag(value, 0x0A);
                Assert::AreEqual(0x00, value);
            }

            TEST_METHOD(IsAnyFlagSetTest)
            {
                int value = 0x03;
                Assert::IsTrue(dhorn::experimental::is_any_flag_set(value, 0x01));
                Assert::IsTrue(dhorn::experimental::is_any_flag_set(value, 0x02));
                Assert::IsTrue(dhorn::experimental::is_any_flag_set(value, 0x03));
                Assert::IsFalse(dhorn::experimental::is_any_flag_set(value, 0x04));
                Assert::IsFalse(dhorn::experimental::is_any_flag_set(value, 0x00));
                Assert::IsTrue(dhorn::experimental::is_any_flag_set(value, 0xFF));
            }

            TEST_METHOD(AreAllFlagsSetTest)
            {
                int value = 0x03;
                Assert::IsTrue(dhorn::experimental::are_all_flags_set(value, 0x01));
                Assert::IsTrue(dhorn::experimental::are_all_flags_set(value, 0x02));
                Assert::IsTrue(dhorn::experimental::are_all_flags_set(value, 0x03));
                Assert::IsTrue(dhorn::experimental::are_all_flags_set(value, 0x00));
                Assert::IsFalse(dhorn::experimental::are_all_flags_set(value, 0x04));
                Assert::IsFalse(dhorn::experimental::are_all_flags_set(value, 0x07));
                Assert::IsFalse(dhorn::experimental::are_all_flags_set(value, 0xFF));
            }

            TEST_METHOD(AreAllFlagsClearTest)
            {
                int value = 0x03;
                Assert::IsFalse(dhorn::experimental::are_all_flags_clear(value, 0x03));
                Assert::IsFalse(dhorn::experimental::are_all_flags_clear(value, 0x01));
                Assert::IsFalse(dhorn::experimental::are_all_flags_clear(value, 0x02));
                Assert::IsTrue(dhorn::experimental::are_all_flags_clear(value, 0x00));
                Assert::IsTrue(dhorn::experimental::are_all_flags_clear(value, 0xFC));
                Assert::IsTrue(dhorn::experimental::are_all_flags_clear(value, 0xF0));
            }
        };
    }
}

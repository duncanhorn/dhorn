/*
 * Duncan Horn
 *
 * BitmaskTests.cpp
 *
 * Tests for the various animations functions/types
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include "dhorn/bitmask.h"

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
        };
    }
}

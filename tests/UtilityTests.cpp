/*
 * Duncan Horn
 *
 * UtilityTests.cpp
 *
 * Tests for the utility.h header file
 */
#include "stdafx.h"

#include <dhorn/utility.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(JoinIntegerSequenceTests)
    {
        TEST_METHOD(JoinEmptySequencesTest)
        {
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<>,
                join_integer_sequence_t<std::index_sequence<>, std::index_sequence<>>>);
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<1>,
                join_integer_sequence_t<std::index_sequence<1>, std::index_sequence<>>>);
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<1>,
                join_integer_sequence_t<std::index_sequence<>, std::index_sequence<1>>>);
        }

        TEST_METHOD(JoinNonEmptySequencesTest)
        {
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<0, 1>,
                join_integer_sequence_t<std::index_sequence<0>, std::index_sequence<1>>>);
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<0, 1, 2, 3>,
                join_integer_sequence_t<std::index_sequence<0>, std::index_sequence<1, 2, 3>>>);
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<0, 1, 2, 3>,
                join_integer_sequence_t<std::index_sequence<0, 1, 2>, std::index_sequence<3>>>);
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<0, 1, 2, 3, 4, 5>,
                join_integer_sequence_t<std::index_sequence<0, 1, 2>, std::index_sequence<3, 4, 5>>>);
        }
    };



    TEST_CLASS(ReverseIntegerSequenceTests)
    {
        TEST_METHOD(ReverseEmptySequenceTest)
        {
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<>,
                reverse_integer_sequence_t<std::index_sequence<>>>);
        }

        TEST_METHOD(ReverseSingleElementSequenceTest)
        {
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<0>,
                reverse_integer_sequence_t<std::index_sequence<0>>>);
        }

        TEST_METHOD(ReverseMultipleElementSequenceTest)
        {
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<0, 1>,
                reverse_integer_sequence_t<std::index_sequence<1, 0>>>);
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<0, 1, 2, 3, 4, 5>,
                reverse_integer_sequence_t<std::index_sequence<5, 4, 3, 2, 1, 0>>>);
            Assert::IsTrue(std::is_same_v<
                std::make_index_sequence<10>,
                reverse_integer_sequence_t<reverse_integer_sequence_t<std::make_index_sequence<10>>>>);
        }
    };



    TEST_CLASS(GetByteTests)
    {
        TEST_METHOD(Unsigned32BitValueTest)
        {
            uint32_t value = 0;
            Assert::AreEqual(static_cast<uint8_t>(0), get_byte<0>(value));
            Assert::AreEqual(static_cast<uint8_t>(0), get_byte<1>(value));
            Assert::AreEqual(static_cast<uint8_t>(0), get_byte<2>(value));
            Assert::AreEqual(static_cast<uint8_t>(0), get_byte<3>(value));

            value = 0xAABBCCDD;
            Assert::AreEqual(static_cast<uint8_t>(0xDD), get_byte<0>(value));
            Assert::AreEqual(static_cast<uint8_t>(0xCC), get_byte<1>(value));
            Assert::AreEqual(static_cast<uint8_t>(0xBB), get_byte<2>(value));
            Assert::AreEqual(static_cast<uint8_t>(0xAA), get_byte<3>(value));

            value = 0x11223344;
            Assert::AreEqual(static_cast<uint8_t>(0x44), get_byte<0>(value));
            Assert::AreEqual(static_cast<uint8_t>(0x33), get_byte<1>(value));
            Assert::AreEqual(static_cast<uint8_t>(0x22), get_byte<2>(value));
            Assert::AreEqual(static_cast<uint8_t>(0x11), get_byte<3>(value));

            value = 0xFFFFFFFF;
            Assert::AreEqual(static_cast<uint8_t>(0xFF), get_byte<0>(value));
            Assert::AreEqual(static_cast<uint8_t>(0xFF), get_byte<1>(value));
            Assert::AreEqual(static_cast<uint8_t>(0xFF), get_byte<2>(value));
            Assert::AreEqual(static_cast<uint8_t>(0xFF), get_byte<3>(value));
        }

        TEST_METHOD(Signed32BitValueTest)
        {
            uint32_t value = 0;
            Assert::AreEqual(static_cast<uint8_t>(0), get_byte<0>(value));
            Assert::AreEqual(static_cast<uint8_t>(0), get_byte<1>(value));
            Assert::AreEqual(static_cast<uint8_t>(0), get_byte<2>(value));
            Assert::AreEqual(static_cast<uint8_t>(0), get_byte<3>(value));

            value = 0xAABBCCDD;
            Assert::AreEqual(static_cast<uint8_t>(0xDD), get_byte<0>(value));
            Assert::AreEqual(static_cast<uint8_t>(0xCC), get_byte<1>(value));
            Assert::AreEqual(static_cast<uint8_t>(0xBB), get_byte<2>(value));
            Assert::AreEqual(static_cast<uint8_t>(0xAA), get_byte<3>(value));

            value = 0x11223344;
            Assert::AreEqual(static_cast<uint8_t>(0x44), get_byte<0>(value));
            Assert::AreEqual(static_cast<uint8_t>(0x33), get_byte<1>(value));
            Assert::AreEqual(static_cast<uint8_t>(0x22), get_byte<2>(value));
            Assert::AreEqual(static_cast<uint8_t>(0x11), get_byte<3>(value));

            value = 0xFFFFFFFF;
            Assert::AreEqual(static_cast<uint8_t>(0xFF), get_byte<0>(value));
            Assert::AreEqual(static_cast<uint8_t>(0xFF), get_byte<1>(value));
            Assert::AreEqual(static_cast<uint8_t>(0xFF), get_byte<2>(value));
            Assert::AreEqual(static_cast<uint8_t>(0xFF), get_byte<3>(value));
        }

        TEST_METHOD(Unsigned8BitValueTest)
        {
            uint8_t value = 0;
            Assert::AreEqual(static_cast<uint8_t>(0), get_byte<0>(value));

            value = 0xAA;
            Assert::AreEqual(static_cast<uint8_t>(0xAA), get_byte<0>(value));

            value = 0x11;
            Assert::AreEqual(static_cast<uint8_t>(0x11), get_byte<0>(value));

            value = 0xFF;
            Assert::AreEqual(static_cast<uint8_t>(0xFF), get_byte<0>(value));
        }

        TEST_METHOD(Signed8BitValueTest)
        {
            int8_t value = 0;
            Assert::AreEqual(static_cast<uint8_t>(0), get_byte<0>(value));

            value = static_cast<uint8_t>(0xAA);
            Assert::AreEqual(static_cast<uint8_t>(0xAA), get_byte<0>(value));

            value = 0x11;
            Assert::AreEqual(static_cast<uint8_t>(0x11), get_byte<0>(value));

            value = static_cast<uint8_t>(0xFF);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), get_byte<0>(value));
        }
    };
}

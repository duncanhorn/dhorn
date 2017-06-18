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
}

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



    TEST_CLASS(MakeReverseIntegerSequenceTests)
    {
        TEST_METHOD(MakeEmptySequenceTest)
        {
            Assert::IsTrue(std::is_same_v<std::integer_sequence<int>, make_reverse_integer_sequence<int, 0>>);
            Assert::IsTrue(std::is_same_v<std::index_sequence<>, make_reverse_index_sequence<0>>);
        }

        TEST_METHOD(MakeSingleElementSequenceTest)
        {
            Assert::IsTrue(std::is_same_v<std::integer_sequence<int, 0>, make_reverse_integer_sequence<int, 1>>);
            Assert::IsTrue(std::is_same_v<std::index_sequence<0>, make_reverse_index_sequence<1>>);
        }

        TEST_METHOD(MakeMultipleElementSequenceTest)
        {
            Assert::IsTrue(std::is_same_v<
                std::integer_sequence<int, 5, 4, 3, 2, 1, 0>,
                make_reverse_integer_sequence<int, 6>>);
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<5, 4, 3, 2, 1, 0>,
                make_reverse_index_sequence<6>>);
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

            Assert::IsTrue(std::is_same_v<
                std::index_sequence<8>,
                reverse_integer_sequence_t<std::index_sequence<8>>>);
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

            Assert::IsTrue(std::is_same_v<
                std::index_sequence<8, 42, 99, 0>,
                reverse_integer_sequence_t<std::index_sequence<0, 99, 42, 8>>>);
        }
    };



    TEST_CLASS(IncrementDecrementIntegerSequenceTests)
    {
        TEST_METHOD(IncrementEmptySequenceTest)
        {
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<>,
                increment_integer_sequence_t<std::index_sequence<>, 0>>);
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<>,
                increment_integer_sequence_t<std::index_sequence<>, 42>>);
        }

        TEST_METHOD(DecrementEmptySequenceTest)
        {
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<>,
                decrement_integer_sequence_t<std::index_sequence<>, 0>>);
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<>,
                decrement_integer_sequence_t<std::index_sequence<>, 42>>);
        }

        TEST_METHOD(IncrementSingleElementSequenceTest)
        {
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<42>,
                increment_integer_sequence_t<std::index_sequence<42>, 0>>);
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<42>,
                increment_integer_sequence_t<std::index_sequence<0>, 42>>);
        }

        TEST_METHOD(DecrementSingleElementSequenceTest)
        {
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<42>,
                decrement_integer_sequence_t<std::index_sequence<42>, 0>>);
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<0>,
                decrement_integer_sequence_t<std::index_sequence<42>, 42>>);
        }

        TEST_METHOD(IncrementMultipleElementsSequenceTest)
        {
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<10, 11, 12, 13, 14>,
                increment_integer_sequence_t<std::make_index_sequence<5>, 10>>);
            Assert::IsTrue(std::is_same_v<
                std::integer_sequence<int, 0, 1, 2, 3, 4>,
                increment_integer_sequence_t<std::integer_sequence<int, 10, 11, 12, 13, 14>, -10>>);
        }

        TEST_METHOD(DecrementMultipleElementsSequenceTest)
        {
            Assert::IsTrue(std::is_same_v<
                std::index_sequence<0, 1, 2, 3, 4>,
                decrement_integer_sequence_t<std::index_sequence<10, 11, 12, 13, 14>, 10>>);
            Assert::IsTrue(std::is_same_v<
                std::integer_sequence<int, -10, -9, -8, -7, -6>,
                decrement_integer_sequence_t<std::make_integer_sequence<int, 5>, 10>>);
        }
    };



    TEST_CLASS(GetByteTests)
    {
        TEST_METHOD(Unsigned32BitValueTest)
        {
            std::uint32_t value = 0;
            Assert::AreEqual(static_cast<std::uint8_t>(0), get_byte<0>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0), get_byte<1>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0), get_byte<2>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0), get_byte<3>(value));

            value = 0xAABBCCDD;
            Assert::AreEqual(static_cast<std::uint8_t>(0xDD), get_byte<0>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0xCC), get_byte<1>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0xBB), get_byte<2>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0xAA), get_byte<3>(value));

            value = 0x11223344;
            Assert::AreEqual(static_cast<std::uint8_t>(0x44), get_byte<0>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0x33), get_byte<1>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0x22), get_byte<2>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0x11), get_byte<3>(value));

            value = 0xFFFFFFFF;
            Assert::AreEqual(static_cast<std::uint8_t>(0xFF), get_byte<0>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0xFF), get_byte<1>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0xFF), get_byte<2>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0xFF), get_byte<3>(value));
        }

        TEST_METHOD(Signed32BitValueTest)
        {
            std::uint32_t value = 0;
            Assert::AreEqual(static_cast<std::uint8_t>(0), get_byte<0>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0), get_byte<1>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0), get_byte<2>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0), get_byte<3>(value));

            value = 0xAABBCCDD;
            Assert::AreEqual(static_cast<std::uint8_t>(0xDD), get_byte<0>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0xCC), get_byte<1>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0xBB), get_byte<2>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0xAA), get_byte<3>(value));

            value = 0x11223344;
            Assert::AreEqual(static_cast<std::uint8_t>(0x44), get_byte<0>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0x33), get_byte<1>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0x22), get_byte<2>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0x11), get_byte<3>(value));

            value = 0xFFFFFFFF;
            Assert::AreEqual(static_cast<std::uint8_t>(0xFF), get_byte<0>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0xFF), get_byte<1>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0xFF), get_byte<2>(value));
            Assert::AreEqual(static_cast<std::uint8_t>(0xFF), get_byte<3>(value));
        }

        TEST_METHOD(Unsigned8BitValueTest)
        {
            std::uint8_t value = 0;
            Assert::AreEqual(static_cast<std::uint8_t>(0), get_byte<0>(value));

            value = 0xAA;
            Assert::AreEqual(static_cast<std::uint8_t>(0xAA), get_byte<0>(value));

            value = 0x11;
            Assert::AreEqual(static_cast<std::uint8_t>(0x11), get_byte<0>(value));

            value = 0xFF;
            Assert::AreEqual(static_cast<std::uint8_t>(0xFF), get_byte<0>(value));
        }

        TEST_METHOD(Signed8BitValueTest)
        {
            std::int8_t value = 0;
            Assert::AreEqual(static_cast<std::uint8_t>(0), get_byte<0>(value));

            value = static_cast<std::uint8_t>(0xAA);
            Assert::AreEqual(static_cast<std::uint8_t>(0xAA), get_byte<0>(value));

            value = 0x11;
            Assert::AreEqual(static_cast<std::uint8_t>(0x11), get_byte<0>(value));

            value = static_cast<std::uint8_t>(0xFF);
            Assert::AreEqual(static_cast<std::uint8_t>(0xFF), get_byte<0>(value));
        }
    };



    TEST_CLASS(VariantIndexTests)
    {
        TEST_METHOD(CompilationErrorsTest)
        {
            // NOTE: Compilation test only; each should fail
            //variant_index_v<int, std::variant<>>;
            //variant_index_v<int, std::variant<int&>>;
        }

        TEST_METHOD(SingleTypeTest)
        {
            Assert::AreEqual(static_cast<std::size_t>(0), variant_index_v<int, std::variant<int>>);
        }

        TEST_METHOD(MultipleTypeTest)
        {
            using variant_type = std::variant<int, float, double, std::string, char, unsigned char>;

            Assert::AreEqual(static_cast<std::size_t>(0), variant_index_v<int, variant_type>);
            Assert::AreEqual(static_cast<std::size_t>(1), variant_index_v<float, variant_type>);
            Assert::AreEqual(static_cast<std::size_t>(2), variant_index_v<double, variant_type>);
            Assert::AreEqual(static_cast<std::size_t>(3), variant_index_v<std::string, variant_type>);
            Assert::AreEqual(static_cast<std::size_t>(4), variant_index_v<char, variant_type>);
            Assert::AreEqual(static_cast<std::size_t>(5), variant_index_v<unsigned char, variant_type>);
        }

        TEST_METHOD(WithReferencesTest)
        {
            using variant_type = std::variant<int, int&, const int&, volatile int, const volatile int&>;

            Assert::AreEqual(static_cast<std::size_t>(0), variant_index_v<int, variant_type>);
            Assert::AreEqual(static_cast<std::size_t>(1), variant_index_v<int&, variant_type>);
            Assert::AreEqual(static_cast<std::size_t>(2), variant_index_v<const int&, variant_type>);
            Assert::AreEqual(static_cast<std::size_t>(3), variant_index_v<volatile int, variant_type>);
            Assert::AreEqual(static_cast<std::size_t>(4), variant_index_v<const volatile int&, variant_type>);
        }
    };



#define CONSTEXPR_ASSERT_TRUE(expr) \
{ \
    constexpr bool result = (expr); \
    Assert::IsTrue(result); \
}

#define CONSTEXPR_ASSERT_FALSE(expr) \
{ \
    constexpr bool result = (expr); \
    Assert::IsFalse(result); \
}

    TEST_CLASS(AnyOfTests)
    {
        TEST_METHOD(CompareWithEmptyTest)
        {
            CONSTEXPR_ASSERT_FALSE(0 == any_of());
            CONSTEXPR_ASSERT_FALSE(any_of() == 0);
            CONSTEXPR_ASSERT_FALSE(0 != any_of());
            CONSTEXPR_ASSERT_FALSE(any_of() != 0);
            CONSTEXPR_ASSERT_FALSE(0 < any_of());
            CONSTEXPR_ASSERT_FALSE(any_of() < 0);
            CONSTEXPR_ASSERT_FALSE(0 > any_of());
            CONSTEXPR_ASSERT_FALSE(any_of() > 0);
            CONSTEXPR_ASSERT_FALSE(0 <= any_of());
            CONSTEXPR_ASSERT_FALSE(any_of() <= 0);
            CONSTEXPR_ASSERT_FALSE(0 >= any_of());
            CONSTEXPR_ASSERT_FALSE(any_of() >= 0);
        }

        TEST_METHOD(CompareWithSingleValueTest)
        {
            CONSTEXPR_ASSERT_FALSE(0 == any_of(1));
            CONSTEXPR_ASSERT_FALSE(any_of(1) == 0);
            CONSTEXPR_ASSERT_TRUE(0 == any_of(0));
            CONSTEXPR_ASSERT_TRUE(any_of(0) == 0);

            CONSTEXPR_ASSERT_TRUE(0 != any_of(1));
            CONSTEXPR_ASSERT_TRUE(any_of(1) != 0);
            CONSTEXPR_ASSERT_FALSE(0 != any_of(0));
            CONSTEXPR_ASSERT_FALSE(any_of(0) != 0);

            CONSTEXPR_ASSERT_TRUE(0 < any_of(1));
            CONSTEXPR_ASSERT_FALSE(any_of(1) < 0);
            CONSTEXPR_ASSERT_FALSE(0 < any_of(0));
            CONSTEXPR_ASSERT_FALSE(any_of(0) < 0);

            CONSTEXPR_ASSERT_FALSE(0 > any_of(1));
            CONSTEXPR_ASSERT_TRUE(any_of(1) > 0);
            CONSTEXPR_ASSERT_FALSE(0 > any_of(0));
            CONSTEXPR_ASSERT_FALSE(any_of(0) > 0);

            CONSTEXPR_ASSERT_TRUE(0 <= any_of(1));
            CONSTEXPR_ASSERT_FALSE(any_of(1) <= 0);
            CONSTEXPR_ASSERT_TRUE(0 <= any_of(0));
            CONSTEXPR_ASSERT_TRUE(any_of(0) <= 0);

            CONSTEXPR_ASSERT_FALSE(0 >= any_of(1));
            CONSTEXPR_ASSERT_TRUE(any_of(1) >= 0);
            CONSTEXPR_ASSERT_TRUE(0 >= any_of(0));
            CONSTEXPR_ASSERT_TRUE(any_of(0) >= 0);
        }

        TEST_METHOD(CompareWithMultipleValuesTest)
        {
            CONSTEXPR_ASSERT_FALSE(0 == any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(any_of(1, 2, 3, 4, 5) == 0);
            CONSTEXPR_ASSERT_TRUE(1 == any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) == 1);
            CONSTEXPR_ASSERT_TRUE(2 == any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) == 2);
            CONSTEXPR_ASSERT_TRUE(3 == any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) == 3);
            CONSTEXPR_ASSERT_TRUE(4 == any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) == 4);
            CONSTEXPR_ASSERT_TRUE(5 == any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) == 5);
            CONSTEXPR_ASSERT_FALSE(6 == any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(any_of(1, 2, 3, 4, 5) == 6);

            CONSTEXPR_ASSERT_TRUE(0 != any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) != 0);
            CONSTEXPR_ASSERT_TRUE(1 != any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) != 1);
            CONSTEXPR_ASSERT_TRUE(2 != any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) != 2);
            CONSTEXPR_ASSERT_TRUE(3 != any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) != 3);
            CONSTEXPR_ASSERT_TRUE(4 != any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) != 4);
            CONSTEXPR_ASSERT_TRUE(5 != any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) != 5);
            CONSTEXPR_ASSERT_TRUE(6 != any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) != 6);

            CONSTEXPR_ASSERT_TRUE(0 < any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(any_of(1, 2, 3, 4, 5) < 0);
            CONSTEXPR_ASSERT_TRUE(1 < any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(any_of(1, 2, 3, 4, 5) < 1);
            CONSTEXPR_ASSERT_TRUE(2 < any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) < 2);
            CONSTEXPR_ASSERT_TRUE(3 < any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) < 3);
            CONSTEXPR_ASSERT_TRUE(4 < any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) < 4);
            CONSTEXPR_ASSERT_FALSE(5 < any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) < 5);
            CONSTEXPR_ASSERT_FALSE(6 < any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) < 6);

            CONSTEXPR_ASSERT_FALSE(0 > any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) > 0);
            CONSTEXPR_ASSERT_FALSE(1 > any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) > 1);
            CONSTEXPR_ASSERT_TRUE(2 > any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) > 2);
            CONSTEXPR_ASSERT_TRUE(3 > any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) > 3);
            CONSTEXPR_ASSERT_TRUE(4 > any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) > 4);
            CONSTEXPR_ASSERT_TRUE(5 > any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(any_of(1, 2, 3, 4, 5) > 5);
            CONSTEXPR_ASSERT_TRUE(6 > any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(any_of(1, 2, 3, 4, 5) > 6);

            CONSTEXPR_ASSERT_TRUE(0 <= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(any_of(1, 2, 3, 4, 5) <= 0);
            CONSTEXPR_ASSERT_TRUE(1 <= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) <= 1);
            CONSTEXPR_ASSERT_TRUE(2 <= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) <= 2);
            CONSTEXPR_ASSERT_TRUE(3 <= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) <= 3);
            CONSTEXPR_ASSERT_TRUE(4 <= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) <= 4);
            CONSTEXPR_ASSERT_TRUE(5 <= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) <= 5);
            CONSTEXPR_ASSERT_FALSE(6 <= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) <= 6);

            CONSTEXPR_ASSERT_FALSE(0 >= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) >= 0);
            CONSTEXPR_ASSERT_TRUE(1 >= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) >= 1);
            CONSTEXPR_ASSERT_TRUE(2 >= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) >= 2);
            CONSTEXPR_ASSERT_TRUE(3 >= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) >= 3);
            CONSTEXPR_ASSERT_TRUE(4 >= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) >= 4);
            CONSTEXPR_ASSERT_TRUE(5 >= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(any_of(1, 2, 3, 4, 5) >= 5);
            CONSTEXPR_ASSERT_TRUE(6 >= any_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(any_of(1, 2, 3, 4, 5) >= 6);
        }
    };



    TEST_CLASS(AllOfTests)
    {
        TEST_METHOD(CompareWithEmptyTest)
        {
            CONSTEXPR_ASSERT_TRUE(0 == all_of());
            CONSTEXPR_ASSERT_TRUE(all_of() == 0);
            CONSTEXPR_ASSERT_TRUE(0 != all_of());
            CONSTEXPR_ASSERT_TRUE(all_of() != 0);
            CONSTEXPR_ASSERT_TRUE(0 < all_of());
            CONSTEXPR_ASSERT_TRUE(all_of() < 0);
            CONSTEXPR_ASSERT_TRUE(0 > all_of());
            CONSTEXPR_ASSERT_TRUE(all_of() > 0);
            CONSTEXPR_ASSERT_TRUE(0 <= all_of());
            CONSTEXPR_ASSERT_TRUE(all_of() <= 0);
            CONSTEXPR_ASSERT_TRUE(0 >= all_of());
            CONSTEXPR_ASSERT_TRUE(all_of() >= 0);
        }

        TEST_METHOD(CompareWithSingleValueTest)
        {
            CONSTEXPR_ASSERT_FALSE(0 == all_of(1));
            CONSTEXPR_ASSERT_FALSE(all_of(1) == 0);
            CONSTEXPR_ASSERT_TRUE(0 == all_of(0));
            CONSTEXPR_ASSERT_TRUE(all_of(0) == 0);

            CONSTEXPR_ASSERT_TRUE(0 != all_of(1));
            CONSTEXPR_ASSERT_TRUE(all_of(1) != 0);
            CONSTEXPR_ASSERT_FALSE(0 != all_of(0));
            CONSTEXPR_ASSERT_FALSE(all_of(0) != 0);

            CONSTEXPR_ASSERT_TRUE(0 < all_of(1));
            CONSTEXPR_ASSERT_FALSE(all_of(1) < 0);
            CONSTEXPR_ASSERT_FALSE(0 < all_of(0));
            CONSTEXPR_ASSERT_FALSE(all_of(0) < 0);

            CONSTEXPR_ASSERT_FALSE(0 > all_of(1));
            CONSTEXPR_ASSERT_TRUE(all_of(1) > 0);
            CONSTEXPR_ASSERT_FALSE(0 > all_of(0));
            CONSTEXPR_ASSERT_FALSE(all_of(0) > 0);

            CONSTEXPR_ASSERT_TRUE(0 <= all_of(1));
            CONSTEXPR_ASSERT_FALSE(all_of(1) <= 0);
            CONSTEXPR_ASSERT_TRUE(0 <= all_of(0));
            CONSTEXPR_ASSERT_TRUE(all_of(0) <= 0);

            CONSTEXPR_ASSERT_FALSE(0 >= all_of(1));
            CONSTEXPR_ASSERT_TRUE(all_of(1) >= 0);
            CONSTEXPR_ASSERT_TRUE(0 >= all_of(0));
            CONSTEXPR_ASSERT_TRUE(all_of(0) >= 0);
        }

        TEST_METHOD(CompareWithMultipleValuesTest)
        {
            CONSTEXPR_ASSERT_FALSE(0 == all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) == 0);
            CONSTEXPR_ASSERT_FALSE(1 == all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) == 1);
            CONSTEXPR_ASSERT_FALSE(2 == all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) == 2);
            CONSTEXPR_ASSERT_FALSE(3 == all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) == 3);
            CONSTEXPR_ASSERT_FALSE(4 == all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) == 4);
            CONSTEXPR_ASSERT_FALSE(5 == all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) == 5);
            CONSTEXPR_ASSERT_FALSE(6 == all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) == 6);
            CONSTEXPR_ASSERT_TRUE(0 == all_of(0, 0, 0, 0, 0));
            CONSTEXPR_ASSERT_TRUE(all_of(0, 0, 0, 0, 0) == 0);

            CONSTEXPR_ASSERT_TRUE(0 != all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(all_of(1, 2, 3, 4, 5) != 0);
            CONSTEXPR_ASSERT_FALSE(1 != all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) != 1);
            CONSTEXPR_ASSERT_FALSE(2 != all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) != 2);
            CONSTEXPR_ASSERT_FALSE(3 != all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) != 3);
            CONSTEXPR_ASSERT_FALSE(4 != all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) != 4);
            CONSTEXPR_ASSERT_FALSE(5 != all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) != 5);
            CONSTEXPR_ASSERT_TRUE(6 != all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(all_of(1, 2, 3, 4, 5) != 6);

            CONSTEXPR_ASSERT_TRUE(0 < all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) < 0);
            CONSTEXPR_ASSERT_FALSE(1 < all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) < 1);
            CONSTEXPR_ASSERT_FALSE(2 < all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) < 2);
            CONSTEXPR_ASSERT_FALSE(3 < all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) < 3);
            CONSTEXPR_ASSERT_FALSE(4 < all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) < 4);
            CONSTEXPR_ASSERT_FALSE(5 < all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) < 5);
            CONSTEXPR_ASSERT_FALSE(6 < all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(all_of(1, 2, 3, 4, 5) < 6);

            CONSTEXPR_ASSERT_FALSE(0 > all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(all_of(1, 2, 3, 4, 5) > 0);
            CONSTEXPR_ASSERT_FALSE(1 > all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) > 1);
            CONSTEXPR_ASSERT_FALSE(2 > all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) > 2);
            CONSTEXPR_ASSERT_FALSE(3 > all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) > 3);
            CONSTEXPR_ASSERT_FALSE(4 > all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) > 4);
            CONSTEXPR_ASSERT_FALSE(5 > all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) > 5);
            CONSTEXPR_ASSERT_TRUE(6 > all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) > 6);

            CONSTEXPR_ASSERT_TRUE(0 <= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) <= 0);
            CONSTEXPR_ASSERT_TRUE(1 <= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) <= 1);
            CONSTEXPR_ASSERT_FALSE(2 <= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) <= 2);
            CONSTEXPR_ASSERT_FALSE(3 <= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) <= 3);
            CONSTEXPR_ASSERT_FALSE(4 <= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) <= 4);
            CONSTEXPR_ASSERT_FALSE(5 <= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(all_of(1, 2, 3, 4, 5) <= 5);
            CONSTEXPR_ASSERT_FALSE(6 <= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(all_of(1, 2, 3, 4, 5) <= 6);

            CONSTEXPR_ASSERT_FALSE(0 >= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(all_of(1, 2, 3, 4, 5) >= 0);
            CONSTEXPR_ASSERT_FALSE(1 >= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_TRUE(all_of(1, 2, 3, 4, 5) >= 1);
            CONSTEXPR_ASSERT_FALSE(2 >= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) >= 2);
            CONSTEXPR_ASSERT_FALSE(3 >= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) >= 3);
            CONSTEXPR_ASSERT_FALSE(4 >= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) >= 4);
            CONSTEXPR_ASSERT_TRUE(5 >= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) >= 5);
            CONSTEXPR_ASSERT_TRUE(6 >= all_of(1, 2, 3, 4, 5));
            CONSTEXPR_ASSERT_FALSE(all_of(1, 2, 3, 4, 5) >= 6);
        }
    };
}

/*
 * Duncan Horn
 *
 * CompressedPairTests.cpp
 *
 * Tests for the compressed_pair type
 */
#include "stdafx.h"

#include <dhorn/compressed_pair.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(CompressedPairTests)
    {
#pragma region Test Types

        struct empty
        {
        };

        struct empty_explicit
        {
            explicit empty_explicit() {}
            explicit empty_explicit(const empty_explicit&) {}
            explicit empty_explicit(empty_explicit&&) {}
        };

        struct empty_final final
        {
        };

        struct empty_final_explicit final
        {
            explicit empty_final_explicit() {}
            explicit empty_final_explicit(const empty_final_explicit&) {}
            explicit empty_final_explicit(empty_final_explicit&&) {}
        };

        struct non_empty
        {
            int value = 42;
        };

        struct non_empty_explicit
        {
            explicit non_empty_explicit() {}
            explicit non_empty_explicit(const non_empty_explicit& o) : value(o.value) {}
            explicit non_empty_explicit(non_empty_explicit&& o) : value(o.value) {}

            int value = 42;
        };

#pragma endregion



#pragma region Size Tests

        template <typename T1, typename T2, bool EmptyOptimization>
        void DoSizeTest()
        {
            // Since we may not be able to derive from each type (e.g. final), derive from a type with them as members
            struct adapt_1 { T1 _1; };
            struct adapt_2 { T2 _2; };
            struct adapt_both { T1 _1; T2 _2; };

            // If we are expecting the empty non-final optimization, we only derive from one of the adapter types (the
            // larger of the two, if they are different)
            struct dummy : std::conditional_t<EmptyOptimization,
                std::conditional_t<sizeof(T1) < sizeof(T2), adapt_2, adapt_1>,
                adapt_both>
            {
            };

            Assert::AreEqual(sizeof(dummy), sizeof(compressed_pair<T1, T2>));
        }

        TEST_METHOD(EmptySizeTest)
        {
            DoSizeTest<empty, empty, true>();
            DoSizeTest<empty, empty_explicit, true>();
            DoSizeTest<empty_explicit, empty, true>();
            DoSizeTest<empty_explicit, empty_explicit, true>();

            DoSizeTest<empty, non_empty, true>();
            DoSizeTest<non_empty, empty, true>();
        }

        TEST_METHOD(NonEmptySizeTest)
        {
            DoSizeTest<non_empty, non_empty, false>();
            DoSizeTest<non_empty, non_empty_explicit, false>();
            DoSizeTest<non_empty_explicit, non_empty, false>();
            DoSizeTest<non_empty_explicit, non_empty_explicit, false>();
        }

        TEST_METHOD(FinalSizeTest)
        {
            // Two non-empty types has a size of the sum of the two (plus potential padding,  but since we only use
            // ints, there won't be any padding)
            DoSizeTest<empty_final, empty_final, false>();
            DoSizeTest<empty_final, empty_final_explicit, false>();
            DoSizeTest<empty_final_explicit, empty_final, false>();
            DoSizeTest<empty_final_explicit, empty_final_explicit, false>();
        }

        TEST_METHOD(StandardTypesTest)
        {
            DoSizeTest<int, int, false>();
            DoSizeTest<int, float, false>();
            DoSizeTest<float, int, false>();
            DoSizeTest<float, float, false>();
        }

#pragma endregion



#pragma region Construction Tests

#pragma region Default Construction

        TEST_METHOD(CompressedDefaultConstructionTest)
        {

        }

        TEST_METHOD(CompressedExplicitDefaultConstructionTest)
        {

        }

        TEST_METHOD(NonCompressedDefaultConstructionTest)
        {
            Assert::IsTrue(is_implicitly_default_constructible_v<compressed_pair<non_empty, non_empty>>);
        }

        TEST_METHOD(NonCompressedExplicitDefaultConstructionTest)
        {
            Assert::IsFalse(is_implicitly_default_constructible_v<compressed_pair<non_empty_explicit, non_empty_explicit>>);
        }

#pragma endregion

#pragma endregion
    };
}

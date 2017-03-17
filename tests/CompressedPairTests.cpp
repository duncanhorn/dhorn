/*
 * Duncan Horn
 *
 * CompressedPairTests.cpp
 *
 * Tests for the compressed_pair type
 */
#include "stdafx.h"

#include <dhorn/compressed_pair.h>

#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::literals;

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

        template <typename TestTy>
        static void DoNonExplicitConstructionTest(const TestTy& test)
        {
            test.template run<empty, empty, true>();
            test.template run<empty, empty_final, true>();
            test.template run<empty, non_empty, true>();

            test.template run<empty_final, empty_final, true>();
            test.template run<empty_final, non_empty, true>();

            test.template run<non_empty, non_empty, true>();
        }

        template <typename TestTy>
        static void DoExplicitConstructionTest(const TestTy& test)
        {
            test.template run<empty_explicit, empty, false>();
            test.template run<empty_explicit, empty_explicit, false>();
            test.template run<empty_explicit, empty_final, false>();
            test.template run<empty_explicit, empty_final_explicit, false>();
            test.template run<empty_explicit, non_empty, false>();
            test.template run<empty_explicit, non_empty_explicit, false>();

            test.template run<empty_final_explicit, empty, false>();
            test.template run<empty_final_explicit, empty_final, false>();
            test.template run<empty_final_explicit, empty_final_explicit, false>();
            test.template run<empty_final_explicit, non_empty, false>();
            test.template run<empty_final_explicit, non_empty_explicit, false>();

            test.template run<non_empty_explicit, empty, false>();
            test.template run<non_empty_explicit, empty_final, false>();
            test.template run<non_empty_explicit, non_empty, false>();
            test.template run<non_empty_explicit, non_empty_explicit, false>();
        }

#pragma region Default Construction

        struct DefaultConstructionTester
        {
            template <typename T1, typename T2, bool Expect>
            void run() const
            {
                Assert::AreEqual(Expect, is_implicitly_default_constructible_v<compressed_pair<T1, T2>>);
                Assert::AreEqual(Expect, is_implicitly_default_constructible_v<compressed_pair<T2, T1>>);
            }
        };

        TEST_METHOD(DefaultConstructionTest)
        {
            DoNonExplicitConstructionTest(DefaultConstructionTester{});
        }

        TEST_METHOD(ExplicitDefaultConstructionTest)
        {
            DoExplicitConstructionTest(DefaultConstructionTester{});
        }

#pragma endregion

#pragma region Value Copy Construction

        struct ValueConstructionTester
        {
            template <typename T1, typename T2, bool Expect>
            void run() const
            {
                Assert::AreEqual(Expect, is_implicitly_constructible_v<compressed_pair<T1, T2>, const T1&, const T2&>);
                Assert::AreEqual(Expect, is_implicitly_constructible_v<compressed_pair<T2, T1>, const T2&, const T1&>);
            }
        };

        TEST_METHOD(ValueConstructionTest)
        {
            DoNonExplicitConstructionTest(ValueConstructionTester{});
        }

        TEST_METHOD(ExplicitValueConstructionTest)
        {
            DoExplicitConstructionTest(ValueConstructionTester{});
        }

#pragma endregion

#pragma region Value Copy Construction

        struct ValueMoveConstructionTester
        {
            template <typename T1, typename T2, bool Expect>
            void run() const
            {
                Assert::AreEqual(Expect, is_implicitly_constructible_v<compressed_pair<T1, T2>, T1&&, T2&&>);
                Assert::AreEqual(Expect, is_implicitly_constructible_v<compressed_pair<T2, T1>, T2&&, T1&&>);
            }
        };

        TEST_METHOD(ValueMoveConstructionTest)
        {
            DoNonExplicitConstructionTest(ValueMoveConstructionTester{});
        }

        TEST_METHOD(ExplicitValueMoveConstructionTest)
        {
            DoExplicitConstructionTest(ValueMoveConstructionTester{});
        }

#pragma endregion

#pragma region Piecewise Construction

        TEST_METHOD(PiecewiseConstructionTest)
        {
            // First is empty
            dhorn::compressed_pair<empty, int> p1(
                std::piecewise_construct,
                std::forward_as_tuple(),
                std::forward_as_tuple(42));
            Assert::AreEqual(42, p1.second());

            // Second is empty
            dhorn::compressed_pair<int, empty_explicit> p2(
                std::piecewise_construct,
                std::forward_as_tuple(42),
                std::forward_as_tuple());
            Assert::AreEqual(42, p2.first());

            // Neither are empty
            dhorn::compressed_pair<std::string, std::vector<int>> p3(
                std::piecewise_construct,
                std::forward_as_tuple("foo"),
                std::forward_as_tuple(8));
            Assert::AreEqual("foo"s, p3.first());
            Assert::AreEqual(8u, p3.second().size());
        }

#pragma endregion

        /* TODO: Default copy/move seems broken :(
#pragma region Copy Construction

        struct CopyConstructionTester
        {
            template <typename T1, typename T2, bool Expect>
            void run() const
            {
                Assert::AreEqual(Expect, is_implicitly_constructible_v<compressed_pair<T1, T2>, const compressed_pair<T1, T2>&>);
                Assert::AreEqual(Expect, is_implicitly_constructible_v<compressed_pair<T2, T1>, const compressed_pair<T2, T1>&>);
            }
        };

        TEST_METHOD(CopyConstructionTest)
        {
            DoNonExplicitConstructionTest(CopyConstructionTester{});
        }

        TEST_METHOD(ExplicitCopyConstructionTest)
        {
            DoExplicitConstructionTest(CopyConstructionTester{});
        }

#pragma endregion

#pragma region Move Construction

        struct MoveConstructionTester
        {
            template <typename T1, typename T2, bool Expect>
            void run() const
            {
                Assert::AreEqual(Expect, is_implicitly_constructible_v<compressed_pair<T1, T2>, compressed_pair<T1, T2>&&>);
                Assert::AreEqual(Expect, is_implicitly_constructible_v<compressed_pair<T2, T1>, compressed_pair<T2, T1>&&>);
            }
        };

        TEST_METHOD(MoveConstructionTest)
        {
            DoNonExplicitConstructionTest(MoveConstructionTester{});
        }

        TEST_METHOD(ExplicitMoveConstructionTest)
        {
            DoExplicitConstructionTest(MoveConstructionTester{});
        }

#pragma endregion
        */

#pragma endregion
    };
}

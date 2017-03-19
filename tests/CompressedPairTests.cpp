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

#pragma region Empty Types

        struct empty
        {
        };

        struct empty_explicit
        {
            explicit empty_explicit() {}
            explicit empty_explicit(const empty_explicit&) {}
            explicit empty_explicit(empty_explicit&&) {}
        };

        struct can_copy_empty
        {
        };

        struct cannot_copy_empty
        {
            cannot_copy_empty(const cannot_copy_empty&) = delete;
            cannot_copy_empty& operator=(const cannot_copy_empty&) = delete;
        };

#pragma endregion



#pragma region Final Types

        struct empty_final final {};

        struct empty_final_explicit final
        {
            explicit empty_final_explicit() {}
            explicit empty_final_explicit(const empty_final_explicit&) {}
            explicit empty_final_explicit(empty_final_explicit&&) {}
        };

        struct can_copy_final final
        {
        };

        struct cannot_copy_final final
        {
            cannot_copy_final(const cannot_copy_final&) = delete;
            cannot_copy_final& operator=(const cannot_copy_final&) = delete;
        };

#pragma endregion



#pragma region Non-Empty Types

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

        struct can_copy_non_empty
        {
            can_copy_non_empty() = default;
            can_copy_non_empty(int value) : value(value) {}
            int value = 42;
        };

        struct cannot_copy_non_empty
        {
            cannot_copy_non_empty(const cannot_copy_non_empty&) = delete;
            cannot_copy_non_empty& operator=(const cannot_copy_non_empty&) = delete;
            int value = 42;
        };

#pragma endregion



#pragma region Adapters

        template <typename Ty>
        struct empty_adapter
        {
            empty_adapter(const Ty&) {}
            empty_adapter(Ty&&) {}
        };

        template <typename Ty>
        struct empty_explicit_adapter
        {
            explicit empty_explicit_adapter(const Ty&) {}
            explicit empty_explicit_adapter(Ty&&) {}
        };

        template <typename Ty>
        struct empty_final_adapter final
        {
            empty_final_adapter(const Ty&) {}
            empty_final_adapter(Ty&&) {}
        };

        template <typename Ty>
        struct empty_final_explicit_adapter final
        {
            explicit empty_final_explicit_adapter(const Ty&) {}
            explicit empty_final_explicit_adapter(Ty&&) {}
        };

        template <typename Ty>
        struct non_empty_adapter
        {
            non_empty_adapter(const Ty&) {}
            non_empty_adapter(Ty&&) {}

            int value = 42;
        };

        template <typename Ty>
        struct non_empty_explicit_adapter
        {
            explicit non_empty_explicit_adapter(const Ty&) {}
            explicit non_empty_explicit_adapter(Ty&&) {}

            int value = 42;
        };

#pragma endregion

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

        template <typename T1, typename T2, typename TestTy>
        static void DoNonExplicitCopyMoveConstructionTest(const TestTy& test)
        {
            test.template run<empty_adapter<T1>, empty_adapter<int>, T1, T2, true>();
            test.template run<empty_adapter<T1>, empty_final_adapter<int>, T1, T2, true>();
            test.template run<empty_adapter<T1>, non_empty_adapter<int>, T1, T2, true>();

            test.template run<empty_final_adapter<T1>, empty_final_adapter<int>, T1, T2, true>();
            test.template run<empty_final_adapter<T1>, non_empty_adapter<int>, T1, T2, true>();

            test.template run<non_empty_adapter<T1>, non_empty_adapter<int>, T1, T2, true>();
        }

        template <typename T1, typename T2, typename TestTy>
        static void DoExplicitCopyMoveConstructionTest(const TestTy& test)
        {
            test.template run<empty_explicit_adapter<T1>, empty_adapter<T2>, T1, T2, false>();
            test.template run<empty_explicit_adapter<T1>, empty_explicit_adapter<T2>, T1, T2, false>();
            test.template run<empty_explicit_adapter<T1>, empty_final_adapter<T2>, T1, T2, false>();
            test.template run<empty_explicit_adapter<T1>, empty_final_explicit_adapter<T2>, T1, T2, false>();
            test.template run<empty_explicit_adapter<T1>, non_empty_adapter<T2>, T1, T2, false>();
            test.template run<empty_explicit_adapter<T1>, non_empty_explicit_adapter<T2>, T1, T2, false>();

            test.template run<empty_final_explicit_adapter<T1>, empty_adapter<T2>, T1, T2, false>();
            test.template run<empty_final_explicit_adapter<T1>, empty_final_adapter<T2>, T1, T2, false>();
            test.template run<empty_final_explicit_adapter<T1>, empty_final_explicit_adapter<T2>, T1, T2, false>();
            test.template run<empty_final_explicit_adapter<T1>, non_empty_adapter<T2>, T1, T2, false>();
            test.template run<empty_final_explicit_adapter<T1>, non_empty_explicit_adapter<T2>, T1, T2, false>();

            test.template run<non_empty_explicit_adapter<T1>, empty_adapter<T2>, T1, T2, false>();
            test.template run<non_empty_explicit_adapter<T1>, empty_final_adapter<T2>, T1, T2, false>();
            test.template run<non_empty_explicit_adapter<T1>, non_empty_adapter<T2>, T1, T2, false>();
            test.template run<non_empty_explicit_adapter<T1>, non_empty_explicit_adapter<T2>, T1, T2, false>();
        }

#pragma region Default Construction

        struct DefaultConstructionTester
        {
            template <typename T1, typename T2, bool Expect>
            void run() const
            {
                Assert::AreEqual(Expect, is_implicitly_default_constructible_v<compressed_pair<T1, T2>>);
                Assert::AreEqual(Expect, is_implicitly_default_constructible_v<compressed_pair<T2, T1>>);

                Assert::IsTrue(std::is_default_constructible_v<compressed_pair<T1, T2>>);
                Assert::IsTrue(std::is_default_constructible_v<compressed_pair<T2, T1>>);
            }
        };

        TEST_METHOD(DefaultConstructionTest)
        {
            DoNonExplicitConstructionTest(DefaultConstructionTester{});

            // TODO: Intellisense
            dhorn::compressed_pair<non_empty, non_empty> p1 = {};
            Assert::AreEqual(42, p1.first().value);
            Assert::AreEqual(42, p1.second().value);

            dhorn::compressed_pair<empty, non_empty> p2 = {};
            Assert::AreEqual(42, p1.second().value);

            dhorn::compressed_pair<non_empty, empty> p3 = {};
            Assert::AreEqual(42, p1.first().value);
        }

        TEST_METHOD(ExplicitDefaultConstructionTest)
        {
            DoExplicitConstructionTest(DefaultConstructionTester{});

            // TODO: Intellisense
            dhorn::compressed_pair<non_empty_explicit, non_empty_explicit> p1;
            Assert::AreEqual(42, p1.first().value);
            Assert::AreEqual(42, p1.second().value);

            dhorn::compressed_pair<empty_explicit, non_empty> p2;
            Assert::AreEqual(42, p1.second().value);

            dhorn::compressed_pair<non_empty, empty_explicit> p3;
            Assert::AreEqual(42, p1.first().value);
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

                Assert::IsTrue(std::is_constructible_v<compressed_pair<T1, T2>, const T1&, const T2&>);
                Assert::IsTrue(std::is_constructible_v<compressed_pair<T2, T1>, const T2&, const T1&>);
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

                Assert::IsTrue(std::is_constructible_v<compressed_pair<T1, T2>, T1&&, T2&&>);
                Assert::IsTrue(std::is_constructible_v<compressed_pair<T2, T1>, T2&&, T1&&>);
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

#pragma region Copy Construction

        struct CopyConstructionTester
        {
            template <typename T1, typename T2, typename From1, typename From2, bool Expect>
            void run() const
            {
                Assert::AreEqual(Expect, is_implicitly_constructible_v<compressed_pair<T1, T2>, const compressed_pair<From1, From2>&>);
                Assert::AreEqual(Expect, is_implicitly_constructible_v<compressed_pair<T2, T1>, const compressed_pair<From2, From1>&>);
            }
        };

        TEST_METHOD(CopyConstructionTest)
        {
            DoNonExplicitCopyMoveConstructionTest<int, int>(CopyConstructionTester{});
        }

        TEST_METHOD(ExplicitCopyConstructionTest)
        {
            DoExplicitCopyMoveConstructionTest<int, int>(CopyConstructionTester{});
        }

#pragma endregion

#pragma region Move Construction

        struct MoveConstructionTester
        {
            template <typename T1, typename T2, typename From1, typename From2, bool Expect>
            void run() const
            {
                Assert::AreEqual(Expect, is_implicitly_constructible_v<compressed_pair<T1, T2>, compressed_pair<From1, From2>&&>);
                Assert::AreEqual(Expect, is_implicitly_constructible_v<compressed_pair<T2, T1>, compressed_pair<From2, From1>&&>);
            }
        };

        TEST_METHOD(MoveConstructionTest)
        {
            DoExplicitCopyMoveConstructionTest<int, int>(MoveConstructionTester{});
        }

        TEST_METHOD(ExplicitMoveConstructionTest)
        {
            DoExplicitCopyMoveConstructionTest<int, int>(MoveConstructionTester{});
        }

#pragma endregion

#pragma endregion



#pragma region Assignment Tests

        template <typename T1, typename T2, bool Expect, typename TestTy>
        static void DoAssignmentTest_Helper(const TestTy& test)
        {
            test.template run<compressed_pair<T1, T2>, compressed_pair<T1, T2>, Expect>();
            test.template run<compressed_pair<T2, T1>, compressed_pair<T2, T1>, Expect>();
        }

        template <typename TestTy>
        static void DoAssignmentTest(const TestTy& test)
        {
            DoAssignmentTest_Helper<can_copy_empty, can_copy_empty, true>(test);
            DoAssignmentTest_Helper<can_copy_empty, can_copy_final, true>(test);
            DoAssignmentTest_Helper<can_copy_empty, can_copy_non_empty, true>(test);
            DoAssignmentTest_Helper<can_copy_empty, cannot_copy_empty, false>(test);
            DoAssignmentTest_Helper<can_copy_empty, cannot_copy_final, false>(test);
            DoAssignmentTest_Helper<can_copy_empty, cannot_copy_non_empty, false>(test);

            DoAssignmentTest_Helper<can_copy_final, can_copy_final, true>(test);
            DoAssignmentTest_Helper<can_copy_final, can_copy_non_empty, true>(test);
            DoAssignmentTest_Helper<can_copy_final, cannot_copy_empty, false>(test);
            DoAssignmentTest_Helper<can_copy_final, cannot_copy_final, false>(test);
            DoAssignmentTest_Helper<can_copy_final, cannot_copy_non_empty, false>(test);

            DoAssignmentTest_Helper<can_copy_non_empty, can_copy_non_empty, true>(test);
            DoAssignmentTest_Helper<can_copy_non_empty, cannot_copy_empty, false>(test);
            DoAssignmentTest_Helper<can_copy_non_empty, cannot_copy_final, false>(test);
            DoAssignmentTest_Helper<can_copy_non_empty, cannot_copy_non_empty, false>(test);

            DoAssignmentTest_Helper<cannot_copy_empty, cannot_copy_empty, false>(test);
            DoAssignmentTest_Helper<cannot_copy_empty, cannot_copy_final, false>(test);
            DoAssignmentTest_Helper<cannot_copy_empty, cannot_copy_non_empty, false>(test);

            DoAssignmentTest_Helper<cannot_copy_final, cannot_copy_final, false>(test);
            DoAssignmentTest_Helper<cannot_copy_final, cannot_copy_non_empty, false>(test);

            DoAssignmentTest_Helper<cannot_copy_non_empty, cannot_copy_non_empty, false>(test);
        }

        template <typename TestTy>
        static void DoConversionAssignmentTest(const TestTy& test)
        {
            // TODO
            test;
        }

        struct CopyAssignmentTester
        {
            template <typename To, typename From, bool Expect>
            void run() const
            {
                Assert::AreEqual(Expect, std::is_assignable_v<To&, const From&>);
            }
        };

        TEST_METHOD(CopyAssignmentTest)
        {
            DoAssignmentTest(CopyAssignmentTester{});
        }

        TEST_METHOD(CopyConversionAssignmentTest)
        {
            DoConversionAssignmentTest(CopyAssignmentTester{});
        }

        struct MoveAssignmentTester
        {
            template <typename To, typename From, bool Expect>
            void run() const
            {
                Assert::AreEqual(Expect, std::is_assignable_v<To&, From&&>);
            }
        };

        TEST_METHOD(MoveAssignmentTest)
        {
            DoAssignmentTest(MoveAssignmentTester{});
        }

        TEST_METHOD(MoveConversionAssignmentTest)
        {
            DoConversionAssignmentTest(MoveAssignmentTester{});
        }

#pragma endregion



#pragma region Accessor Tests



#pragma endregion



#pragma region Modifier Tests

        TEST_METHOD(SwapTest)
        {
            Assert::IsTrue(std::is_swappable_v<compressed_pair<can_copy_empty, can_copy_empty>>);
            Assert::IsTrue(std::is_swappable_v<compressed_pair<can_copy_empty, can_copy_final>>);
            Assert::IsTrue(std::is_swappable_v<compressed_pair<can_copy_empty, can_copy_non_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<can_copy_empty, cannot_copy_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<can_copy_empty, cannot_copy_final>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<can_copy_empty, cannot_copy_non_empty>>);

            Assert::IsTrue(std::is_swappable_v<compressed_pair<can_copy_final, can_copy_empty>>);
            Assert::IsTrue(std::is_swappable_v<compressed_pair<can_copy_final, can_copy_final>>);
            Assert::IsTrue(std::is_swappable_v<compressed_pair<can_copy_final, can_copy_non_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<can_copy_final, cannot_copy_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<can_copy_final, cannot_copy_final>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<can_copy_final, cannot_copy_non_empty>>);

            Assert::IsTrue(std::is_swappable_v<compressed_pair<can_copy_non_empty, can_copy_empty>>);
            Assert::IsTrue(std::is_swappable_v<compressed_pair<can_copy_non_empty, can_copy_final>>);
            Assert::IsTrue(std::is_swappable_v<compressed_pair<can_copy_non_empty, can_copy_non_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<can_copy_non_empty, cannot_copy_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<can_copy_non_empty, cannot_copy_final>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<can_copy_non_empty, cannot_copy_non_empty>>);

            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_empty, can_copy_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_empty, can_copy_final>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_empty, can_copy_non_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_empty, cannot_copy_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_empty, cannot_copy_final>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_empty, cannot_copy_non_empty>>);

            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_final, can_copy_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_final, can_copy_final>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_final, can_copy_non_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_final, cannot_copy_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_final, cannot_copy_final>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_final, cannot_copy_non_empty>>);

            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_non_empty, can_copy_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_non_empty, can_copy_final>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_non_empty, can_copy_non_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_non_empty, cannot_copy_empty>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_non_empty, cannot_copy_final>>);
            Assert::IsFalse(std::is_swappable_v<compressed_pair<cannot_copy_non_empty, cannot_copy_non_empty>>);

            // TODO: Intellisense
            compressed_pair<can_copy_non_empty, can_copy_non_empty> p1(1, 2);
            compressed_pair<can_copy_non_empty, can_copy_non_empty> p2(3, 4);
            p1.swap(p2);
            Assert::AreEqual(1, p2.first().value);
            Assert::AreEqual(2, p2.second().value);
            Assert::AreEqual(3, p1.first().value);
            Assert::AreEqual(4, p1.second().value);
            std::swap(p1, p2);
            Assert::AreEqual(1, p1.first().value);
            Assert::AreEqual(2, p1.second().value);
            Assert::AreEqual(3, p2.first().value);
            Assert::AreEqual(4, p2.second().value);

            compressed_pair<can_copy_empty, can_copy_non_empty> p3(can_copy_empty{}, 1);
            compressed_pair<can_copy_empty, can_copy_non_empty> p4(can_copy_empty{}, 2);
            p3.swap(p4);
            Assert::AreEqual(1, p4.second().value);
            Assert::AreEqual(2, p3.second().value);
            std::swap(p3, p4);
            Assert::AreEqual(1, p3.second().value);
            Assert::AreEqual(2, p4.second().value);

            compressed_pair<can_copy_non_empty, can_copy_empty> p5(1, can_copy_empty{});
            compressed_pair<can_copy_non_empty, can_copy_empty> p6(2, can_copy_empty{});
            p5.swap(p6);
            Assert::AreEqual(1, p6.first().value);
            Assert::AreEqual(2, p5.first().value);
            std::swap(p5, p6);
            Assert::AreEqual(1, p5.first().value);
            Assert::AreEqual(2, p6.first().value);
        }

#pragma endregion
    };
}

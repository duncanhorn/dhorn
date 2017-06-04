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
            non_empty() = default;
            non_empty(int value) : value(value) {}

            int value = 42;
        };

        struct non_empty_explicit
        {
            explicit non_empty_explicit() {}
            explicit non_empty_explicit(int value) : value(value) {}
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
            empty_adapter() = default;
            template <typename T = Ty, std::enable_if_t<std::is_copy_constructible_v<T>, int> = 0>
            empty_adapter(const Ty&) {}
            template <typename T = Ty, std::enable_if_t<std::is_move_constructible_v<T>, int> = 0>
            empty_adapter(Ty&&) {}

            template <typename T = Ty, std::enable_if_t<std::is_copy_assignable_v<T>, int> = 0>
            empty_adapter& operator=(const Ty&) { return *this; }
            template <typename T = Ty, std::enable_if_t<std::is_move_assignable_v<T>, int> = 0>
            empty_adapter& operator=(Ty&&) { return *this; }
        };

        template <typename Ty>
        struct empty_explicit_adapter
        {
            empty_explicit_adapter() = default;
            template <typename T = Ty, std::enable_if_t<std::is_copy_constructible_v<T>, int> = 0>
            explicit empty_explicit_adapter(const Ty&) {}
            template <typename T = Ty, std::enable_if_t<std::is_move_constructible_v<T>, int> = 0>
            explicit empty_explicit_adapter(Ty&&) {}

            template <typename T = Ty, std::enable_if_t<std::is_copy_assignable_v<T>, int> = 0>
            empty_explicit_adapter& operator=(const Ty&) { return *this; }
            template <typename T = Ty, std::enable_if_t<std::is_move_assignable_v<T>, int> = 0>
            empty_explicit_adapter& operator=(Ty&&) { return *this; }
        };

        template <typename Ty>
        struct empty_final_adapter final
        {
            empty_final_adapter() = default;
            template <typename T = Ty, std::enable_if_t<std::is_copy_constructible_v<T>, int> = 0>
            empty_final_adapter(const Ty&) {}
            template <typename T = Ty, std::enable_if_t<std::is_move_constructible_v<T>, int> = 0>
            empty_final_adapter(Ty&&) {}

            template <typename T = Ty, std::enable_if_t<std::is_copy_assignable_v<T>, int> = 0>
            empty_final_adapter& operator=(const Ty&) { return *this; }
            template <typename T = Ty, std::enable_if_t<std::is_move_assignable_v<T>, int> = 0>
            empty_final_adapter& operator=(Ty&&) { return *this; }
        };

        template <typename Ty>
        struct empty_final_explicit_adapter final
        {
            empty_final_explicit_adapter() = default;
            template <typename T = Ty, std::enable_if_t<std::is_copy_constructible_v<T>, int> = 0>
            explicit empty_final_explicit_adapter(const Ty&) {}
            template <typename T = Ty, std::enable_if_t<std::is_move_constructible_v<T>, int> = 0>
            explicit empty_final_explicit_adapter(Ty&&) {}

            template <typename T = Ty, std::enable_if_t<std::is_copy_assignable_v<T>, int> = 0>
            empty_final_explicit_adapter& operator=(const Ty&) { return *this; }
            template <typename T = Ty, std::enable_if_t<std::is_move_assignable_v<T>, int> = 0>
            empty_final_explicit_adapter& operator=(Ty&&) { return *this; }
        };

        template <typename Ty>
        struct non_empty_adapter
        {
            non_empty_adapter() = default;
            template <typename T = Ty, std::enable_if_t<std::is_copy_constructible_v<T>, int> = 0>
            non_empty_adapter(const Ty& value) : value(value) {}
            template <typename T = Ty, std::enable_if_t<std::is_move_constructible_v<T>, int> = 0>
            non_empty_adapter(Ty&& value) : value(std::move(value)) {}

            template <typename T = Ty, std::enable_if_t<std::is_copy_assignable_v<T>, int> = 0>
            non_empty_adapter& operator=(const Ty& value) { this->value = value; return *this; }
            template <typename T = Ty, std::enable_if_t<std::is_move_assignable_v<T>, int> = 0>
            non_empty_adapter& operator=(Ty&& value) { this->value = std::move(value); return *this; }

            Ty value;
        };

        template <typename Ty>
        struct non_empty_explicit_adapter
        {
            non_empty_explicit_adapter() = default;
            template <typename T = Ty, std::enable_if_t<std::is_copy_constructible_v<T>, int> = 0>
            explicit non_empty_explicit_adapter(const Ty& value) : value(value) {}
            template <typename T = Ty, std::enable_if_t<std::is_move_constructible_v<T>, int> = 0>
            explicit non_empty_explicit_adapter(Ty&& value) : value(std::move(value)) {}

            template <typename T = Ty, std::enable_if_t<std::is_copy_assignable_v<T>, int> = 0>
            non_empty_explicit_adapter& operator=(const Ty& value) { this->value = value; return *this; }
            template <typename T = Ty, std::enable_if_t<std::is_move_assignable_v<T>, int> = 0>
            non_empty_explicit_adapter& operator=(Ty&& value) { this->value = std::move(value); return *this; }

            Ty value;
        };

#pragma endregion

#pragma endregion



        TEST_METHOD_INITIALIZE(TestInitialize)
        {
            object_counter::reset();
        }

        TEST_METHOD_CLEANUP(TestCleanup)
        {
            Assert::AreEqual(0u, object_counter::instance_count);
        }



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
            // Copy/move; ignore input template args. Copy/move are _never_ explicit
            test.template run<empty, empty, empty, empty, true>();
            test.template run<empty, empty_final, empty, empty_final, true>();
            test.template run<empty, non_empty, empty, non_empty, true>();
            test.template run<empty, empty_explicit, empty, empty_explicit, true>();
            test.template run<empty, empty_final_explicit, empty, empty_final_explicit, true>();
            test.template run<empty, non_empty_explicit, empty, non_empty_explicit, true>();

            test.template run<empty_final, empty_final, empty_final, empty_final, true>();
            test.template run<empty_final, non_empty, empty_final, non_empty, true>();
            test.template run<empty_final, empty_explicit, empty_final, empty_explicit, true>();
            test.template run<empty_final, empty_final_explicit, empty_final, empty_final_explicit, true>();
            test.template run<empty_final, non_empty_explicit, empty_final, non_empty_explicit, true>();

            test.template run<non_empty, non_empty, non_empty, non_empty, true>();
            test.template run<non_empty, empty_explicit, non_empty, empty_explicit, true>();
            test.template run<non_empty, empty_final_explicit, non_empty, empty_final_explicit, true>();
            test.template run<non_empty, non_empty_explicit, non_empty, non_empty_explicit, true>();

            test.template run<empty_explicit, empty_explicit, empty_explicit, empty_explicit, true>();
            test.template run<empty_explicit, empty_final_explicit, empty_explicit, empty_final_explicit, true>();
            test.template run<empty_explicit, non_empty_explicit, empty_explicit, non_empty_explicit, true>();

            test.template run<empty_final_explicit, empty_final_explicit, empty_final_explicit, empty_final_explicit, true>();
            test.template run<empty_final_explicit, non_empty_explicit, empty_final_explicit, non_empty_explicit, true>();

            test.template run<non_empty_explicit, non_empty_explicit, non_empty_explicit, non_empty_explicit, true>();

            // Conversion copy/move
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
            // Conversion copy/move
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

            compressed_pair<non_empty, non_empty> p1 = {};
            Assert::AreEqual(42, p1.first().value);
            Assert::AreEqual(42, p1.second().value);

            compressed_pair<empty, non_empty> p2 = {};
            Assert::AreEqual(42, p1.second().value);

            compressed_pair<non_empty, empty> p3 = {};
            Assert::AreEqual(42, p1.first().value);

            compressed_pair<object_counter, object_counter> p;
            Assert::AreEqual(2u, object_counter::constructed_count);
        }

        TEST_METHOD(ExplicitDefaultConstructionTest)
        {
            DoExplicitConstructionTest(DefaultConstructionTester{});

            compressed_pair<non_empty_explicit, non_empty_explicit> p1;
            Assert::AreEqual(42, p1.first().value);
            Assert::AreEqual(42, p1.second().value);

            compressed_pair<empty_explicit, non_empty> p2;
            Assert::AreEqual(42, p1.second().value);

            compressed_pair<non_empty, empty_explicit> p3;
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

            int a = 8;
            int b = 42;
            compressed_pair<non_empty, non_empty> p1 = { a, b };
            Assert::AreEqual(8, p1.first().value);
            Assert::AreEqual(42, p1.second().value);

            compressed_pair<empty, non_empty> p2 = { {}, a };
            Assert::AreEqual(8, p2.second().value);

            compressed_pair<non_empty, empty> p3 = { a, {} };
            Assert::AreEqual(8, p3.first().value);

            object_counter obj;
            compressed_pair<object_counter, object_counter> p(obj, obj);
            Assert::AreEqual(3u, object_counter::constructed_count);
            Assert::AreEqual(2u, object_counter::copy_count);
        }

        TEST_METHOD(ExplicitValueConstructionTest)
        {
            DoExplicitConstructionTest(ValueConstructionTester{});

            int a = 8;
            int b = 42;
            compressed_pair<non_empty_explicit, non_empty_explicit> p1(a, b);
            Assert::AreEqual(8, p1.first().value);
            Assert::AreEqual(42, p1.second().value);

            compressed_pair<empty_explicit, non_empty> p2(empty_explicit{}, a);
            Assert::AreEqual(8, p2.second().value);

            compressed_pair<non_empty, empty_explicit> p3(a, empty_explicit{});
            Assert::AreEqual(8, p3.first().value);
        }

#pragma endregion

#pragma region Value Move Construction

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

            int a = 8;
            int b = 42;
            compressed_pair<non_empty, non_empty> p1 = { std::move(a), std::move(b) };
            Assert::AreEqual(8, p1.first().value);
            Assert::AreEqual(42, p1.second().value);

            compressed_pair<empty, non_empty> p2 = { {}, std::move(a) };
            Assert::AreEqual(8, p2.second().value);

            compressed_pair<non_empty, empty> p3 = { std::move(a), {} };
            Assert::AreEqual(8, p3.first().value);

            // NOTE: object_counter has no state, so moving it more than once is okay
            object_counter counter;

            compressed_pair<object_counter, object_counter> p4 = { std::move(counter), std::move(counter) };
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair<object_counter, non_empty> p5 = { std::move(counter), non_empty{} };
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair<non_empty, object_counter> p6 = { non_empty{}, std::move(counter) };
            Assert::AreEqual(0u, object_counter::copy_count);
        }

        TEST_METHOD(ExplicitValueMoveConstructionTest)
        {
            DoExplicitConstructionTest(ValueMoveConstructionTester{});

            int a = 8;
            int b = 42;
            compressed_pair<non_empty_explicit, non_empty_explicit> p1(std::move(a), std::move(b));
            Assert::AreEqual(8, p1.first().value);
            Assert::AreEqual(42, p1.second().value);

            compressed_pair<empty_explicit, non_empty> p2(empty_explicit{}, std::move(a));
            Assert::AreEqual(8, p2.second().value);

            compressed_pair<non_empty, empty_explicit> p3(std::move(a), empty_explicit{});
            Assert::AreEqual(8, p3.first().value);

            // NOTE: object_counter has no state, so moving it more than once is okay
            object_counter counter;

            compressed_pair<object_counter, non_empty_explicit> p4(std::move(counter), non_empty_explicit{});
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair<non_empty_explicit, object_counter> p5(non_empty_explicit{}, std::move(counter));
            Assert::AreEqual(0u, object_counter::copy_count);
        }

#pragma endregion

#pragma region Piecewise Construction

        TEST_METHOD(PiecewiseConstructionTest)
        {
            // First is empty
            compressed_pair<empty, int> p1(
                std::piecewise_construct,
                std::forward_as_tuple(),
                std::forward_as_tuple(42));
            Assert::AreEqual(42, p1.second());

            // Second is empty
            compressed_pair<int, empty_explicit> p2(
                std::piecewise_construct,
                std::forward_as_tuple(42),
                std::forward_as_tuple());
            Assert::AreEqual(42, p2.first());

            // Neither are empty
            compressed_pair<std::string, std::vector<int>> p3(
                std::piecewise_construct,
                std::forward_as_tuple("foo"),
                std::forward_as_tuple(8));
            Assert::AreEqual("foo"s, p3.first());
            Assert::AreEqual(8u, p3.second().size());

            compressed_pair<object_counter, object_counter> p4(
                std::piecewise_construct,
                std::forward_as_tuple(),
                std::forward_as_tuple(object_counter{}));
            Assert::AreEqual(2u, object_counter::instance_count);
            Assert::AreEqual(3u, object_counter::constructed_count);
            Assert::AreEqual(0u, object_counter::copy_count);
            Assert::AreEqual(1u, object_counter::move_count);
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

            compressed_pair<non_empty, non_empty> a = { 0, 1 };
            compressed_pair<non_empty, non_empty> b = a;
            Assert::AreEqual(0, b.first().value);
            Assert::AreEqual(1, b.second().value);

            compressed_pair<non_empty, empty> c = { 0, {} };
            compressed_pair<non_empty, empty> d = c;
            Assert::AreEqual(0, d.first().value);

            compressed_pair<empty, non_empty> e = { {}, 0 };
            compressed_pair<empty, non_empty> f = e;
            Assert::AreEqual(0, f.second().value);

            compressed_pair<object_counter, object_counter> p1 = {};
            compressed_pair<object_counter, object_counter> p2 = p1;
            Assert::AreEqual(2u, object_counter::copy_count);

            compressed_pair<object_counter, non_empty> p3 = {};
            compressed_pair<object_counter, non_empty> p4 = p3;
            Assert::AreEqual(3u, object_counter::copy_count);

            compressed_pair<non_empty, object_counter> p5 = {};
            compressed_pair<non_empty, object_counter> p6 = p5;
            Assert::AreEqual(4u, object_counter::copy_count);

            // Converting Copy
            using adapt = non_empty_adapter<object_counter>;
            compressed_pair<adapt, adapt> p7 = p1;
            Assert::AreEqual(6u, object_counter::copy_count);

            compressed_pair<adapt, empty_adapter<non_empty>> p8 = p3;
            Assert::AreEqual(7u, object_counter::copy_count);

            compressed_pair<empty_adapter<non_empty>, adapt> p9 = p5;
            Assert::AreEqual(8u, object_counter::copy_count);
        }

        TEST_METHOD(ExplicitCopyConstructionTest)
        {
            DoExplicitCopyMoveConstructionTest<int, int>(CopyConstructionTester{});

            compressed_pair<non_empty_explicit, non_empty_explicit> a(0, 1);
            compressed_pair<non_empty_explicit, non_empty_explicit> b = a;
            Assert::AreEqual(0, b.first().value);
            Assert::AreEqual(1, b.second().value);

            compressed_pair<non_empty_explicit, empty> c(0, empty{});
            compressed_pair<non_empty_explicit, empty> d = c;
            Assert::AreEqual(0, d.first().value);

            compressed_pair<empty, non_empty_explicit> e(empty{}, 0);
            compressed_pair<empty, non_empty_explicit> f = e;
            Assert::AreEqual(0, f.second().value);

            compressed_pair<object_counter, non_empty_explicit> p1;
            compressed_pair<object_counter, non_empty_explicit> p2 = p1;
            Assert::AreEqual(1u, object_counter::copy_count);

            compressed_pair<non_empty_explicit, object_counter> p3;
            compressed_pair<non_empty_explicit, object_counter> p4 = p3;
            Assert::AreEqual(2u, object_counter::copy_count);

            // Converting Copy
            using adapt = non_empty_explicit_adapter<object_counter>;
            compressed_pair<adapt, empty_explicit_adapter<non_empty_explicit>> p5(p1);
            Assert::AreEqual(3u, object_counter::copy_count);

            compressed_pair< empty_explicit_adapter<non_empty_explicit>, adapt> p6(p3);
            Assert::AreEqual(4u, object_counter::copy_count);
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
            DoNonExplicitCopyMoveConstructionTest<int, int>(MoveConstructionTester{});

            compressed_pair<non_empty, non_empty> a = { 0, 1 };
            compressed_pair<non_empty, non_empty> b = std::move(a);
            Assert::AreEqual(0, b.first().value);
            Assert::AreEqual(1, b.second().value);

            compressed_pair<non_empty, empty> c = { 0,{} };
            compressed_pair<non_empty, empty> d = std::move(c);
            Assert::AreEqual(0, d.first().value);

            compressed_pair<empty, non_empty> e = { {}, 0 };
            compressed_pair<empty, non_empty> f = std::move(e);
            Assert::AreEqual(0, f.second().value);

            compressed_pair<object_counter, object_counter> p1 = {};
            compressed_pair<object_counter, object_counter> p2 = std::move(p1);
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair<object_counter, non_empty> p3 = {};
            compressed_pair<object_counter, non_empty> p4 = std::move(p3);
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair<non_empty, object_counter> p5 = {};
            compressed_pair<non_empty, object_counter> p6 = std::move(p5);
            Assert::AreEqual(0u, object_counter::copy_count);

            // Converting Move
            // NOTE: object_counter has no state, so moving it more than once is okay
            using adapt = non_empty_adapter<object_counter>;
            compressed_pair<adapt, adapt> p7 = std::move(p1);
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair<adapt, empty_adapter<non_empty>> p8 = std::move(p3);
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair<empty_adapter<non_empty>, adapt> p9 = std::move(p5);
            Assert::AreEqual(0u, object_counter::copy_count);
        }

        TEST_METHOD(ExplicitMoveConstructionTest)
        {
            DoExplicitCopyMoveConstructionTest<int, int>(MoveConstructionTester{});

            compressed_pair<non_empty_explicit, non_empty_explicit> a(0, 1);
            compressed_pair<non_empty_explicit, non_empty_explicit> b = std::move(a);
            Assert::AreEqual(0, b.first().value);
            Assert::AreEqual(1, b.second().value);

            compressed_pair<non_empty_explicit, empty> c(0, empty{});
            compressed_pair<non_empty_explicit, empty> d = std::move(c);
            Assert::AreEqual(0, d.first().value);

            compressed_pair<empty, non_empty_explicit> e(empty{}, 0);
            compressed_pair<empty, non_empty_explicit> f = std::move(e);
            Assert::AreEqual(0, f.second().value);

            compressed_pair<object_counter, non_empty_explicit> p1;
            compressed_pair<object_counter, non_empty_explicit> p2 = std::move(p1);
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair<non_empty_explicit, object_counter> p3;
            compressed_pair<non_empty_explicit, object_counter> p4 = std::move(p3);
            Assert::AreEqual(0u, object_counter::copy_count);

            // Converting Move
            // NOTE: object_counter has no state, so moving it more than once is okay
            using adapt = non_empty_explicit_adapter<object_counter>;
            compressed_pair<adapt, empty_explicit_adapter<non_empty_explicit>> p5(std::move(p1));
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair< empty_explicit_adapter<non_empty_explicit>, adapt> p6(std::move(p3));
            Assert::AreEqual(0u, object_counter::copy_count);
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

        template <typename Adapt1, typename Adapt2, typename T1, typename T2, bool Expect, typename TestTy>
        static void DoConversionAssignmentTest_Helper_Impl(const TestTy& test)
        {
            test.template run<compressed_pair<Adapt1, Adapt2>, compressed_pair<T1, T2>, Expect>();
            test.template run<compressed_pair<Adapt2, Adapt1>, compressed_pair<T2, T1>, Expect>();
        }

        template <typename T1, typename T2, bool Expect, typename TestTy>
        static void DoConversionAssignmentTest_Helper(const TestTy& test)
        {
            using empty_1 = empty_adapter<T1>;
            using empty_2 = empty_adapter<T2>;
            using final_1 = empty_final_adapter<T1>;
            using final_2 = empty_final_adapter<T2>;
            using non_empty_1 = non_empty_adapter<T1>;
            using non_empty_2 = non_empty_adapter<T2>;

            DoConversionAssignmentTest_Helper_Impl<empty_1, empty_2, T1, T2, Expect>(test);
            DoConversionAssignmentTest_Helper_Impl<empty_1, final_2, T1, T2, Expect>(test);
            DoConversionAssignmentTest_Helper_Impl<empty_1, non_empty_2, T1, T2, Expect>(test);

            DoConversionAssignmentTest_Helper_Impl<final_1, empty_2, T1, T2, Expect>(test);
            DoConversionAssignmentTest_Helper_Impl<final_1, final_2, T1, T2, Expect>(test);
            DoConversionAssignmentTest_Helper_Impl<final_1, non_empty_2, T1, T2, Expect>(test);

            DoConversionAssignmentTest_Helper_Impl<non_empty_1, empty_2, T1, T2, Expect>(test);
            DoConversionAssignmentTest_Helper_Impl<non_empty_1, final_2, T1, T2, Expect>(test);
            DoConversionAssignmentTest_Helper_Impl<non_empty_1, non_empty_2, T1, T2, Expect>(test);
        }

        template <typename TestTy>
        static void DoConversionAssignmentTest(const TestTy& test)
        {
            DoConversionAssignmentTest_Helper<can_copy_empty, can_copy_empty, true>(test);
            DoConversionAssignmentTest_Helper<can_copy_empty, can_copy_final, true>(test);
            DoConversionAssignmentTest_Helper<can_copy_empty, can_copy_non_empty, true>(test);
            DoConversionAssignmentTest_Helper<can_copy_empty, cannot_copy_empty, false>(test);
            DoConversionAssignmentTest_Helper<can_copy_empty, cannot_copy_final, false>(test);
            DoConversionAssignmentTest_Helper<can_copy_empty, cannot_copy_non_empty, false>(test);

            DoConversionAssignmentTest_Helper<can_copy_final, can_copy_final, true>(test);
            DoConversionAssignmentTest_Helper<can_copy_final, can_copy_non_empty, true>(test);
            DoConversionAssignmentTest_Helper<can_copy_final, cannot_copy_empty, false>(test);
            DoConversionAssignmentTest_Helper<can_copy_final, cannot_copy_final, false>(test);
            DoConversionAssignmentTest_Helper<can_copy_final, cannot_copy_non_empty, false>(test);

            DoConversionAssignmentTest_Helper<can_copy_non_empty, can_copy_non_empty, true>(test);
            DoConversionAssignmentTest_Helper<can_copy_non_empty, cannot_copy_empty, false>(test);
            DoConversionAssignmentTest_Helper<can_copy_non_empty, cannot_copy_final, false>(test);
            DoConversionAssignmentTest_Helper<can_copy_non_empty, cannot_copy_non_empty, false>(test);

            DoConversionAssignmentTest_Helper<cannot_copy_empty, cannot_copy_empty, false>(test);
            DoConversionAssignmentTest_Helper<cannot_copy_empty, cannot_copy_final, false>(test);
            DoConversionAssignmentTest_Helper<cannot_copy_empty, cannot_copy_non_empty, false>(test);

            DoConversionAssignmentTest_Helper<cannot_copy_final, cannot_copy_final, false>(test);
            DoConversionAssignmentTest_Helper<cannot_copy_final, cannot_copy_non_empty, false>(test);

            DoConversionAssignmentTest_Helper<cannot_copy_non_empty, cannot_copy_non_empty, false>(test);
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

            compressed_pair<int, int> a(0, 1);
            compressed_pair<int, int> b(2, 3);
            Assert::AreEqual(2, b.first());
            Assert::AreEqual(3, b.second());
            b = a;
            Assert::AreEqual(0, b.first());
            Assert::AreEqual(1, b.second());

            compressed_pair<empty, int> c({}, 0);
            compressed_pair<empty, int> d({}, 1);
            Assert::AreEqual(1, d.second());
            d = c;
            Assert::AreEqual(0, d.second());

            compressed_pair<int, empty> e(0, {});
            compressed_pair<int, empty> f(1, {});
            Assert::AreEqual(1, f.first());
            f = e;
            Assert::AreEqual(0, f.first());

            compressed_pair<object_counter, object_counter> p1;
            compressed_pair<object_counter, object_counter> p2;
            p2 = p1;
            Assert::AreEqual(2u, object_counter::copy_count);

            compressed_pair<non_empty, object_counter> p3;
            compressed_pair<non_empty, object_counter> p4;
            p4 = p3;
            Assert::AreEqual(3u, object_counter::copy_count);

            compressed_pair<object_counter, non_empty> p5;
            compressed_pair<object_counter, non_empty> p6;
            p6 = p5;
            Assert::AreEqual(4u, object_counter::copy_count);
        }

        TEST_METHOD(CopyConversionAssignmentTest)
        {
            DoConversionAssignmentTest(CopyAssignmentTester{});

            compressed_pair<int, int> a(0, 1);
            compressed_pair<double, double> b(2, 3);
            Assert::AreEqual(2.0, b.first());
            Assert::AreEqual(3.0, b.second());
            b = a;
            Assert::AreEqual(0.0, b.first());
            Assert::AreEqual(1.0, b.second());

            compressed_pair<empty, int> c({}, 0);
            compressed_pair<empty, double> d({}, 1);
            Assert::AreEqual(1.0, d.second());
            d = c;
            Assert::AreEqual(0.0, d.second());

            compressed_pair<int, empty> e(0, {});
            compressed_pair<double, empty> f(1, {});
            Assert::AreEqual(1.0, f.first());
            f = e;
            Assert::AreEqual(0.0, f.first());

            using adapt = non_empty_adapter<object_counter>;

            compressed_pair<object_counter, object_counter> p1;
            compressed_pair<adapt, adapt> p2;
            p2 = p1;
            Assert::AreEqual(2u, object_counter::copy_count);

            compressed_pair<empty, object_counter> p3;
            compressed_pair<empty, adapt> p4;
            p4 = p3;
            Assert::AreEqual(3u, object_counter::copy_count);

            compressed_pair<object_counter, empty> p5;
            compressed_pair<adapt, empty> p6;
            p6 = p5;
            Assert::AreEqual(4u, object_counter::copy_count);
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

            compressed_pair<int, int> a(0, 1);
            compressed_pair<int, int> b(2, 3);
            Assert::AreEqual(2, b.first());
            Assert::AreEqual(3, b.second());
            b = std::move(a);
            Assert::AreEqual(0, b.first());
            Assert::AreEqual(1, b.second());

            compressed_pair<empty, int> c({}, 0);
            compressed_pair<empty, int> d({}, 1);
            Assert::AreEqual(1, d.second());
            d = std::move(c);
            Assert::AreEqual(0, d.second());

            compressed_pair<int, empty> e(0, {});
            compressed_pair<int, empty> f(1, {});
            Assert::AreEqual(1, f.first());
            f = std::move(e);
            Assert::AreEqual(0, f.first());

            compressed_pair<object_counter, object_counter> p1;
            compressed_pair<object_counter, object_counter> p2;
            p2 = std::move(p1);
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair<non_empty, object_counter> p3;
            compressed_pair<non_empty, object_counter> p4;
            p4 = std::move(p3);
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair<object_counter, non_empty> p5;
            compressed_pair<object_counter, non_empty> p6;
            p6 = std::move(p5);
            Assert::AreEqual(0u, object_counter::copy_count);
        }

        TEST_METHOD(MoveConversionAssignmentTest)
        {
            DoConversionAssignmentTest(MoveAssignmentTester{});

            compressed_pair<int, int> a(0, 1);
            compressed_pair<double, double> b(2, 3);
            Assert::AreEqual(2.0, b.first());
            Assert::AreEqual(3.0, b.second());
            b = std::move(a);
            Assert::AreEqual(0.0, b.first());
            Assert::AreEqual(1.0, b.second());

            compressed_pair<empty, int> c({}, 0);
            compressed_pair<empty, double> d({}, 1);
            Assert::AreEqual(1.0, d.second());
            d = std::move(c);
            Assert::AreEqual(0.0, d.second());

            compressed_pair<int, empty> e(0, {});
            compressed_pair<double, empty> f(1, {});
            Assert::AreEqual(1.0, f.first());
            f = std::move(e);
            Assert::AreEqual(0.0, f.first());

            using adapt = non_empty_adapter<object_counter>;

            compressed_pair<object_counter, object_counter> p1;
            compressed_pair<adapt, adapt> p2;
            p2 = std::move(p1);
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair<empty, object_counter> p3;
            compressed_pair<empty, adapt> p4;
            p4 = std::move(p3);
            Assert::AreEqual(0u, object_counter::copy_count);

            compressed_pair<object_counter, empty> p5;
            compressed_pair<adapt, empty> p6;
            p6 = std::move(p5);
            Assert::AreEqual(0u, object_counter::copy_count);
        }

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



#pragma region Comparison Operators Tests

        template <typename TestTy>
        void DoComparisonTest(const TestTy& test)
        {
            compressed_pair<int, int> a1(10, 10);
            compressed_pair<int, int> a2(10, 10);
            test.template run<0>(a1, a1);
            test.template run<0>(a2, a2);
            test.template run<0>(a1, a2);
            test.template run<0>(a2, a1);

            a1.first() = 8;
            test.template run<-1>(a1, a2);
            test.template run<1>(a2, a1);

            a1.first() = 10;
            a1.second() = 42;
            test.template run<1>(a1, a2);
            test.template run<-1>(a2, a1);


            compressed_pair<int, std::string> b1(42, "foo");
            compressed_pair<int, std::string> b2(42, "foo");
            test.template run<0>(b1, b1);
            test.template run<0>(b2, b2);
            test.template run<0>(b1, b2);
            test.template run<0>(b2, b1);

            b1.first() = 8;
            test.template run<-1>(b1, b2);
            test.template run<1>(b2, b1);

            b1.first() = 42;
            b2.second() = "bar";
            test.template run<1>(b1, b2);
            test.template run<-1>(b2, b1);
        }

        struct EqualityTester
        {
            template <int Compare, typename PairTy>
            void run(const PairTy& lhs, const PairTy& rhs) const
            {
                Assert::AreEqual(Compare == 0, lhs == rhs);
                Assert::AreEqual(Compare == 0, lhs == rhs);
            }
        };

        TEST_METHOD(EqualityComparisonTest)
        {
            DoComparisonTest(EqualityTester{});
        }

        struct InEqualityTester
        {
            template <int Compare, typename PairTy>
            void run(const PairTy& lhs, const PairTy& rhs) const
            {
                Assert::AreEqual(Compare != 0, lhs != rhs);
            }
        };

        TEST_METHOD(InEqualityComparisonTest)
        {
            DoComparisonTest(InEqualityTester{});
        }

        struct LessThanTester
        {
            template <int Compare, typename PairTy>
            void run(const PairTy& lhs, const PairTy& rhs) const
            {
                Assert::AreEqual(Compare < 0, lhs < rhs);
            }
        };

        TEST_METHOD(LessThanComparisonTest)
        {
            DoComparisonTest(LessThanTester{});
        }

        struct GreaterThanTester
        {
            template <int Compare, typename PairTy>
            void run(const PairTy& lhs, const PairTy& rhs) const
            {
                Assert::AreEqual(Compare > 0, lhs > rhs);
            }
        };

        TEST_METHOD(GreaterThanComparisonTest)
        {
            DoComparisonTest(GreaterThanTester{});
        }

        struct LessThanOrEqualTester
        {
            template <int Compare, typename PairTy>
            void run(const PairTy& lhs, const PairTy& rhs) const
            {
                Assert::AreEqual(Compare <= 0, lhs <= rhs);
            }
        };

        TEST_METHOD(LessThanOrEqualComparisonTest)
        {
            DoComparisonTest(LessThanOrEqualTester{});
        }

        struct GreaterThanOrEqualTester
        {
            template <int Compare, typename PairTy>
            void run(const PairTy& lhs, const PairTy& rhs) const
            {
                Assert::AreEqual(Compare >= 0, lhs >= rhs);
            }
        };

        TEST_METHOD(GreaterThanOrEqualComparisonTest)
        {
            DoComparisonTest(GreaterThanOrEqualTester{});
        }

#pragma endregion



#pragma region Std Functions Tests

        TEST_METHOD(TupleSizeTest)
        {
            Assert::AreEqual(2u, std::tuple_size_v<compressed_pair<int, int>>);
        }

        TEST_METHOD(TupleElementTest)
        {
            using test_type = compressed_pair<int, float>;
            Assert::IsTrue(std::is_same_v<int, std::tuple_element_t<0, test_type>>);
            Assert::IsTrue(std::is_same_v<float, std::tuple_element_t<1, test_type>>);
        }

        TEST_METHOD(GetTest)
        {
            using test_type = dhorn::compressed_pair<int, std::string>;
            test_type p;

            // By Index
            Assert::IsTrue(std::is_same_v<int&, decltype(std::get<0>(p))>);
            Assert::IsTrue(std::is_same_v<const int&, decltype(std::get<0>(std::as_const(p)))>);
            Assert::IsTrue(std::is_same_v<int&&, decltype(std::get<0>(std::move(p)))>);
            Assert::IsTrue(std::is_same_v<const int&&, decltype(std::get<0>(std::move(std::as_const(p))))>);

            Assert::IsTrue(std::is_same_v<std::string&, decltype(std::get<1>(p))>);
            Assert::IsTrue(std::is_same_v<const std::string&, decltype(std::get<1>(std::as_const(p)))>);
            Assert::IsTrue(std::is_same_v<std::string&&, decltype(std::get<1>(std::move(p)))>);
            Assert::IsTrue(std::is_same_v<const std::string&&, decltype(std::get<1>(std::move(std::as_const(p))))>);

            std::get<0>(p) = 42;
            std::get<1>(p) = "foo";
            Assert::AreEqual(42, p.first());
            Assert::IsTrue(p.second() == "foo");

            // By Type
            Assert::IsTrue(std::is_same_v<int&, decltype(std::get<int>(p))>);
            Assert::IsTrue(std::is_same_v<const int&, decltype(std::get<int>(std::as_const(p)))>);
            Assert::IsTrue(std::is_same_v<int&&, decltype(std::get<int>(std::move(p)))>);
            Assert::IsTrue(std::is_same_v<const int&&, decltype(std::get<int>(std::move(std::as_const(p))))>);

            Assert::IsTrue(std::is_same_v<std::string&, decltype(std::get<std::string>(p))>);
            Assert::IsTrue(std::is_same_v<const std::string&, decltype(std::get<std::string>(std::as_const(p)))>);
            Assert::IsTrue(std::is_same_v<std::string&&, decltype(std::get<std::string>(std::move(p)))>);
            Assert::IsTrue(std::is_same_v<const std::string&&, decltype(std::get<std::string>(std::move(std::as_const(p))))>);

            std::get<int>(p) = 8;
            std::get<std::string>(p) = "bar";
            Assert::AreEqual(8, p.first());
            Assert::IsTrue(p.second() == "bar");

            // The following fails to compile (as it should)
            //compressed_pair<int, int> p2;
            //std::get<int>(p2) = 0;

            // Object counting
            compressed_pair<object_counter, object_counter> pair;

            auto obj = std::get<0>(pair);
            obj = std::get<1>(pair);
            Assert::AreEqual(2u, object_counter::copy_count);

            obj = std::get<0>(std::as_const(pair));
            obj = std::get<1>(std::as_const(pair));
            Assert::AreEqual(4u, object_counter::copy_count);

            obj = std::get<0>(std::move(pair));
            obj = std::get<1>(std::move(pair));
            Assert::AreEqual(4u, object_counter::copy_count);

            obj = std::get<0>(std::move(std::as_const(pair)));
            obj = std::get<1>(std::move(std::as_const(pair)));
            Assert::AreEqual(4u, object_counter::copy_count);
        }

#pragma endregion
    };
}

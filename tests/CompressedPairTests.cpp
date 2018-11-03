/*
 * Duncan Horn
 *
 * CompressedPairTests.cpp
 *
 * Tests for the dhorn::compressed_pair type
 */

#include <dhorn/compressed_pair.h>
#include <dhorn/utility.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "object_counter.h"

using namespace dhorn::literals;
using namespace std::literals;

using std::swap;

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

struct CompressedPairTests : testing::Test
{
    virtual void SetUp() override
    {
        dhorn::tests::object_counter::reset();
    }

    virtual void TearDown() override
    {
        ASSERT_EQ(0_sz, dhorn::tests::object_counter::instance_count);
    }
};

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

    ASSERT_EQ(sizeof(dummy), sizeof(dhorn::compressed_pair<T1, T2>));
}

TEST_F(CompressedPairTests, EmptySizeTest)
{
    DoSizeTest<empty, empty, true>();
    DoSizeTest<empty, empty_explicit, true>();
    DoSizeTest<empty_explicit, empty, true>();
    DoSizeTest<empty_explicit, empty_explicit, true>();

    DoSizeTest<empty, non_empty, true>();
    DoSizeTest<non_empty, empty, true>();
}

TEST_F(CompressedPairTests, NonEmptySizeTest)
{
    DoSizeTest<non_empty, non_empty, false>();
    DoSizeTest<non_empty, non_empty_explicit, false>();
    DoSizeTest<non_empty_explicit, non_empty, false>();
    DoSizeTest<non_empty_explicit, non_empty_explicit, false>();
}

TEST_F(CompressedPairTests, FinalSizeTest)
{
    // Two non-empty types has a size of the sum of the two (plus potential padding,  but since we only use
    // ints, there won't be any padding)
    DoSizeTest<empty_final, empty_final, false>();
    DoSizeTest<empty_final, empty_final_explicit, false>();
    DoSizeTest<empty_final_explicit, empty_final, false>();
    DoSizeTest<empty_final_explicit, empty_final_explicit, false>();
}

TEST_F(CompressedPairTests, StandardTypesTest)
{
    DoSizeTest<int, int, false>();
    DoSizeTest<int, float, false>();
    DoSizeTest<float, int, false>();
    DoSizeTest<float, float, false>();
}

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

struct DefaultConstructionTester
{
    template <typename T1, typename T2, bool Expect>
    void run() const
    {
        ASSERT_EQ(Expect, (dhorn::is_implicitly_default_constructible_v<dhorn::compressed_pair<T1, T2>>));
        ASSERT_EQ(Expect, (dhorn::is_implicitly_default_constructible_v<dhorn::compressed_pair<T2, T1>>));

        ASSERT_TRUE((std::is_default_constructible_v<dhorn::compressed_pair<T1, T2>>));
        ASSERT_TRUE((std::is_default_constructible_v<dhorn::compressed_pair<T2, T1>>));
    }
};

TEST_F(CompressedPairTests, DefaultConstructionTest)
{
    DoNonExplicitConstructionTest(DefaultConstructionTester{});

    dhorn::compressed_pair<non_empty, non_empty> p1 = {};
    ASSERT_EQ(42, p1.first().value);
    ASSERT_EQ(42, p1.second().value);

    dhorn::compressed_pair<empty, non_empty> p2 = {};
    ASSERT_EQ(42, p2.second().value);

    dhorn::compressed_pair<non_empty, empty> p3 = {};
    ASSERT_EQ(42, p3.first().value);

    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
}

TEST_F(CompressedPairTests, ExplicitDefaultConstructionTest)
{
    DoExplicitConstructionTest(DefaultConstructionTester{});

    dhorn::compressed_pair<non_empty_explicit, non_empty_explicit> p1;
    ASSERT_EQ(42, p1.first().value);
    ASSERT_EQ(42, p1.second().value);

    dhorn::compressed_pair<empty_explicit, non_empty> p2;
    ASSERT_EQ(42, p1.second().value);

    dhorn::compressed_pair<non_empty, empty_explicit> p3;
    ASSERT_EQ(42, p1.first().value);
}

struct ValueConstructionTester
{
    template <typename T1, typename T2, bool Expect>
    void run() const
    {
        ASSERT_EQ(Expect, (dhorn::is_implicitly_constructible_v<dhorn::compressed_pair<T1, T2>, const T1&, const T2&>));
        ASSERT_EQ(Expect, (dhorn::is_implicitly_constructible_v<dhorn::compressed_pair<T2, T1>, const T2&, const T1&>));

        ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_pair<T1, T2>, const T1&, const T2&>));
        ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_pair<T2, T1>, const T2&, const T1&>));
    }
};

TEST_F(CompressedPairTests, ValueConstructionTest)
{
    DoNonExplicitConstructionTest(ValueConstructionTester{});

    int a = 8;
    int b = 42;
    dhorn::compressed_pair<non_empty, non_empty> p1 = { a, b };
    ASSERT_EQ(8, p1.first().value);
    ASSERT_EQ(42, p1.second().value);

    dhorn::compressed_pair<empty, non_empty> p2 = { {}, a };
    ASSERT_EQ(8, p2.second().value);

    dhorn::compressed_pair<non_empty, empty> p3 = { a, {} };
    ASSERT_EQ(8, p3.first().value);

    dhorn::tests::object_counter obj;
    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p(obj, obj);
    ASSERT_EQ(3_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedPairTests, ExplicitValueConstructionTest)
{
    DoExplicitConstructionTest(ValueConstructionTester{});

    int a = 8;
    int b = 42;
    dhorn::compressed_pair<non_empty_explicit, non_empty_explicit> p1(a, b);
    ASSERT_EQ(8, p1.first().value);
    ASSERT_EQ(42, p1.second().value);

    dhorn::compressed_pair<empty_explicit, non_empty> p2(empty_explicit{}, a);
    ASSERT_EQ(8, p2.second().value);

    dhorn::compressed_pair<non_empty, empty_explicit> p3(a, empty_explicit{});
    ASSERT_EQ(8, p3.first().value);
}

struct ValueMoveConstructionTester
{
    template <typename T1, typename T2, bool Expect>
    void run() const
    {
        ASSERT_EQ(Expect, (dhorn::is_implicitly_constructible_v<dhorn::compressed_pair<T1, T2>, T1&&, T2&&>));
        ASSERT_EQ(Expect, (dhorn::is_implicitly_constructible_v<dhorn::compressed_pair<T2, T1>, T2&&, T1&&>));

        ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_pair<T1, T2>, T1&&, T2&&>));
        ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_pair<T2, T1>, T2&&, T1&&>));
    }
};

TEST_F(CompressedPairTests, ValueMoveConstructionTest)
{
    DoNonExplicitConstructionTest(ValueMoveConstructionTester{});

    int a = 8;
    int b = 42;
    dhorn::compressed_pair<non_empty, non_empty> p1 = { std::move(a), std::move(b) };
    ASSERT_EQ(8, p1.first().value);
    ASSERT_EQ(42, p1.second().value);

    dhorn::compressed_pair<empty, non_empty> p2 = { {}, std::move(a) };
    ASSERT_EQ(8, p2.second().value);

    dhorn::compressed_pair<non_empty, empty> p3 = { std::move(a), {} };
    ASSERT_EQ(8, p3.first().value);

    // NOTE: dhorn::tests::object_counter has no state, so moving it more than once is okay
    dhorn::tests::object_counter counter;

    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p4 = { std::move(counter), std::move(counter) };
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty> p5 = { std::move(counter), non_empty{} };
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<non_empty, dhorn::tests::object_counter> p6 = { non_empty{}, std::move(counter) };
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedPairTests, ExplicitValueMoveConstructionTest)
{
    DoExplicitConstructionTest(ValueMoveConstructionTester{});

    int a = 8;
    int b = 42;
    dhorn::compressed_pair<non_empty_explicit, non_empty_explicit> p1(std::move(a), std::move(b));
    ASSERT_EQ(8, p1.first().value);
    ASSERT_EQ(42, p1.second().value);

    dhorn::compressed_pair<empty_explicit, non_empty> p2(empty_explicit{}, std::move(a));
    ASSERT_EQ(8, p2.second().value);

    dhorn::compressed_pair<non_empty, empty_explicit> p3(std::move(a), empty_explicit{});
    ASSERT_EQ(8, p3.first().value);

    // NOTE: dhorn::tests::object_counter has no state, so moving it more than once is okay
    dhorn::tests::object_counter counter;

    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty_explicit> p4(std::move(counter), non_empty_explicit{});
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<non_empty_explicit, dhorn::tests::object_counter> p5(non_empty_explicit{}, std::move(counter));
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedPairTests, PiecewiseConstructionTest)
{
    // First is empty
    dhorn::compressed_pair<empty, int> p1(
        std::piecewise_construct,
        std::forward_as_tuple(),
        std::forward_as_tuple(42));
    ASSERT_EQ(42, p1.second());

    // Second is empty
    dhorn::compressed_pair<int, empty_explicit> p2(
        std::piecewise_construct,
        std::forward_as_tuple(42),
        std::forward_as_tuple());
    ASSERT_EQ(42, p2.first());

    // Neither are empty
    dhorn::compressed_pair<std::string, std::vector<int>> p3(
        std::piecewise_construct,
        std::forward_as_tuple("foo"),
        std::forward_as_tuple(8));
    ASSERT_EQ("foo"s, p3.first());
    ASSERT_EQ(8_sz, p3.second().size());

    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p4(
        std::piecewise_construct,
        std::forward_as_tuple(),
        std::forward_as_tuple(dhorn::tests::object_counter{}));
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(3_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::move_count);
}

struct CopyConstructionTester
{
    template <typename T1, typename T2, typename From1, typename From2, bool Expect>
    void run() const
    {
        ASSERT_EQ(Expect, (dhorn::is_implicitly_constructible_v<dhorn::compressed_pair<T1, T2>, const dhorn::compressed_pair<From1, From2>&>));
        ASSERT_EQ(Expect, (dhorn::is_implicitly_constructible_v<dhorn::compressed_pair<T2, T1>, const dhorn::compressed_pair<From2, From1>&>));
    }
};

TEST_F(CompressedPairTests, CopyConstructionTest)
{
    DoNonExplicitCopyMoveConstructionTest<int, int>(CopyConstructionTester{});

    dhorn::compressed_pair<non_empty, non_empty> a = { 0, 1 };
    dhorn::compressed_pair<non_empty, non_empty> b = a;
    ASSERT_EQ(0, b.first().value);
    ASSERT_EQ(1, b.second().value);

    dhorn::compressed_pair<non_empty, empty> c = { 0, {} };
    dhorn::compressed_pair<non_empty, empty> d = c;
    ASSERT_EQ(0, d.first().value);

    dhorn::compressed_pair<empty, non_empty> e = { {}, 0 };
    dhorn::compressed_pair<empty, non_empty> f = e;
    ASSERT_EQ(0, f.second().value);

    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p1 = {};
    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p2 = p1;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty> p3 = {};
    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty> p4 = p3;
    ASSERT_EQ(3_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<non_empty, dhorn::tests::object_counter> p5 = {};
    dhorn::compressed_pair<non_empty, dhorn::tests::object_counter> p6 = p5;
    ASSERT_EQ(4_sz, dhorn::tests::object_counter::copy_count);

    // Converting Copy
    using adapt = non_empty_adapter<dhorn::tests::object_counter>;
    dhorn::compressed_pair<adapt, adapt> p7 = p1;
    ASSERT_EQ(6_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<adapt, empty_adapter<non_empty>> p8 = p3;
    ASSERT_EQ(7_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<empty_adapter<non_empty>, adapt> p9 = p5;
    ASSERT_EQ(8_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedPairTests, ExplicitCopyConstructionTest)
{
    DoExplicitCopyMoveConstructionTest<int, int>(CopyConstructionTester{});

    dhorn::compressed_pair<non_empty_explicit, non_empty_explicit> a(0, 1);
    dhorn::compressed_pair<non_empty_explicit, non_empty_explicit> b = a;
    ASSERT_EQ(0, b.first().value);
    ASSERT_EQ(1, b.second().value);

    dhorn::compressed_pair<non_empty_explicit, empty> c(0, empty{});
    dhorn::compressed_pair<non_empty_explicit, empty> d = c;
    ASSERT_EQ(0, d.first().value);

    dhorn::compressed_pair<empty, non_empty_explicit> e(empty{}, 0);
    dhorn::compressed_pair<empty, non_empty_explicit> f = e;
    ASSERT_EQ(0, f.second().value);

    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty_explicit> p1;
    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty_explicit> p2 = p1;
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<non_empty_explicit, dhorn::tests::object_counter> p3;
    dhorn::compressed_pair<non_empty_explicit, dhorn::tests::object_counter> p4 = p3;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::copy_count);

    // Converting Copy
    using adapt = non_empty_explicit_adapter<dhorn::tests::object_counter>;
    dhorn::compressed_pair<adapt, empty_explicit_adapter<non_empty_explicit>> p5(p1);
    ASSERT_EQ(3_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair< empty_explicit_adapter<non_empty_explicit>, adapt> p6(p3);
    ASSERT_EQ(4_sz, dhorn::tests::object_counter::copy_count);
}

struct MoveConstructionTester
{
    template <typename T1, typename T2, typename From1, typename From2, bool Expect>
    void run() const
    {
        ASSERT_EQ(Expect, (dhorn::is_implicitly_constructible_v<dhorn::compressed_pair<T1, T2>, dhorn::compressed_pair<From1, From2>&&>));
        ASSERT_EQ(Expect, (dhorn::is_implicitly_constructible_v<dhorn::compressed_pair<T2, T1>, dhorn::compressed_pair<From2, From1>&&>));
    }
};

TEST_F(CompressedPairTests, MoveConstructionTest)
{
    DoNonExplicitCopyMoveConstructionTest<int, int>(MoveConstructionTester{});

    dhorn::compressed_pair<non_empty, non_empty> a = { 0, 1 };
    dhorn::compressed_pair<non_empty, non_empty> b = std::move(a);
    ASSERT_EQ(0, b.first().value);
    ASSERT_EQ(1, b.second().value);

    dhorn::compressed_pair<non_empty, empty> c = { 0,{} };
    dhorn::compressed_pair<non_empty, empty> d = std::move(c);
    ASSERT_EQ(0, d.first().value);

    dhorn::compressed_pair<empty, non_empty> e = { {}, 0 };
    dhorn::compressed_pair<empty, non_empty> f = std::move(e);
    ASSERT_EQ(0, f.second().value);

    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p1 = {};
    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p2 = std::move(p1);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty> p3 = {};
    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty> p4 = std::move(p3);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<non_empty, dhorn::tests::object_counter> p5 = {};
    dhorn::compressed_pair<non_empty, dhorn::tests::object_counter> p6 = std::move(p5);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    // Converting Move
    // NOTE: dhorn::tests::object_counter has no state, so moving it more than once is okay
    using adapt = non_empty_adapter<dhorn::tests::object_counter>;
    dhorn::compressed_pair<adapt, adapt> p7 = std::move(p1);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<adapt, empty_adapter<non_empty>> p8 = std::move(p3);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<empty_adapter<non_empty>, adapt> p9 = std::move(p5);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedPairTests, ExplicitMoveConstructionTest)
{
    DoExplicitCopyMoveConstructionTest<int, int>(MoveConstructionTester{});

    dhorn::compressed_pair<non_empty_explicit, non_empty_explicit> a(0, 1);
    dhorn::compressed_pair<non_empty_explicit, non_empty_explicit> b = std::move(a);
    ASSERT_EQ(0, b.first().value);
    ASSERT_EQ(1, b.second().value);

    dhorn::compressed_pair<non_empty_explicit, empty> c(0, empty{});
    dhorn::compressed_pair<non_empty_explicit, empty> d = std::move(c);
    ASSERT_EQ(0, d.first().value);

    dhorn::compressed_pair<empty, non_empty_explicit> e(empty{}, 0);
    dhorn::compressed_pair<empty, non_empty_explicit> f = std::move(e);
    ASSERT_EQ(0, f.second().value);

    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty_explicit> p1;
    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty_explicit> p2 = std::move(p1);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<non_empty_explicit, dhorn::tests::object_counter> p3;
    dhorn::compressed_pair<non_empty_explicit, dhorn::tests::object_counter> p4 = std::move(p3);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    // Converting Move
    // NOTE: dhorn::tests::object_counter has no state, so moving it more than once is okay
    using adapt = non_empty_explicit_adapter<dhorn::tests::object_counter>;
    dhorn::compressed_pair<adapt, empty_explicit_adapter<non_empty_explicit>> p5(std::move(p1));
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair< empty_explicit_adapter<non_empty_explicit>, adapt> p6(std::move(p3));
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);
}

template <typename T1, typename T2, bool Expect, typename TestTy>
static void DoAssignmentTest_Helper(const TestTy& test)
{
    test.template run<dhorn::compressed_pair<T1, T2>, dhorn::compressed_pair<T1, T2>, Expect>();
    test.template run<dhorn::compressed_pair<T2, T1>, dhorn::compressed_pair<T2, T1>, Expect>();
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
    test.template run<dhorn::compressed_pair<Adapt1, Adapt2>, dhorn::compressed_pair<T1, T2>, Expect>();
    test.template run<dhorn::compressed_pair<Adapt2, Adapt1>, dhorn::compressed_pair<T2, T1>, Expect>();
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
        ASSERT_EQ(Expect, (std::is_assignable_v<To&, const From&>));
    }
};

TEST_F(CompressedPairTests, CopyAssignmentTest)
{
    DoAssignmentTest(CopyAssignmentTester{});

    dhorn::compressed_pair<int, int> a(0, 1);
    dhorn::compressed_pair<int, int> b(2, 3);
    ASSERT_EQ(2, b.first());
    ASSERT_EQ(3, b.second());
    b = a;
    ASSERT_EQ(0, b.first());
    ASSERT_EQ(1, b.second());

    dhorn::compressed_pair<empty, int> c({}, 0);
    dhorn::compressed_pair<empty, int> d({}, 1);
    ASSERT_EQ(1, d.second());
    d = c;
    ASSERT_EQ(0, d.second());

    dhorn::compressed_pair<int, empty> e(0, {});
    dhorn::compressed_pair<int, empty> f(1, {});
    ASSERT_EQ(1, f.first());
    f = e;
    ASSERT_EQ(0, f.first());

    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p1;
    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p2;
    p2 = p1;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<non_empty, dhorn::tests::object_counter> p3;
    dhorn::compressed_pair<non_empty, dhorn::tests::object_counter> p4;
    p4 = p3;
    ASSERT_EQ(3_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty> p5;
    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty> p6;
    p6 = p5;
    ASSERT_EQ(4_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedPairTests, CopyConversionAssignmentTest)
{
    DoConversionAssignmentTest(CopyAssignmentTester{});

    dhorn::compressed_pair<int, int> a(0, 1);
    dhorn::compressed_pair<double, double> b(2, 3);
    ASSERT_EQ(2.0, b.first());
    ASSERT_EQ(3.0, b.second());
    b = a;
    ASSERT_EQ(0.0, b.first());
    ASSERT_EQ(1.0, b.second());

    dhorn::compressed_pair<empty, int> c({}, 0);
    dhorn::compressed_pair<empty, double> d({}, 1);
    ASSERT_EQ(1.0, d.second());
    d = c;
    ASSERT_EQ(0.0, d.second());

    dhorn::compressed_pair<int, empty> e(0, {});
    dhorn::compressed_pair<double, empty> f(1, {});
    ASSERT_EQ(1.0, f.first());
    f = e;
    ASSERT_EQ(0.0, f.first());

    using adapt = non_empty_adapter<dhorn::tests::object_counter>;

    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p1;
    dhorn::compressed_pair<adapt, adapt> p2;
    p2 = p1;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<empty, dhorn::tests::object_counter> p3;
    dhorn::compressed_pair<empty, adapt> p4;
    p4 = p3;
    ASSERT_EQ(3_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<dhorn::tests::object_counter, empty> p5;
    dhorn::compressed_pair<adapt, empty> p6;
    p6 = p5;
    ASSERT_EQ(4_sz, dhorn::tests::object_counter::copy_count);
}

struct MoveAssignmentTester
{
    template <typename To, typename From, bool Expect>
    void run() const
    {
        ASSERT_EQ(Expect, (std::is_assignable_v<To&, From&&>));
    }
};

TEST_F(CompressedPairTests, MoveAssignmentTest)
{
    DoAssignmentTest(MoveAssignmentTester{});

    dhorn::compressed_pair<int, int> a(0, 1);
    dhorn::compressed_pair<int, int> b(2, 3);
    ASSERT_EQ(2, b.first());
    ASSERT_EQ(3, b.second());
    b = std::move(a);
    ASSERT_EQ(0, b.first());
    ASSERT_EQ(1, b.second());

    dhorn::compressed_pair<empty, int> c({}, 0);
    dhorn::compressed_pair<empty, int> d({}, 1);
    ASSERT_EQ(1, d.second());
    d = std::move(c);
    ASSERT_EQ(0, d.second());

    dhorn::compressed_pair<int, empty> e(0, {});
    dhorn::compressed_pair<int, empty> f(1, {});
    ASSERT_EQ(1, f.first());
    f = std::move(e);
    ASSERT_EQ(0, f.first());

    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p1;
    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p2;
    p2 = std::move(p1);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<non_empty, dhorn::tests::object_counter> p3;
    dhorn::compressed_pair<non_empty, dhorn::tests::object_counter> p4;
    p4 = std::move(p3);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty> p5;
    dhorn::compressed_pair<dhorn::tests::object_counter, non_empty> p6;
    p6 = std::move(p5);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedPairTests, MoveConversionAssignmentTest)
{
    DoConversionAssignmentTest(MoveAssignmentTester{});

    dhorn::compressed_pair<int, int> a(0, 1);
    dhorn::compressed_pair<double, double> b(2, 3);
    ASSERT_EQ(2.0, b.first());
    ASSERT_EQ(3.0, b.second());
    b = std::move(a);
    ASSERT_EQ(0.0, b.first());
    ASSERT_EQ(1.0, b.second());

    dhorn::compressed_pair<empty, int> c({}, 0);
    dhorn::compressed_pair<empty, double> d({}, 1);
    ASSERT_EQ(1.0, d.second());
    d = std::move(c);
    ASSERT_EQ(0.0, d.second());

    dhorn::compressed_pair<int, empty> e(0, {});
    dhorn::compressed_pair<double, empty> f(1, {});
    ASSERT_EQ(1.0, f.first());
    f = std::move(e);
    ASSERT_EQ(0.0, f.first());

    using adapt = non_empty_adapter<dhorn::tests::object_counter>;

    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> p1;
    dhorn::compressed_pair<adapt, adapt> p2;
    p2 = std::move(p1);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<empty, dhorn::tests::object_counter> p3;
    dhorn::compressed_pair<empty, adapt> p4;
    p4 = std::move(p3);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    dhorn::compressed_pair<dhorn::tests::object_counter, empty> p5;
    dhorn::compressed_pair<adapt, empty> p6;
    p6 = std::move(p5);
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedPairTests, SwapTest)
{
    ASSERT_TRUE((std::is_swappable_v<dhorn::compressed_pair<can_copy_empty, can_copy_empty>>));
    ASSERT_TRUE((std::is_swappable_v<dhorn::compressed_pair<can_copy_empty, can_copy_final>>));
    ASSERT_TRUE((std::is_swappable_v<dhorn::compressed_pair<can_copy_empty, can_copy_non_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<can_copy_empty, cannot_copy_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<can_copy_empty, cannot_copy_final>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<can_copy_empty, cannot_copy_non_empty>>));

    ASSERT_TRUE((std::is_swappable_v<dhorn::compressed_pair<can_copy_final, can_copy_empty>>));
    ASSERT_TRUE((std::is_swappable_v<dhorn::compressed_pair<can_copy_final, can_copy_final>>));
    ASSERT_TRUE((std::is_swappable_v<dhorn::compressed_pair<can_copy_final, can_copy_non_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<can_copy_final, cannot_copy_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<can_copy_final, cannot_copy_final>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<can_copy_final, cannot_copy_non_empty>>));

    ASSERT_TRUE((std::is_swappable_v<dhorn::compressed_pair<can_copy_non_empty, can_copy_empty>>));
    ASSERT_TRUE((std::is_swappable_v<dhorn::compressed_pair<can_copy_non_empty, can_copy_final>>));
    ASSERT_TRUE((std::is_swappable_v<dhorn::compressed_pair<can_copy_non_empty, can_copy_non_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<can_copy_non_empty, cannot_copy_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<can_copy_non_empty, cannot_copy_final>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<can_copy_non_empty, cannot_copy_non_empty>>));

    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_empty, can_copy_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_empty, can_copy_final>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_empty, can_copy_non_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_empty, cannot_copy_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_empty, cannot_copy_final>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_empty, cannot_copy_non_empty>>));

    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_final, can_copy_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_final, can_copy_final>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_final, can_copy_non_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_final, cannot_copy_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_final, cannot_copy_final>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_final, cannot_copy_non_empty>>));

    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_non_empty, can_copy_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_non_empty, can_copy_final>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_non_empty, can_copy_non_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_non_empty, cannot_copy_empty>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_non_empty, cannot_copy_final>>));
    ASSERT_FALSE((std::is_swappable_v<dhorn::compressed_pair<cannot_copy_non_empty, cannot_copy_non_empty>>));

    dhorn::compressed_pair<can_copy_non_empty, can_copy_non_empty> p1(1, 2);
    dhorn::compressed_pair<can_copy_non_empty, can_copy_non_empty> p2(3, 4);
    p1.swap(p2);
    ASSERT_EQ(1, p2.first().value);
    ASSERT_EQ(2, p2.second().value);
    ASSERT_EQ(3, p1.first().value);
    ASSERT_EQ(4, p1.second().value);
    swap(p1, p2);
    ASSERT_EQ(1, p1.first().value);
    ASSERT_EQ(2, p1.second().value);
    ASSERT_EQ(3, p2.first().value);
    ASSERT_EQ(4, p2.second().value);

    dhorn::compressed_pair<can_copy_empty, can_copy_non_empty> p3(can_copy_empty{}, 1);
    dhorn::compressed_pair<can_copy_empty, can_copy_non_empty> p4(can_copy_empty{}, 2);
    p3.swap(p4);
    ASSERT_EQ(1, p4.second().value);
    ASSERT_EQ(2, p3.second().value);
    swap(p3, p4);
    ASSERT_EQ(1, p3.second().value);
    ASSERT_EQ(2, p4.second().value);

    dhorn::compressed_pair<can_copy_non_empty, can_copy_empty> p5(1, can_copy_empty{});
    dhorn::compressed_pair<can_copy_non_empty, can_copy_empty> p6(2, can_copy_empty{});
    p5.swap(p6);
    ASSERT_EQ(1, p6.first().value);
    ASSERT_EQ(2, p5.first().value);
    swap(p5, p6);
    ASSERT_EQ(1, p5.first().value);
    ASSERT_EQ(2, p6.first().value);
}

template <typename TestTy>
void DoComparisonTest(const TestTy& test)
{
    dhorn::compressed_pair<int, int> a1(10, 10);
    dhorn::compressed_pair<int, int> a2(10, 10);
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


    dhorn::compressed_pair<int, std::string> b1(42, "foo");
    dhorn::compressed_pair<int, std::string> b2(42, "foo");
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
        ASSERT_EQ(Compare == 0, lhs == rhs);
        ASSERT_EQ(Compare == 0, lhs == rhs);
    }
};

TEST_F(CompressedPairTests, EqualityComparisonTest)
{
    DoComparisonTest(EqualityTester{});
}

struct InEqualityTester
{
    template <int Compare, typename PairTy>
    void run(const PairTy& lhs, const PairTy& rhs) const
    {
        ASSERT_EQ(Compare != 0, lhs != rhs);
    }
};

TEST_F(CompressedPairTests, InEqualityComparisonTest)
{
    DoComparisonTest(InEqualityTester{});
}

struct LessThanTester
{
    template <int Compare, typename PairTy>
    void run(const PairTy& lhs, const PairTy& rhs) const
    {
        ASSERT_EQ(Compare < 0, lhs < rhs);
    }
};

TEST_F(CompressedPairTests, LessThanComparisonTest)
{
    DoComparisonTest(LessThanTester{});
}

struct GreaterThanTester
{
    template <int Compare, typename PairTy>
    void run(const PairTy& lhs, const PairTy& rhs) const
    {
        ASSERT_EQ(Compare > 0, lhs > rhs);
    }
};

TEST_F(CompressedPairTests, GreaterThanComparisonTest)
{
    DoComparisonTest(GreaterThanTester{});
}

struct LessThanOrEqualTester
{
    template <int Compare, typename PairTy>
    void run(const PairTy& lhs, const PairTy& rhs) const
    {
        ASSERT_EQ(Compare <= 0, lhs <= rhs);
    }
};

TEST_F(CompressedPairTests, LessThanOrEqualComparisonTest)
{
    DoComparisonTest(LessThanOrEqualTester{});
}

struct GreaterThanOrEqualTester
{
    template <int Compare, typename PairTy>
    void run(const PairTy& lhs, const PairTy& rhs) const
    {
        ASSERT_EQ(Compare >= 0, lhs >= rhs);
    }
};

TEST_F(CompressedPairTests, GreaterThanOrEqualComparisonTest)
{
    DoComparisonTest(GreaterThanOrEqualTester{});
}

TEST_F(CompressedPairTests, TupleSizeTest)
{
    ASSERT_EQ(2_sz, (std::tuple_size_v<dhorn::compressed_pair<int, int>>));
}

TEST_F(CompressedPairTests, TupleElementTest)
{
    using test_type = dhorn::compressed_pair<int, float>;
    ASSERT_TRUE((std::is_same_v<int, std::tuple_element_t<0, test_type>>));
    ASSERT_TRUE((std::is_same_v<float, std::tuple_element_t<1, test_type>>));
}

TEST_F(CompressedPairTests, GetTest)
{
    using test_type = dhorn::compressed_pair<int, std::string>;
    test_type p;

    // By Index
    ASSERT_TRUE((std::is_same_v<int&, decltype(std::get<0>(p))>));
    ASSERT_TRUE((std::is_same_v<const int&, decltype(std::get<0>(std::as_const(p)))>));
    ASSERT_TRUE((std::is_same_v<int&&, decltype(std::get<0>(std::move(p)))>));
    ASSERT_TRUE((std::is_same_v<const int&&, decltype(std::get<0>(std::move(std::as_const(p))))>));

    ASSERT_TRUE((std::is_same_v<std::string&, decltype(std::get<1>(p))>));
    ASSERT_TRUE((std::is_same_v<const std::string&, decltype(std::get<1>(std::as_const(p)))>));
    ASSERT_TRUE((std::is_same_v<std::string&&, decltype(std::get<1>(std::move(p)))>));
    ASSERT_TRUE((std::is_same_v<const std::string&&, decltype(std::get<1>(std::move(std::as_const(p))))>));

    std::get<0>(p) = 42;
    std::get<1>(p) = "foo";
    ASSERT_EQ(42, p.first());
    ASSERT_TRUE(p.second() == "foo");

    // By Type
    ASSERT_TRUE((std::is_same_v<int&, decltype(std::get<int>(p))>));
    ASSERT_TRUE((std::is_same_v<const int&, decltype(std::get<int>(std::as_const(p)))>));
    ASSERT_TRUE((std::is_same_v<int&&, decltype(std::get<int>(std::move(p)))>));
    ASSERT_TRUE((std::is_same_v<const int&&, decltype(std::get<int>(std::move(std::as_const(p))))>));

    ASSERT_TRUE((std::is_same_v<std::string&, decltype(std::get<std::string>(p))>));
    ASSERT_TRUE((std::is_same_v<const std::string&, decltype(std::get<std::string>(std::as_const(p)))>));
    ASSERT_TRUE((std::is_same_v<std::string&&, decltype(std::get<std::string>(std::move(p)))>));
    ASSERT_TRUE((std::is_same_v<const std::string&&, decltype(std::get<std::string>(std::move(std::as_const(p))))>));

    std::get<int>(p) = 8;
    std::get<std::string>(p) = "bar";
    ASSERT_EQ(8, p.first());
    ASSERT_TRUE(p.second() == "bar");

    // The following fails to compile (as it should)
    //dhorn::compressed_pair<int, int> p2;
    //std::get<int>(p2) = 0;

    // Object counting
    dhorn::compressed_pair<dhorn::tests::object_counter, dhorn::tests::object_counter> pair;

    auto obj = std::get<0>(pair);
    obj = std::get<1>(pair);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::copy_count);

    obj = std::get<0>(std::as_const(pair));
    obj = std::get<1>(std::as_const(pair));
    ASSERT_EQ(4_sz, dhorn::tests::object_counter::copy_count);

    obj = std::get<0>(std::move(pair));
    obj = std::get<1>(std::move(pair));
    ASSERT_EQ(4_sz, dhorn::tests::object_counter::copy_count);

    obj = std::get<0>(std::move(std::as_const(pair)));
    obj = std::get<1>(std::move(std::as_const(pair)));
    ASSERT_EQ(4_sz, dhorn::tests::object_counter::copy_count);
}

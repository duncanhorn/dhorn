/*
 * Duncan Horn
 *
 * ScopeGuardTests.cpp
 *
 * Tests for the scope_guard.h functions
 */

#include <dhorn/scope_guard.h>
#include <gtest/gtest.h>

#include "object_counter.h"

struct ScopeGuardTests : testing::Test
{
    virtual void SetUp() override
    {

        dhorn::tests::object_counter::reset();
    }

    virtual void TearDown() override
    {
        ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::instance_count);
    }
};

TEST_F(ScopeGuardTests, RunOnExitTest)
{
    int x = 0;
    {
        auto fn = dhorn::make_scope_guard([&]() { x = 42; });
        ASSERT_EQ(0, x);
    }
    ASSERT_EQ(42, x);
}

TEST_F(ScopeGuardTests, RunOnExceptionTest)
{
    int x = 0;
    try
    {
        auto fn = dhorn::make_scope_guard([&]() { x = 42; });
        ASSERT_EQ(0, x);

        throw std::exception();
    }
    catch (...)
    {
    }

    ASSERT_EQ(42, x);
}

TEST_F(ScopeGuardTests, CancelTest)
{
    int x = 0;
    {
        auto fn = dhorn::make_scope_guard([&]() { x = 42; });
        ASSERT_EQ(0, x);

        fn.cancel();
    }
    ASSERT_EQ(0, x);
}

TEST_F(ScopeGuardTests, MoveConstructTest)
{
    {
        dhorn::tests::object_counter cnt;
        auto fn = dhorn::make_scope_guard([cnt = std::move(cnt)]() {});
    }

    // Only the initial constructed value should have been a non-move
    ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::copy_count);
    ASSERT_EQ(dhorn::tests::object_counter::constructed_count - 1, dhorn::tests::object_counter::move_count);
}

TEST_F(ScopeGuardTests, CopyConstructorTest)
{
    {
        dhorn::tests::object_counter cnt;
        auto func = [cnt = std::move(cnt)]() {};
        auto fn = dhorn::make_scope_guard(func);
    }

    // Should have been at least one copy
    ASSERT_EQ(static_cast<std::size_t>(1), dhorn::tests::object_counter::copy_count);
}

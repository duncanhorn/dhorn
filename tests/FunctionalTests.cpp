/*
 * Duncan Horn
 *
 * FunctionalTests.cpp
 *
 * Tests for the functional.h header file
 */

#include <dhorn/functional.h>
#include <dhorn/utility.h>
#include <functional>
#include <gtest/gtest.h>
#include <string>

#include "object_counter.h"

using namespace dhorn::literals;
using namespace std::literals;

struct FunctionalTests : testing::Test
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

TEST_F(FunctionalTests, MakeLambdaSharedInstanceTest)
{
    dhorn::tests::object_counter obj;
    auto lambda = dhorn::make_lambda_shared([obj = std::move(obj)]()
    {
    });

    // Should not have made a copy
    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);

    auto moveCnt = dhorn::tests::object_counter::move_count;
    auto objCnt = dhorn::tests::object_counter::instance_count;
    auto constructCnt = dhorn::tests::object_counter::constructed_count;

    // Copying the new lambda should not affect object count
    auto lambdaCopy = lambda;

    ASSERT_EQ(0_sz, dhorn::tests::object_counter::copy_count);
    ASSERT_EQ(moveCnt, dhorn::tests::object_counter::move_count);
    ASSERT_EQ(objCnt, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(constructCnt, dhorn::tests::object_counter::constructed_count);
}

TEST_F(FunctionalTests, MakeLambdaSharedCopyableTest)
{
    // unique_ptr is not copy-able
    auto lambda = dhorn::make_lambda_shared([ptr = std::make_unique<std::string>("foo")]()
    {
    });

    // This is strictly a "does it compile" test
    std::function<void(void)> fn = lambda;
}

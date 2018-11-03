/*
 * Duncan Horn
 *
 * AlgorithmTests.cpp
 *
 * Tests for the algorithm.h functions
 */

#include <dhorn/algorithm.h>
#include <dhorn/utility.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace dhorn::literals;
using namespace std::literals;

TEST(SplitTests, EmptyStringTest)
{
    std::string empty;
    std::vector<std::string> result;

    // Splitting an empty string results in an empty string
    dhorn::split(empty.begin(), empty.end(), std::back_inserter(result), ' ');
    ASSERT_EQ(1_sz, result.size());
    ASSERT_TRUE(result[0].empty());
}

TEST(SplitTests, SingleTargetCharacterTest)
{
    auto str = " "s;
    std::vector<std::string> result;

    // Splitting the target character results in two empty strings
    dhorn::split(str.begin(), str.end(), std::back_inserter(result), ' ');
    ASSERT_EQ(2_sz, result.size());
    ASSERT_TRUE(result[0].empty());
    ASSERT_TRUE(result[1].empty());
}

TEST(SplitTests, SingleNonTargetCharacterTest)
{
    auto str = " "s;
    std::vector<std::string> result;

    // Splitting at something other than the target character should result in the same string
    dhorn::split(str.begin(), str.end(), std::back_inserter(result), '-');
    ASSERT_EQ(static_cast<std::size_t>(1), result.size());
    ASSERT_EQ(str, result[0]);
}

TEST(SplitTests, MultipleTargetCharacterTest)
{
    auto str = "     "s;
    std::vector<std::string> result;

    // Splitting a string of N target characters results in N + 1 empty strings
    dhorn::split(str.begin(), str.end(), std::back_inserter(result), ' ');
    ASSERT_EQ(str.length() + 1, result.size());
    ASSERT_TRUE(std::all_of(result.begin(), result.end(), [](const std::string& s)
    {
        return s.empty();
    }));
}

TEST(SplitTests, MultipleNonTargetCharacterTest)
{
    auto str = "     "s;
    std::vector<std::string> result;

    // Splitting at something other than the target character should result in the same string
    dhorn::split(str.begin(), str.end(), std::back_inserter(result), '-');
    ASSERT_EQ(static_cast<std::size_t>(1), result.size());
    ASSERT_EQ(str, result[0]);
}

TEST(SplitTests, MixedTest)
{
    auto str = "foo,bar,,foobar,"s;
    std::vector<std::string> result;

    dhorn::split(str.begin(), str.end(), std::back_inserter(result), ',');
    ASSERT_EQ(static_cast<std::size_t>(5), result.size());
    ASSERT_EQ("foo"s, result[0]);
    ASSERT_EQ("bar"s, result[1]);
    ASSERT_EQ(""s, result[2]);
    ASSERT_EQ("foobar"s, result[3]);
    ASSERT_EQ(""s, result[4]);
}

TEST(SplitTests, VectorTest)
{
    std::vector<int> vector = { 0, 1, 2, 3, 0, 4, 5, 0, 6 };
    std::vector<std::vector<int>> result;

    dhorn::split(vector.begin(), vector.end(), std::back_inserter(result), 0);
    ASSERT_EQ(static_cast<std::size_t>(4), result.size());
    ASSERT_EQ(static_cast<std::size_t>(0), result[0].size());
    ASSERT_EQ(static_cast<std::size_t>(3), result[1].size());
    ASSERT_EQ(static_cast<std::size_t>(2), result[2].size());
    ASSERT_EQ(static_cast<std::size_t>(1), result[3].size());
}

TEST(SplitTests, ConditionalSplitTest)
{
    auto str = "foo bar cat dog"s;
    std::vector<std::string> result;

    int pos = 0;
    dhorn::split_if(str.begin(), str.end(), std::back_inserter(result), [&](char)
    {
        return ++pos % 4 == 0;
    });

    ASSERT_EQ(static_cast<std::size_t>(4), result.size());
    ASSERT_EQ("foo"s, result[0]);
    ASSERT_EQ("bar"s, result[1]);
    ASSERT_EQ("cat"s, result[2]);
    ASSERT_EQ("dog"s, result[3]);
}

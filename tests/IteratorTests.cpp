/*
 * Duncan Horn
 *
 * IteratorTests.cpp
 *
 * Tests for the iterator.h header
 */

#include <dhorn/iterator.h>
#include <dhorn/utility.h>
#include <gtest/gtest.h>
#include <vector>

using namespace dhorn::literals;
using namespace std::literals;

TEST(ProxyPointerTests, NonConstTest)
{
    dhorn::proxy_pointer<std::string> ptr{ "foo"s };
    ASSERT_EQ(3_sz, ptr->size());
    ASSERT_EQ(3_sz, (*ptr).size());

    ptr->push_back('b');
    (*ptr).append("ar");
    ASSERT_EQ(6_sz, ptr->size());
}

TEST(ProxyPointerTests, ConstTest)
{
    const dhorn::proxy_pointer<std::string> ptr{ "foo"s };
    ASSERT_EQ(3_sz, ptr->size());
    ASSERT_EQ(3_sz, (*ptr).size());
}

TEST(ExclusiveIteratorTests, IncrementTest)
{
    dhorn::exclusive_iterator itr(0);
    ASSERT_EQ(0, *itr);
    ASSERT_EQ(0, *itr++);
    ASSERT_EQ(1, *itr);
    ASSERT_EQ(2, *++itr);
    ASSERT_EQ(2, *itr);
    ASSERT_EQ(10, *(itr + 8));
    ASSERT_EQ(2, *itr);
    ASSERT_EQ(10, *(itr += 8));
    ASSERT_EQ(10, *itr);
}

TEST(ExclusiveIteratorTests, DecrementTest)
{
    dhorn::exclusive_iterator itr(42);
    ASSERT_EQ(42, *itr);
    ASSERT_EQ(42, *itr--);
    ASSERT_EQ(41, *itr);
    ASSERT_EQ(40, *--itr);
    ASSERT_EQ(40, *itr);
    ASSERT_EQ(30, *(itr - 10));
    ASSERT_EQ(40, *itr);
    ASSERT_EQ(30, *(itr -= 10));
    ASSERT_EQ(30, *itr);
}

TEST(ExclusiveIteratorTests, IndexingTest)
{
    constexpr dhorn::exclusive_iterator itr(0);
    ASSERT_EQ(0, itr[0]);
    ASSERT_EQ(8, itr[8]);
}

TEST(ExclusiveIteratorTests, ComparisonTest)
{
    constexpr dhorn::exclusive_iterator itr0(0);
    constexpr dhorn::exclusive_iterator itr8(8);
    constexpr dhorn::exclusive_iterator itr8_2(8);
    ASSERT_TRUE(itr0 == itr0);
    ASSERT_FALSE(itr0 == itr8);
    ASSERT_FALSE(itr8 == itr0);
    ASSERT_TRUE(itr8 == itr8_2);

    ASSERT_FALSE(itr0 != itr0);
    ASSERT_TRUE(itr0 != itr8);
    ASSERT_TRUE(itr8 != itr0);
    ASSERT_FALSE(itr8 != itr8_2);

    ASSERT_FALSE(itr0 < itr0);
    ASSERT_TRUE(itr0 < itr8);
    ASSERT_FALSE(itr8 < itr0);
    ASSERT_FALSE(itr8 < itr8_2);

    ASSERT_TRUE(itr0 <= itr0);
    ASSERT_TRUE(itr0 <= itr8);
    ASSERT_FALSE(itr8 <= itr0);
    ASSERT_TRUE(itr8 <= itr8_2);

    ASSERT_FALSE(itr0 > itr0);
    ASSERT_FALSE(itr0 > itr8);
    ASSERT_TRUE(itr8 > itr0);
    ASSERT_FALSE(itr8 > itr8_2);

    ASSERT_TRUE(itr0 >= itr0);
    ASSERT_FALSE(itr0 >= itr8);
    ASSERT_TRUE(itr8 >= itr0);
    ASSERT_TRUE(itr8 >= itr8_2);
}

TEST(ExclusiveIteratorTests, IterationTest)
{
    dhorn::exclusive_iterator begin(0u);
    dhorn::exclusive_iterator end(10u);
    for (unsigned int i = 0; i < 10; ++i)
    {
        ASSERT_EQ(i, *begin);
        ASSERT_TRUE(begin != end);
        ++begin;
    }
    ASSERT_TRUE(begin == end);
}

TEST(ExclusiveRangeTests, DistanceTest)
{
    constexpr dhorn::exclusive_range range(0, 10);
    constexpr auto dist = std::distance(range.begin(), range.end());
    ASSERT_EQ(10, dist);

    ASSERT_EQ(10, std::distance(range.rbegin(), range.rend()));
}

TEST(ExclusiveRangeTests, ForwardIterationTest)
{
    int expected = 0;
    for (auto value : dhorn::exclusive_range(0, 10))
    {
        ASSERT_EQ(expected++, value);
    }

    ASSERT_EQ(10, expected);
}

TEST(ExclusiveRangeTests, ReverseIterationTest)
{
    dhorn::exclusive_range range(0, 10);
    int expected = 9;
    std::for_each(range.rbegin(), range.rend(), [&](int value)
    {
        ASSERT_EQ(expected--, value);
    });

    ASSERT_EQ(-1, expected);
}

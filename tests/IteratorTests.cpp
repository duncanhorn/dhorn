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

TEST(ExclusiveIntegerRangeTests, EmptyRangeTest)
{
    constexpr auto range = dhorn::exclusive_range(0, 0);
    constexpr auto distance = std::distance(range.begin(), range.end());
    ASSERT_EQ(0ll, distance);

    std::vector<int> v(range.begin(), range.end());
    ASSERT_EQ(0_sz, v.size());

    constexpr auto indexRange = dhorn::index_range(0);
    constexpr auto indexDistance = std::distance(indexRange.begin(), indexRange.end());
    ASSERT_EQ(0ll, indexDistance);

    std::vector<std::size_t> v2(indexRange.begin(), indexRange.end());
    ASSERT_EQ(0_sz, v2.size());
}

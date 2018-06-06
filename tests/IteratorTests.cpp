/*
 * Duncan Horn
 *
 * IteratorTests.cpp
 *
 * Tests for the iterator.h header
 */
#include "stdafx.h"

#include <dhorn/iterator.h>
#include <dhorn/utility.h>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace dhorn::literals;
using namespace std::literals;

namespace dhorn::tests
{
    TEST_CLASS(ExclusiveIntegerRangeTests)
    {
        TEST_METHOD(EmptyRangeTest)
        {
            constexpr auto range = exclusive_range(0, 0);
            constexpr auto distance = std::distance(range.begin(), range.end());
            Assert::AreEqual(0ll, distance);

            std::vector<int> v(range.begin(), range.end());
            Assert::AreEqual(0_sz, v.size());

            constexpr auto indexRange = index_range(0);
            constexpr auto indexDistance = std::distance(indexRange.begin(), indexRange.end());
            Assert::AreEqual(0ll, indexDistance);

            std::vector<std::size_t> v2(indexRange.begin(), indexRange.end());
            Assert::AreEqual(0_sz, v2.size());
        }
    };
}

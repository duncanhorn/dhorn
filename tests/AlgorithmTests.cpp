/*
 * Duncan Horn
 *
 * AlgorithmTests.cpp
 *
 * Tests for the algorithm.h functions
 */
#include "stdafx.h"

#include <dhorn/algorithm.h>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::literals;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(SplitTests)
        {
            TEST_METHOD(EmptyStringTest)
            {
                std::string empty;
                std::vector<std::string> result;

                // Splitting an empty string results in an empty string
                split(empty.begin(), empty.end(), std::back_inserter(result), ' ');
                Assert::AreEqual(1u, result.size());
                Assert::IsTrue(result[0].empty());
            }

            TEST_METHOD(SingleTargetCharacterTest)
            {
                auto str = " "s;
                std::vector<std::string> result;

                // Splitting the target character results in two empty strings
                split(str.begin(), str.end(), std::back_inserter(result), ' ');
                Assert::AreEqual(2u, result.size());
                Assert::IsTrue(result[0].empty());
                Assert::IsTrue(result[1].empty());

            }

            TEST_METHOD(SingleNonTargetCharacterTest)
            {
                auto str = " "s;
                std::vector<std::string> result;

                // Splitting at something other than the target character should result in the same string
                split(str.begin(), str.end(), std::back_inserter(result), '-');
                Assert::AreEqual(1u, result.size());
                Assert::AreEqual(str, result[0]);
            }

            TEST_METHOD(MultipleTargetCharacterTest)
            {
                auto str = "     "s;
                std::vector<std::string> result;

                // Splitting a string of N target characters results in N + 1 empty strings
                split(str.begin(), str.end(), std::back_inserter(result), ' ');
                Assert::AreEqual(str.length() + 1, result.size());
                Assert::IsTrue(std::all_of(result.begin(), result.end(), [](const std::string& s)
                {
                    return s.empty();
                }));
            }

            TEST_METHOD(MultipleNonTargetCharacterTest)
            {
                auto str = "     "s;
                std::vector<std::string> result;

                // Splitting at something other than the target character should result in the same string
                split(str.begin(), str.end(), std::back_inserter(result), '-');
                Assert::AreEqual(1u, result.size());
                Assert::AreEqual(str, result[0]);
            }

            TEST_METHOD(MixedTest)
            {
                auto str = "foo,bar,,foobar,"s;
                std::vector<std::string> result;

                split(str.begin(), str.end(), std::back_inserter(result), ',');
                Assert::AreEqual(5u, result.size());
                Assert::AreEqual("foo"s, result[0]);
                Assert::AreEqual("bar"s, result[1]);
                Assert::AreEqual(""s, result[2]);
                Assert::AreEqual("foobar"s, result[3]);
                Assert::AreEqual(""s, result[4]);
            }

            TEST_METHOD(VectorTest)
            {
                std::vector<int> vector = { 0, 1, 2, 3, 0, 4, 5, 0, 6 };
                std::vector<std::vector<int>> result;

                split(vector.begin(), vector.end(), std::back_inserter(result), 0);
                Assert::AreEqual(4u, result.size());
                Assert::AreEqual(0u, result[0].size());
                Assert::AreEqual(3u, result[1].size());
                Assert::AreEqual(2u, result[2].size());
                Assert::AreEqual(1u, result[3].size());
            }

            TEST_METHOD(ConditionalSplitTest)
            {
                auto str = "foo bar cat dog"s;
                std::vector<std::string> result;

                int pos = 0;
                split_if(str.begin(), str.end(), std::back_inserter(result), [&](char)
                {
                    return ++pos % 4 == 0;
                });

                Assert::AreEqual(4u, result.size());
                Assert::AreEqual("foo"s, result[0]);
                Assert::AreEqual("bar"s, result[1]);
                Assert::AreEqual("cat"s, result[2]);
                Assert::AreEqual("dog"s, result[3]);
            }
        };
    }
}

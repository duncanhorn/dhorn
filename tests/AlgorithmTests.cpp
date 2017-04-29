/*
 * Duncan Horn
 *
 * AlgorithmTests.cpp
 *
 * Tests for the algorithm.h functions
 */
#include "stdafx.h"

#include <dhorn/algorithm.h>
#include <dhorn/experimental/algorithm.h>
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
                split(std::begin(empty), std::end(empty), std::back_inserter(result), ' ');
                Assert::AreEqual(1u, result.size());
                Assert::IsTrue(result[0].empty());
            }

            TEST_METHOD(SingleTargetCharacterTest)
            {
                auto str = " "s;
                std::vector<std::string> result;

                // Splitting the target character results in two empty strings
                split(std::begin(str), std::end(str), std::back_inserter(result), ' ');
                Assert::AreEqual(2u, result.size());
                Assert::IsTrue(result[0].empty());
                Assert::IsTrue(result[1].empty());

            }

            TEST_METHOD(SingleNonTargetCharacterTest)
            {
                auto str = " "s;
                std::vector<std::string> result;

                // Splitting at something other than the target character should result in the same string
                split(std::begin(str), std::end(str), std::back_inserter(result), '-');
                Assert::AreEqual(1u, result.size());
                Assert::AreEqual(str, result[0]);
            }

            TEST_METHOD(MultipleTargetCharacterTest)
            {
                auto str = "     "s;
                std::vector<std::string> result;

                // Splitting a string of N target characters results in N + 1 empty strings
                split(std::begin(str), std::end(str), std::back_inserter(result), ' ');
                Assert::AreEqual(str.length() + 1, result.size());
                Assert::IsTrue(std::all_of(std::begin(result), std::end(result), [](const std::string& s)
                {
                    return s.empty();
                }));
            }

            TEST_METHOD(MultipleNonTargetCharacterTest)
            {
                auto str = "     "s;
                std::vector<std::string> result;

                // Splitting at something other than the target character should result in the same string
                split(std::begin(str), std::end(str), std::back_inserter(result), '-');
                Assert::AreEqual(1u, result.size());
                Assert::AreEqual(str, result[0]);
            }

            TEST_METHOD(MixedTest)
            {
                auto str = "foo,bar,,foobar,"s;
                std::vector<std::string> result;

                split(std::begin(str), std::end(str), std::back_inserter(result), ',');
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

                split(std::begin(vector), std::end(vector), std::back_inserter(result), 0);
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
                split_if(std::begin(str), std::end(str), std::back_inserter(result), [&](char)
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
















        //
        // EXPERIMENTAL TESTS
        //

        struct foo
        {
            foo(int value) : val(value) {}
            foo &operator=(int value) { this->val = value; return *this; }

            bool operator==(const foo &o) { return val == o.val; }
            bool operator!=(const foo &o) { return !(*this == o); }

            int val;
        };

        constexpr bool operator<(const foo& lhs, const foo& rhs) { return lhs.val < rhs.val; }
        constexpr bool operator>(const foo& lhs, const foo& rhs) { return lhs.val > rhs.val; }

        TEST_CLASS(MaxTests)
        {
            template <typename ResultType, typename... Args>
            void DoTest(ResultType expectedValue, Args &&...args)
            {
                auto val = dhorn::experimental::max(std::forward<Args>(args)...);
                Assert::IsTrue(expectedValue == val);
                Assert::IsTrue(std::is_same<decltype(val), ResultType>::value);
            }

            TEST_METHOD(IntegerTest)
            {
                DoTest<int>(8, 8);
                DoTest<int>(2, 1, 2);
                DoTest<int>(2, 2, 1);
                DoTest<int>(5, 1, 2, 3, 4, 5);
                DoTest<int>(5, 5, 4, 3, 2, 1);
            }

            TEST_METHOD(DoubleTest)
            {
                DoTest<double>(8.0, 8.0);
                DoTest<double>(2.0, 1.0, 2.0);
                DoTest<double>(2.0, 2.0, 1.0);
                DoTest<double>(5.0, 1.0, 2.0, 3.0, 4.0, 5.0);
                DoTest<double>(5.0, 5.0, 4.0, 3.0, 2.0, 1.0);
            }

            TEST_METHOD(MixedTypeTest)
            {
                // int and double
                DoTest<double>(2.0, 1, 2.0);
                DoTest<double>(2.0, 1.0, 2);

                // int and float
                DoTest<float>(2.0f, 1, 2.0f);
                DoTest<float>(2.0f, 1.0f, 2);

                // float and double
                DoTest<double>(2.0, 1.0f, 2.0);
                DoTest<double>(2.0, 1.0, 2.0f);

                // Mixed int/float/double
                DoTest<double>(9.0, 1, 2.0f, 3.0, 4.0, 5.0f, 6, 7.0, 8.0f, 9);
            }

            TEST_METHOD(CustomTypeTest)
            {
                // Single value test
                DoTest<foo>(2, foo{ 2 });

                // Two values, same type
                DoTest<foo>(2, foo{ 1 }, foo{ 2 });
                DoTest<foo>(2, foo{ 2 }, foo{ 1 });

                // Two values, different type
                DoTest<foo>(2, 1, foo{ 2 });
                DoTest<foo>(2, foo{ 1 }, 2);

                // Mixed/multiple
                DoTest<foo>('7', 1, '2', foo{ 3 }, foo{ 4 }, 5, '6', '7', foo{ 8 });
            }
        };

        TEST_CLASS(MinTests)
        {
            template <typename ResultType, typename... Args>
            void DoTest(ResultType expectedValue, Args &&...args)
            {
                auto val = dhorn::experimental::min(std::forward<Args>(args)...);
                Assert::IsTrue(expectedValue == val);
                Assert::IsTrue(std::is_same<decltype(val), ResultType>::value);
            }

            TEST_METHOD(IntegerTest)
            {
                DoTest<int>(8, 8);
                DoTest<int>(1, 1, 2);
                DoTest<int>(1, 2, 1);
                DoTest<int>(1, 1, 2, 3, 4, 5);
                DoTest<int>(1, 5, 4, 3, 2, 1);
            }

            TEST_METHOD(DoubleTest)
            {
                DoTest<double>(8.0, 8.0);
                DoTest<double>(1.0, 1.0, 2.0);
                DoTest<double>(1.0, 2.0, 1.0);
                DoTest<double>(1.0, 1.0, 2.0, 3.0, 4.0, 5.0);
                DoTest<double>(1.0, 5.0, 4.0, 3.0, 2.0, 1.0);
            }

            TEST_METHOD(MixedTypeTest)
            {
                // int and double
                DoTest<double>(1.0, 1, 2.0);
                DoTest<double>(1.0, 1.0, 2);

                // int and float
                DoTest<float>(1.0f, 1, 2.0f);
                DoTest<float>(1.0f, 1.0f, 2);

                // float and double
                DoTest<double>(1.0, 1.0f, 2.0);
                DoTest<double>(1.0, 1.0, 2.0f);

                // Mixed int/float/double
                DoTest<double>(1.0, 1, 2.0f, 3.0, 4.0, 5.0f, 6, 7.0, 8.0f, 9);
            }

            TEST_METHOD(CustomTypeTest)
            {
                // Single value test
                DoTest<foo>(2, foo{ 2 });

                // Two values, same type
                DoTest<foo>(1, foo{ 1 }, foo{ 2 });
                DoTest<foo>(1, foo{ 2 }, foo{ 1 });

                // Two values, different type
                DoTest<foo>(1, 1, foo{ 2 });
                DoTest<foo>(1, foo{ 1 }, 2);

                // Mixed/multiple
                DoTest<foo>(1, 1, '2', foo{ 3 }, foo{ 4 }, 5, '6', '7', foo{ 8 });
            }
        };
    }
}

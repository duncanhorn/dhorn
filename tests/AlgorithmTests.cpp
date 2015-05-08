/*
 * AlgorithmTests.cpp
 *
 * Duncan Horn
 *
 * Tests for the algorithms.h functions
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/algorithm.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        struct foo
        {
            foo(int val) : val(val) {}
            foo &operator=(int val) { this->val = val; return *this; }

            bool operator==(foo &o) { return val == o.val; }
            bool operator!=(foo &o) { return !(*this == o); }

            int val;
        };

        bool operator<(foo lhs, foo rhs) { return lhs.val < rhs.val; }
        bool operator>(foo lhs, foo rhs) { return lhs.val > rhs.val; }

        TEST_CLASS(MaxTests)
        {
            template <typename ResultType, typename... Args>
            void DoTest(_In_ ResultType expectedValue, _In_ Args&&... args)
            {
                auto val = dhorn::max(std::forward<Args>(args)...);
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
            }
        };
    }
}

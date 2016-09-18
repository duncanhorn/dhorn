/*
 * Duncan Horn
 *
 * FunctionalTest.cpp
 *
 * Tests for functional.h
 */
#include "stdafx.h"

#include <functional>

#include <dhorn/experimental/functional.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        struct test_class
        {
            int do_multiply(void)
            {
                return val0 * val1;
            }

            int add(int lhs, int rhs)
            {
                return lhs + rhs;
            }

            int min(int lhs, int rhs)
            {
                return (lhs < rhs) ? lhs : rhs;
            }

            int val0;
            int val1;
        };

        TEST_CLASS(FunctionalTests)
        {
            TEST_METHOD(BindMemberFunctionTest)
            {
                test_class obj{ 10, 5 };
                test_class obj2{ 4, 5 };
                test_class *pObj = &obj;

                // Test the do_multiply function
                std::function<int(void)> mult;
                mult = dhorn::bind_member_function(&test_class::do_multiply, obj);
                Assert::AreEqual(mult(), 50);
                obj.val0 = 1;
                Assert::AreEqual(mult(), 5);

                mult = dhorn::bind_member_function(&test_class::do_multiply, obj2);
                Assert::AreEqual(mult(), 20);

                mult = dhorn::bind_member_function(&test_class::do_multiply, pObj);
                Assert::AreEqual(mult(), 5);
                obj.val1 = 50;
                Assert::AreEqual(mult(), 50);

                // Test the add function
                std::function<int(int, int)> add;
                add = dhorn::bind_member_function(&test_class::add, obj);
                Assert::AreEqual(add(2, 5), 7);
                add = dhorn::bind_member_function(&test_class::add, pObj);
                Assert::AreEqual(add(20, 30), 50);

                // Test the min function (with the current add object)
                add = dhorn::bind_member_function(&test_class::min, obj);
                Assert::AreEqual(add(10, 5), 5);
            }
        };
    }
}

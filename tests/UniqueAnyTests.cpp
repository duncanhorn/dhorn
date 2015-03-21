/*
 * UniqueAnyTests.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn unique_any class
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include "dhorn/unique_any.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        static int count = 0;

        TEST_CLASS(UniqueAnyTests)
        {
            class test_class
            {
            public:
                test_class()
                {
                    ++count;
                }

                test_class(const test_class &)
                {
                    ++count;
                }

                ~test_class()
                {
                    --count;
                }

                bool operator!=(const test_class &) const { return false; }
                bool operator==(const test_class &) const { return false; }
            };

            TEST_METHOD_INITIALIZE(InitializeTest)
            {
                // Reset the count before each test invocation
                count = 0;
            }

            TEST_METHOD(PointerTest)
            {
                {
                    // Default construct should not affect the count
                    dhorn::unique_any<test_class *> ptr;
                    Assert::AreEqual(0, count);

                    ptr = new test_class();
                    Assert::AreEqual(1, count);

                    // Resetting the value should destroy the previous one
                    ptr = new test_class();
                    Assert::AreEqual(1, count);
                    ptr.reset(new test_class());
                    Assert::AreEqual(1, count);
                }

                // Should have released all resources
                Assert::AreEqual(0, count);
            }

            TEST_METHOD(ArrayTest)
            {
                {
                    // Default construct should not affect the count
                    dhorn::unique_any<test_class[]> ptr;
                    Assert::AreEqual(0, count);

                    // Calling new[] with a size of 5 should mean that count is now 5 as well
                    ptr = new test_class[5];
                    Assert::AreEqual(5, count);

                    // Resetting should call the destructor for *all* elements in the array
                    ptr = new test_class[6];
                    Assert::AreEqual(6, count);
                    ptr.reset(new test_class[2]);
                    Assert::AreEqual(2, count);
                }

                // Should have released all resources
                Assert::AreEqual(0, count);
            }

            TEST_METHOD(ValueTest)
            {
                std::vector<test_class> arr(5);
                Assert::AreEqual(5, count);

                {
                    dhorn::unique_any<std::vector<test_class>> val;
                    Assert::AreEqual(5, count);

                    // Assignment should copy here (i.e. it acts like a normal non-unique value)
                    val = arr;
                    Assert::AreEqual(10, count);

                    // Moving should transfer all ownership, though
                    val.reset(std::move(arr));
                    Assert::AreEqual(5, count);
                }

                // Should have released all resources
                Assert::AreEqual(0, count);
            }

            TEST_METHOD(UniqueHandleTest)
            {

            }
        };
    }
}

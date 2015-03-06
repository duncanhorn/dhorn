/*
 * TaskPoolTests.cpp
 *
 * Duncan Horn
 *
 * Tests for dhorn::task_pool
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <atomic>

#include "dhorn/task_pool.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(TaskPoolTests)
        {
            template <size_t TestCount, size_t ThreadPoolSize>
            void DoSimpleTest()
            {
                dhorn::task_pool taskPool(ThreadPoolSize);
                std::atomic_int x{};

                for (size_t i = 0; i < TestCount; ++i)
                {
                    taskPool.submit([&]()
                    {
                        ++x;
                    });
                }

                taskPool.shutdown();
                Assert::IsTrue(x == TestCount);
            }

            TEST_METHOD(SingleThreadSimpleTest)
            {
                DoSimpleTest<1000, 1>();
            }

            TEST_METHOD(MultiThreadSimpleTest)
            {
                DoSimpleTest<1000, 10>();
            }

            template <size_t TestCount, size_t ThreadPoolSize, size_t TestPoolSize>
            void DoFutureTest()
            {
                static_assert(TestCount % 2 == 0, "TestCount must be even");
                dhorn::task_pool taskPool(ThreadPoolSize);
                dhorn::task_pool testPool(TestPoolSize);
                std::atomic_int x{};

                for (size_t i = 0; i < TestCount / 2; ++i)
                {
                    for (size_t j = 0; j < TestCount; ++j)
                    {
                        testPool.submit([&x, &taskPool, i, j]()
                        {
                            auto future = taskPool.submit_for_result([&x, i, j]() -> int
                            {
                                ++x;
                                return i + j;
                            });

                            Assert::IsTrue(future.get() == static_cast<int>(i + j));
                        });
                    }
                }

                testPool.shutdown();
                taskPool.shutdown();
                Assert::IsTrue(x == TestCount);
            }

            TEST_METHOD(SingleThreadFutureTest)
            {
                DoFutureTest<1000, 1, 1>();
            }
        };
    }
}

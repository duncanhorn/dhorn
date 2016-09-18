/*
 * Duncan Horn
 *
 * TaskPoolTests.cpp
 *
 * Tests for dhorn::task_pool
 */
#include "stdafx.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <random>

#include <dhorn/experimental/task_pool.h>

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
                std::atomic_size_t x{};

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
                DoSimpleTest<1000, 4>();
            }

            template <size_t TestCount, size_t ThreadPoolSize, size_t TestPoolSize>
            void DoFutureTest()
            {
                dhorn::task_pool taskPool(ThreadPoolSize);
                dhorn::task_pool testPool(TestPoolSize);
                std::atomic_size_t x{};

                size_t first_pass = static_cast<size_t>(std::sqrt(TestCount));
                size_t second_pass = (TestCount / first_pass);

                for (size_t i = 0; i < first_pass; ++i)
                {
                    for (size_t j = 0; j < second_pass; ++j)
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
                Assert::AreEqual(x.load(), first_pass * second_pass);
            }

            TEST_METHOD(SingleProducerSingleConsumerFutureTest)
            {
                DoFutureTest<1000, 1, 1>();
            }

            TEST_METHOD(SingleProducerMultiConsumerFutureTest)
            {
                DoFutureTest<1000, 4, 1>();
            }

            TEST_METHOD(MultiProducerSingleConsumerFutureTest)
            {
                DoFutureTest<1000, 1, 4>();
            }

            TEST_METHOD(MultiProducerMultiConsumerFutureTest)
            {
                DoFutureTest<1000, 4, 4>();
            }

            TEST_METHOD(BarrierTest)
            {
                const size_t test_pool_size = 50; // To test against deadlock

                dhorn::task_pool testPool(test_pool_size);
                dhorn::task_pool taskPool(4);

                // Repeat test 20 times to be safe
                for (size_t i = 0; i < 20; ++i)
                {
                    std::atomic_size_t x{};

                    for (size_t j = 0; j < test_pool_size; ++j)
                    {
                        testPool.submit([&]()
                        {
                            // Fill the task pool up with some semi-long to complete tasks
                            for (size_t k = 0; k < 50; ++k)
                            {
                                taskPool.submit([]()
                                {
                                    double val = 20;
                                    val = sqrt(val) + sqrt(val);
                                    Assert::IsTrue(val != 0); // Ensure no optimizations
                                });
                            }

                            taskPool.barrier();
                            ++x;
                        });
                    }

                    testPool.barrier();
                    Assert::AreEqual(x.load(), test_pool_size);
                }

                testPool.shutdown();
                taskPool.shutdown();
            }
        };
    }
}

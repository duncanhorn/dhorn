/*
 * Duncan Horn
 *
 * MessageQueueTests.cpp
 *
 * Tests for message_queue<void(void)>
 */
#include "stdafx.h"

#include <atomic>
#include <numeric>
#include <thread>
#include <vector>

#include <dhorn/experimental/message_queue.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(MessageQueueTests)
        {
            TEST_METHOD(SingleThreadTest)
            {
                const std::size_t testCount = 100;
                dhorn::experimental::message_queue<int(int, int)> msgQueue;
                int x = 0;

                // Insert data
                for (std::size_t i = 0; i < testCount; ++i)
                {
                    msgQueue.push_back([&](int a, int b) -> int
                    {
                        ++x;
                        return a + b;
                    });
                }

                Assert::IsTrue(x == 0);

                // Remove data
                int localCount = 0;
                for (std::size_t i = 0; i < testCount; ++i)
                {
                    Assert::IsTrue(msgQueue.pop_front()(i, 1) == static_cast<int>(i + 1));
                    Assert::IsTrue(x == ++localCount);
                }
            }

            TEST_METHOD(SingleProducerSingleConsumerTest)
            {
                const std::size_t testCount = 100;
                dhorn::experimental::message_queue<void(void)> msgQueue;
                int x = 0;

                std::thread producer([&]()
                {
                    for (std::size_t i = 0; i < testCount; ++i)
                    {
                        msgQueue.push_back([&]()
                        {
                            ++x;
                        });
                    }
                });

                // The test thread is the consumer
                int localCount = 0;
                for (std::size_t i = 0; i < testCount; ++i)
                {
                    msgQueue.pop_front()();
                    Assert::IsTrue(x == ++localCount);
                }

                producer.join();
            }

            TEST_METHOD(MultipleProducersSingleConsumerTest)
            {
                const std::size_t testCount = 1000;
                const std::size_t producerCount = 20;
                dhorn::experimental::message_queue<void(void)> msgQueue;
                int counts[producerCount] = {};

                std::vector<std::thread> producers;
                for (std::size_t i = 0; i < producerCount; ++i)
                {
                    producers.emplace_back([i, testCount, &msgQueue, &counts]()
                    {
                        for (std::size_t j = 0; j < testCount; ++j)
                        {
                            msgQueue.push_back([&counts, i]()
                            {
                                ++counts[i];
                            });
                        }
                    });
                }

                int localCount = 0;
                for (std::size_t i = 0; i < testCount * producerCount; ++i)
                {
                    msgQueue.pop_front()();

                    int count = std::accumulate(std::begin(counts), std::end(counts), 0);
                    Assert::IsTrue(count == ++localCount);
                }

                int index = 0;
                for (auto &thread : producers)
                {
                    thread.join();
                    Assert::IsTrue(counts[index++] == testCount);
                }
            }

            TEST_METHOD(SingleProducerMultipleConsumersTest)
            {
                const std::size_t testCount = 5000;
                const std::size_t consumerCount = 20;
                static_assert(testCount % consumerCount == 0, "Must be divisible");
                dhorn::experimental::message_queue<void(void)> msgQueue;
                std::atomic_int x{};

                std::vector<std::thread> consumers;
                for (int i = 0; i < consumerCount; ++i)
                {
                    consumers.emplace_back([&]()
                    {
                        for (std::size_t j = 0; j < testCount / consumerCount; ++j)
                        {
                            msgQueue.pop_front()();
                        }
                    });
                }

                for (std::size_t i = 0; i < testCount; ++i)
                {
                    msgQueue.push_back([&]()
                    {
                        ++x;
                    });
                }

                for (auto &thread : consumers)
                {
                    thread.join();
                }
                Assert::IsTrue(static_cast<std::size_t>(x) == testCount);
            }

            TEST_METHOD(MultipleProducersMultipleConsumersTest)
            {
                const std::size_t testCount = 1000;
                const std::size_t producerCount = 20;
                const std::size_t consumerCount = 20;
                dhorn::experimental::message_queue<void(void)> msgQueue;
                std::atomic_int counts[producerCount] = {};
                std::atomic_int x{};

                std::vector<std::thread> producers;
                for (std::size_t i = 0; i < producerCount; ++i)
                {
                    producers.emplace_back([i, testCount, &msgQueue, &counts, &x]()
                    {
                        for (std::size_t j = 0; j < testCount; ++j)
                        {
                            msgQueue.push_back([&counts, &x, i]()
                            {
                                ++counts[i];
                                ++x;
                            });
                        }
                    });
                }

                std::vector<std::thread> consumers;
                for (int i = 0; i < consumerCount; ++i)
                {
                    consumers.emplace_back([&]()
                    {
                        for (std::size_t j = 0; j < testCount; ++j)
                        {
                            msgQueue.pop_front()();
                        }
                    });
                }

                for (auto &thread : consumers)
                {
                    thread.join();
                }

                int index = 0;
                for (auto &thread : producers)
                {
                    thread.join();
                    Assert::IsTrue(static_cast<std::size_t>(counts[index++]) == testCount);
                }

                Assert::IsTrue(x == (testCount * producerCount));
            }

            TEST_METHOD(TryPopFrontTest)
            {
                const std::size_t testCount = 100;
                dhorn::experimental::message_queue<void(void)> msgQueue;
                int x = 0;

                // Insert data
                for (std::size_t i = 0; i < testCount; ++i)
                {
                    msgQueue.push_back([&]()
                    {
                        ++x;
                    });
                }

                Assert::IsTrue(x == 0);

                std::function<void(void)> func;
                while (msgQueue.try_pop_front(func))
                {
                    func();
                }

                Assert::IsTrue(x == testCount);
            }
        };
    }
}

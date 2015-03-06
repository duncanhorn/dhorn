/*
 * Duncan Horn
 *
 * MessageQueueTests.cpp
 *
 * Tests for message_queue
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <atomic>
#include <numeric>
#include <thread>
#include <vector>

#include "dhorn/message_queue.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(MessageQueueTests)
        {
            TEST_METHOD(SingleThreadTest)
            {
                const size_t testCount = 100;
                dhorn::message_queue msgQueue;
                int x = 0;

                // Insert data
                for (size_t i = 0; i < testCount; ++i)
                {
                    msgQueue.push_back([&]()
                    {
                        ++x;
                    });
                }

                Assert::IsTrue(x == 0);

                // Remove data
                int localCount = 0;
                for (size_t i = 0; i < testCount; ++i)
                {
                    msgQueue.pop_front()();
                    Assert::IsTrue(x == ++localCount);
                }
            }

            TEST_METHOD(SingleProducerSingleConsumerTest)
            {
                const size_t testCount = 100;
                dhorn::message_queue msgQueue;
                int x = 0;

                std::thread producer([&]()
                {
                    for (size_t i = 0; i < testCount; ++i)
                    {
                        msgQueue.push_back([&]()
                        {
                            ++x;
                        });
                    }
                });

                // The test thread is the consumer
                int localCount = 0;
                for (size_t i = 0; i < testCount; ++i)
                {
                    msgQueue.pop_front()();
                    Assert::IsTrue(x == ++localCount);
                }

                producer.join();
            }

            TEST_METHOD(MultipleProducersSingleConsumerTest)
            {
                const size_t testCount = 1000;
                const size_t producerCount = 20;
                dhorn::message_queue msgQueue;
                int counts[producerCount] = {};

                std::vector<std::thread> producers;
                for (size_t i = 0; i < producerCount; ++i)
                {
                    producers.emplace_back([i, testCount, &msgQueue, &counts]()
                    {
                        for (size_t j = 0; j < testCount; ++j)
                        {
                            msgQueue.push_back([&counts, i]()
                            {
                                ++counts[i];
                            });
                        }
                    });
                }

                int localCount = 0;
                for (size_t i = 0; i < testCount * producerCount; ++i)
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
                const size_t testCount = 5000;
                const size_t consumerCount = 20;
                static_assert(testCount % consumerCount == 0, "Must be divisible");
                dhorn::message_queue msgQueue;
                std::atomic_int x{};

                std::vector<std::thread> consumers;
                for (int i = 0; i < consumerCount; ++i)
                {
                    consumers.emplace_back([&]()
                    {
                        for (size_t j = 0; j < testCount / consumerCount; ++j)
                        {
                            msgQueue.pop_front()();
                        }
                    });
                }

                for (size_t i = 0; i < testCount; ++i)
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
                Assert::IsTrue(x == testCount);
            }

            TEST_METHOD(MultipleProducersMultipleConsumersTest)
            {
                const size_t testCount = 1000;
                const size_t producerCount = 20;
                const size_t consumerCount = 20;
                dhorn::message_queue msgQueue;
                std::atomic_int counts[producerCount] = {};
                std::atomic_int x{};

                std::vector<std::thread> producers;
                for (size_t i = 0; i < producerCount; ++i)
                {
                    producers.emplace_back([i, testCount, &msgQueue, &counts, &x]()
                    {
                        for (size_t j = 0; j < testCount; ++j)
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
                        for (size_t j = 0; j < testCount; ++j)
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
                    Assert::IsTrue(counts[index++] == testCount);
                }

                Assert::IsTrue(x == (testCount * producerCount));
            }

            TEST_METHOD(TryPopFrontTest)
            {
                const size_t testCount = 100;
                dhorn::message_queue msgQueue;
                int x = 0;

                // Insert data
                for (size_t i = 0; i < testCount; ++i)
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

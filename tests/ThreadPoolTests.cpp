/*
 * Duncan Horn
 *
 * ThreadPoolTests.cpp
 *
 * Tests for dhorn::experimental::thread_pool
 */
#include "stdafx.h"

#include <array>
#include <dhorn/experimental/thread_pool.h>

using namespace std::literals;

using namespace dhorn::experimental;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(ThreadPoolTests)
    {
        TEST_METHOD(DefaultInitializeTest)
        {
            thread_pool pool;

            Assert::AreEqual(0u, pool.count());
            pool.join();
        }

        TEST_METHOD(SubmitSingleTaskTest)
        {
            thread_pool pool;
            std::mutex mutex;
            int value = 0;

            {
                std::lock_guard<std::mutex> guard(mutex);
                pool.submit([&]()
                {
                    std::lock_guard<std::mutex> guard(mutex);
                    value = 42;
                });

                Assert::AreEqual(1u, pool.count());
                Assert::AreEqual(0, value);
            }

            pool.join();
            Assert::AreEqual(42, value);
        }

        TEST_METHOD(MultipleConcurrentSubmitTest)
        {
            thread_pool pool;
            std::mutex mutex;
            size_t value = 0;

            const size_t loop_count = 100;
            {
                std::lock_guard<std::mutex> guard(mutex);

                for (size_t i = 0; i < loop_count; ++i)
                {
                    pool.submit([&]()
                    {
                        // Lock to ensure that this task does not finish before we queue up the rest
                        std::lock_guard<std::mutex> guard(mutex);
                        ++value;
                    });
                }

                Assert::AreEqual(loop_count, pool.count());
                Assert::AreEqual(0u, value);
            }

            pool.join();
            Assert::AreEqual(loop_count, value);
        }

        TEST_METHOD(MultipleSequentialSubmitTest)
        {
            thread_pool pool;
            std::mutex mutex;
            std::condition_variable cond;

            const size_t loop_count = 100;
            {
                std::unique_lock<std::mutex> lock(mutex);

                for (size_t i = 0; i < loop_count; ++i)
                {
                    pool.submit([&]()
                    {
                        std::lock_guard<std::mutex> guard(mutex);
                        cond.notify_one();
                    });

                    cond.wait(lock);

                    // Since we can't reliably determine task completion, we can't guarantee that only one thread will
                    // ever get created. That said, it's very unlikely that more than two will get created
                    Assert::IsTrue(pool.count() <= 2);
                }
            }

            pool.join();
        }

        TEST_METHOD(MaxWaitingTest)
        {
            thread_pool pool;
            std::mutex mutex;
            std::condition_variable cond;
            size_t count = 0;

            // For the first test, set before submitting tasks
            pool.set_max_available_threads(1);

            const size_t loop_count = 100;
            for (size_t i = 0; i < loop_count; ++i)
            {
                pool.submit([&]()
                {
                    std::lock_guard<std::mutex> guard(mutex);
                    ++count;
                    cond.notify_one();
                });
            }

            {
                std::unique_lock<std::mutex> lock(mutex);
                cond.wait(lock, [&]()
                {
                    return count == loop_count;
                });
            }

            for (size_t i = 0; i < loop_count; ++i)
            {
                if (pool.count() <= 1)
                {
                    break;
                }

                // From experimentation, this test takes ~30ms, so this should be more than enough
                std::this_thread::sleep_for(10ms);
            }

            Assert::AreEqual(1u, pool.count());

            // Now test when setting the max availble threads after submitting all tasks
            pool.set_max_available_threads(100);
            count = 0;

            for (size_t i = 0; i < loop_count; ++i)
            {
                pool.submit([&]()
                {
                    std::lock_guard<std::mutex> guard(mutex);
                    ++count;
                    cond.notify_one();
                });
            }

            pool.set_max_available_threads(1);

            {
                std::unique_lock<std::mutex> lock(mutex);
                cond.wait(lock, [&]()
                {
                    return count == loop_count;
                });
            }

            for (size_t i = 0; i < loop_count; ++i)
            {
                if (pool.count() <= 1)
                {
                    break;
                }

                // From experimentation, this test takes ~30ms, so this should be more than enough
                std::this_thread::sleep_for(10ms);
            }

            Assert::AreEqual(1u, pool.count());

            pool.join();
        }

        TEST_METHOD(MaxThreadsTest)
        {
            thread_pool pool;
            size_t count = 0;

            pool.set_max_threads(1);

            const size_t loop_count = 100;
            for (size_t i = 0; i < loop_count; ++i)
            {
                pool.submit([&]()
                {
                    // No data race since only one thread
                    ++count;
                });

                Assert::AreEqual(1u, pool.count());
            }

            pool.join();
            Assert::AreEqual(loop_count, count);

            // Now with two threads
            pool = thread_pool();
            pool.set_max_threads(2);

            for (size_t i = 0; i < loop_count; ++i)
            {
                pool.submit([&]()
                {
                });

                Assert::IsTrue(pool.count() <= 2);
            }

            Assert::AreEqual(2u, pool.count());

            // Setting max back down to one should terminate a thread
            pool.set_max_threads(1);
            for (size_t i = 0; i < 10; ++i)
            {
                if (pool.count() == 1)
                {
                    break;
                }

                std::this_thread::sleep_for(10ms);
            }

            Assert::AreEqual(1u, pool.count());
            pool.join();
        }

        TEST_METHOD(MinThreadsTest)
        {
            thread_pool pool;

            pool.set_min_threads(1);
            Assert::AreEqual(1u, pool.count());

            pool.set_min_threads(2);
            Assert::AreEqual(2u, pool.count());

            // Setting back down to one shouldn't change the count
            pool.set_min_threads(1);
            Assert::AreEqual(2u, pool.count());

            pool.join();
        }

        TEST_METHOD(SubmitWithPriorityTest)
        {
            thread_pool pool;
            pool.set_max_threads(1);

            std::array<size_t, 6> array;
            size_t index = 0;
            {
                // We need to submit a dummy task to ensure that we don't accidentally start processing a lower priority
                // task before we submit the higher priority ones
                size_t stage = 0;

                std::mutex mutex;
                std::condition_variable cond;
                std::unique_lock<std::mutex> lock(mutex);
                pool.submit([&]()
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    // No need to check the stage; we already know that the main thread is waiting for us
                    ++stage;
                    cond.notify_one();

                    cond.wait(lock, [&]()
                    {
                        return stage == 2;
                    });
                });

                // Wait until we know that the dummy task is running
                cond.wait(lock, [&]()
                {
                    return stage == 1;
                });

                // Submit in this order: NLLHNH
                // Submit in this order: LNNHLH
                pool.submit(thread_pool_priority::low, [&]()
                {
                    array[index++] = 4;
                });
                pool.submit(thread_pool_priority::normal, [&]()
                {
                    array[index++] = 2;
                });
                pool.submit(thread_pool_priority::normal, [&]()
                {
                    array[index++] = 3;
                });
                pool.submit(thread_pool_priority::high, [&]()
                {
                    array[index++] = 0;
                });
                pool.submit(thread_pool_priority::low, [&]()
                {
                    array[index++] = 5;
                });
                pool.submit(thread_pool_priority::high, [&]()
                {
                    array[index++] = 1;
                });

                // Notify the dummy task that it's okay to complete
                ++stage;
                cond.notify_one();
            }

            while (index != array.size())
            {
                std::this_thread::sleep_for(1ms);
            }

            Assert::AreEqual(0u, array[0]);
            Assert::AreEqual(1u, array[1]);
            Assert::AreEqual(2u, array[2]);
            Assert::AreEqual(3u, array[3]);
            Assert::AreEqual(4u, array[4]);
            Assert::AreEqual(5u, array[5]);

            pool.join();
        }

        TEST_METHOD(SingleThreadThreadPoolConstructionTest)
        {
            single_thread_thread_pool pool;
            Assert::AreEqual(1u, pool.count());
            pool.join();
        }

        TEST_METHOD(JoinTest)
        {
            single_thread_thread_pool pool;
            size_t count = 0;
            std::mutex mutex;

            const size_t loop_count = 100;
            {
                std::lock_guard<std::mutex> guard(mutex);
                for (size_t i = 0; i < loop_count; ++i)
                {
                    pool.submit([&]()
                    {
                        std::lock_guard<std::mutex> guard(mutex);
                        ++count;
                    });
                }
            }

            pool.join();

            // Should have had the chance to complete
            Assert::AreEqual(loop_count, count);
        }

        TEST_METHOD(SubmitForResultTest)
        {
            thread_pool pool;

            auto future = pool.submit_for_result([]()
            {
                // Simulate a long-ish running task
                std::this_thread::sleep_for(10ms);
                return 42;
            });

            Assert::AreEqual(42, future.get());

            // If we throw an exception, the caller should be the one to handle it
            future = pool.submit_for_result([]() -> int
            {
                throw std::exception();
            });

            try
            {
                future.get();
                Assert::Fail(L"Expected an exception");
            }
            catch (std::exception&)
            {
            }

            pool.join();
        }

        TEST_METHOD(DontCopyTest)
        {
            object_counter::reset();

            thread_pool pool;

            // Any of the submit functions should not cause a copy to occur
            pool.submit([obj = object_counter{}](){});
            pool.submit(thread_pool_priority::high, [obj = object_counter{}](){});
            pool.submit_for_result([obj = object_counter{}](){});
            pool.submit_for_result(thread_pool_priority::high, [obj = object_counter{}](){});

            // Now test the variants that accept args
            pool.submit([obj = object_counter{}](const object_counter&){}, object_counter{});
            pool.submit(thread_pool_priority::low, [obj = object_counter{}](const object_counter&){}, object_counter{});
            pool.submit_for_result([obj = object_counter{}](const object_counter&){}, object_counter{});
            pool.submit_for_result(thread_pool_priority::low, [obj = object_counter{}](const object_counter&){}, object_counter{});

            pool.join();

            Assert::AreEqual(0u, object_counter::copy_count);

            // All instances should be destroyed, too
            Assert::AreEqual(0u, object_counter::instance_count);
        }

        TEST_METHOD(InvokeWithArgsTest)
        {
            thread_pool pool;
            std::mutex mutex;

            int value = 0;
            std::future<size_t> future;
            {
                std::lock_guard<std::mutex> guard(mutex);

                // Create a string and make sure its destructor is run before the task can make progress so that we can
                // validate that we are passing a copy of the string, not a reference
                auto str = "foobar"s;
                pool.submit([&mutex, &value](const std::string& str)
                {
                    std::lock_guard<std::mutex> guard(mutex);
                    value = str.length();
                }, str);

                future = pool.submit_for_result([&mutex](const std::string& str)
                {
                    std::lock_guard<std::mutex> guard(mutex);
                    return str.length();
                }, str);

                str.clear();
            }

            pool.join();

            Assert::AreEqual(6, value);
            Assert::AreEqual(6u, future.get());
        }
    };
}

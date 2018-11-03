/*
 * Duncan Horn
 *
 * ThreadPoolTests.cpp
 *
 * Tests for dhorn::experimental::thread_pool
 */

#pragma warning(push)
#pragma warning(disable:4702) // Unreachable code since we unconditionally throw

#include <array>
#include <dhorn/thread_pool.h>
#include <gtest/gtest.h>

#include "object_counter.h"

#pragma warning(pop)

using namespace std::literals;

struct ThreadPoolTests : testing::Test
{
    virtual void SetUp() override
    {
        dhorn::tests::object_counter::reset();
    }

    virtual void TearDown() override
    {
        ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::instance_count);
    }
};

TEST_F(ThreadPoolTests, DefaultInitializeTest)
{
    dhorn::thread_pool pool;

    ASSERT_EQ(static_cast<std::size_t>(0), pool.count());
    pool.join();
}

TEST_F(ThreadPoolTests, SubmitSingleTaskTest)
{
    dhorn::thread_pool pool;
    std::mutex mutex;
    int value = 0;

    {
        std::lock_guard<std::mutex> guard(mutex);
        pool.submit([&]()
        {
            std::lock_guard<std::mutex> guard(mutex);
            value = 42;
        });

        ASSERT_EQ(static_cast<std::size_t>(1), pool.count());
        ASSERT_EQ(0, value);
    }

    pool.join();
    ASSERT_EQ(42, value);
}

TEST_F(ThreadPoolTests, MultipleConcurrentSubmitTest)
{
    dhorn::thread_pool pool;
    std::mutex mutex;
    std::size_t value = 0;

    const std::size_t loop_count = 100;
    {
        std::lock_guard<std::mutex> guard(mutex);

        for (std::size_t i = 0; i < loop_count; ++i)
        {
            pool.submit([&]()
            {
                // Lock to ensure that this task does not finish before we queue up the rest
                std::lock_guard<std::mutex> guard(mutex);
                ++value;
            });
        }

        ASSERT_EQ(loop_count, pool.count());
        ASSERT_EQ(static_cast<std::size_t>(0), value);
    }

    pool.join();
    ASSERT_EQ(loop_count, value);
}

TEST_F(ThreadPoolTests, MultipleSequentialSubmitTest)
{
    dhorn::thread_pool pool;
    std::mutex mutex;
    std::condition_variable cond;

    const std::size_t loop_count = 100;
    {
        std::unique_lock<std::mutex> lock(mutex);

        for (std::size_t i = 0; i < loop_count; ++i)
        {
            pool.submit([&]()
            {
                std::lock_guard<std::mutex> guard(mutex);
                cond.notify_one();
            });

            cond.wait(lock);

            // Since we can't reliably determine task completion, we can't guarantee that only one thread will
            // ever get created. That said, it's very unlikely that more than two will get created
            ASSERT_TRUE(pool.count() <= 2);
        }
    }

    pool.join();
}

TEST_F(ThreadPoolTests, MaxWaitingTest)
{
    dhorn::thread_pool pool;
    std::mutex mutex;
    std::condition_variable cond;
    std::size_t count = 0;

    // For the first test, set before submitting tasks
    pool.set_max_available_threads(1);

    const std::size_t loop_count = 100;
    for (std::size_t i = 0; i < loop_count; ++i)
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

    for (std::size_t i = 0; i < loop_count; ++i)
    {
        if (pool.count() <= 1)
        {
            break;
        }

        // From experimentation, this test takes ~30ms, so this should be more than enough
        std::this_thread::sleep_for(10ms);
    }

    ASSERT_EQ(static_cast<std::size_t>(1), pool.count());

    // Now test when setting the max availble threads after submitting all tasks
    pool.set_max_available_threads(100);
    count = 0;

    for (std::size_t i = 0; i < loop_count; ++i)
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

    for (std::size_t i = 0; i < loop_count; ++i)
    {
        if (pool.count() <= 1)
        {
            break;
        }

        // From experimentation, this test takes ~30ms, so this should be more than enough
        std::this_thread::sleep_for(10ms);
    }

    ASSERT_EQ(static_cast<std::size_t>(1), pool.count());

    pool.join();
}

TEST_F(ThreadPoolTests, MaxThreadsTest)
{
    dhorn::thread_pool pool;
    std::size_t count = 0;

    pool.set_max_threads(1);

    const std::size_t loop_count = 100;
    for (std::size_t i = 0; i < loop_count; ++i)
    {
        pool.submit([&]()
        {
            // No data race since only one thread
            ++count;
        });

        ASSERT_EQ(static_cast<std::size_t>(1), pool.count());
    }

    pool.join();
    ASSERT_EQ(loop_count, count);

    // Now with two threads
    pool = dhorn::thread_pool();
    pool.set_max_threads(2);

    for (std::size_t i = 0; i < loop_count; ++i)
    {
        pool.submit([&]()
        {
        });

        ASSERT_TRUE(pool.count() <= 2);
    }

    ASSERT_EQ(static_cast<std::size_t>(2), pool.count());

    // Setting max back down to one should terminate a thread
    pool.set_max_threads(1);
    for (std::size_t i = 0; i < 10; ++i)
    {
        if (pool.count() == 1)
        {
            break;
        }

        std::this_thread::sleep_for(10ms);
    }

    ASSERT_EQ(static_cast<std::size_t>(1), pool.count());
    pool.join();
}

TEST_F(ThreadPoolTests, MinThreadsTest)
{
    dhorn::thread_pool pool;

    pool.set_min_threads(1);
    ASSERT_EQ(static_cast<std::size_t>(1), pool.count());

    pool.set_min_threads(2);
    ASSERT_EQ(static_cast<std::size_t>(2), pool.count());

    // Setting back down to one shouldn't change the count
    pool.set_min_threads(1);
    ASSERT_EQ(static_cast<std::size_t>(2), pool.count());

    pool.join();
}

TEST_F(ThreadPoolTests, SubmitWithPriorityTest)
{
    dhorn::thread_pool pool;
    pool.set_max_threads(1);

    std::array<std::size_t, 6> array;
    std::size_t index = 0;
    {
        // We need to submit a dummy task to ensure that we don't accidentally start processing a lower priority
        // task before we submit the higher priority ones
        volatile std::size_t stage = 0;

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
        pool.submit(dhorn::thread_pool_priority::low, [&]()
        {
            array[index++] = 4;
        });
        pool.submit(dhorn::thread_pool_priority::normal, [&]()
        {
            array[index++] = 2;
        });
        pool.submit(dhorn::thread_pool_priority::normal, [&]()
        {
            array[index++] = 3;
        });
        pool.submit(dhorn::thread_pool_priority::high, [&]()
        {
            array[index++] = 0;
        });
        pool.submit(dhorn::thread_pool_priority::low, [&]()
        {
            array[index++] = 5;
        });
        pool.submit(dhorn::thread_pool_priority::high, [&]()
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

    ASSERT_EQ(static_cast<std::size_t>(0), array[0]);
    ASSERT_EQ(static_cast<std::size_t>(1), array[1]);
    ASSERT_EQ(static_cast<std::size_t>(2), array[2]);
    ASSERT_EQ(static_cast<std::size_t>(3), array[3]);
    ASSERT_EQ(static_cast<std::size_t>(4), array[4]);
    ASSERT_EQ(static_cast<std::size_t>(5), array[5]);

    pool.join();
}

TEST_F(ThreadPoolTests, SingleThreadThreadPoolConstructionTest)
{
    dhorn::single_thread_thread_pool pool;
    ASSERT_EQ(static_cast<std::size_t>(1), pool.count());
    pool.join();
}

TEST_F(ThreadPoolTests, JoinTest)
{
    dhorn::single_thread_thread_pool pool;
    std::size_t count = 0;
    std::mutex mutex;

    const std::size_t loop_count = 100;
    {
        std::lock_guard<std::mutex> guard(mutex);
        for (std::size_t i = 0; i < loop_count; ++i)
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
    ASSERT_EQ(loop_count, count);
}

TEST_F(ThreadPoolTests, DetachTest)
{
    // Testing framework becomes unhappy if we end with threads running, so make sure we can wait
    std::future<void> future;

    {
        dhorn::thread_pool pool;

        std::mutex mutex;
        std::lock_guard<std::mutex> guard(mutex);

        future = pool.submit_for_result([&]()
        {
            // Can't complete until test is over
            std::lock_guard<std::mutex> guard(mutex);
        });

        pool.detach();
    }

    future.wait();
}

TEST_F(ThreadPoolTests, SubmitForResultTest)
{
    dhorn::thread_pool pool;

    auto future = pool.submit_for_result([]()
    {
        // Simulate a long-ish running task
        std::this_thread::sleep_for(10ms);
        return 42;
    });

    ASSERT_EQ(42, future.get());

    // If we throw an exception, the caller should be the one to handle it
    future = pool.submit_for_result([]() -> int
    {
        throw std::exception();
    });

    ASSERT_THROW(future.get(), std::exception);
    pool.join();
}

TEST_F(ThreadPoolTests, DontCopyTest)
{
    dhorn::thread_pool pool;

    // Any of the submit functions should not cause a copy to occur
    pool.submit([obj = dhorn::tests::object_counter{}](){});
    pool.submit(dhorn::thread_pool_priority::high, [obj = dhorn::tests::object_counter{}](){});
    pool.submit_for_result([obj = dhorn::tests::object_counter{}](){});
    pool.submit_for_result(dhorn::thread_pool_priority::high, [obj = dhorn::tests::object_counter{}](){});

    // Now test the variants that accept args
    pool.submit([obj = dhorn::tests::object_counter{}](const dhorn::tests::object_counter&){}, dhorn::tests::object_counter{});
    pool.submit(dhorn::thread_pool_priority::low, [obj = dhorn::tests::object_counter{}](const dhorn::tests::object_counter&){}, dhorn::tests::object_counter{});
    pool.submit_for_result([obj = dhorn::tests::object_counter{}](const dhorn::tests::object_counter&){}, dhorn::tests::object_counter{});
    pool.submit_for_result(dhorn::thread_pool_priority::low, [obj = dhorn::tests::object_counter{}](const dhorn::tests::object_counter&){}, dhorn::tests::object_counter{});

    pool.join();

    ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::copy_count);

    // All instances should be destroyed, too
    ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::instance_count);
}

TEST_F(ThreadPoolTests, InvokeWithArgsTest)
{
    dhorn::thread_pool pool;
    std::mutex mutex;

    int value = 0;
    std::future<std::size_t> future;
    {
        std::lock_guard<std::mutex> guard(mutex);

        // Create a string and make sure its destructor is run before the task can make progress so that we can
        // validate that we are passing a copy of the string, not a reference
        auto str = "foobar"s;
        pool.submit([&mutex, &value](const std::string& str)
        {
            std::lock_guard<std::mutex> guard(mutex);
            value = static_cast<int>(str.length());
        }, str);

        future = pool.submit_for_result([&mutex](const std::string& str)
        {
            std::lock_guard<std::mutex> guard(mutex);
            return str.length();
        }, str);

        str.clear();
    }

    pool.join();

    ASSERT_EQ(6, value);
    ASSERT_EQ(static_cast<std::size_t>(6), future.get());
}

TEST_F(ThreadPoolTests, ManyThreadsTest)
{
    dhorn::thread_pool pool;
    std::mutex mutex;

    {
        std::lock_guard<std::mutex> guard(mutex);

        // Queue up a bunch of work while we hold the lock to guarantee that they won't complete
        for (std::size_t i = 0; i < 1000; ++i)
        {
            pool.submit([&]()
            {
                std::lock_guard<std::mutex> guard(mutex);
            });
        }

        auto value = pool.submit_for_result([]() {return 42; }).get();
        ASSERT_EQ(42, value);
    }

    pool.join();
}

TEST_F(ThreadPoolTests, CreationBehaviorTest)
{
    static int value = 0;
    struct test_behavior
    {
        inline auto operator()()
        {
            value = 8;
            return dhorn::make_scope_guard([&]()
            {
                value = 42;
            });
        }
    };
    struct test_traits : public dhorn::default_thread_pool_traits
    {
        using creation_behavior = test_behavior;
    };

    dhorn::basic_thread_pool<test_traits> pool;

    // Construction should not have created any threads
    ASSERT_EQ(0, value);

    pool.submit_for_result([](){}).wait();
    ASSERT_EQ(8, value);

    pool.join();
    ASSERT_EQ(42, value);
}
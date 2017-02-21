/*
 * Duncan Horn
 *
 * ThreadPoolTests.cpp
 *
 * Tests for dhorn::experimental::thread_pool
 */
#include "stdafx.h"

#include <dhorn/experimental/thread_pool.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(ThreadPoolTests)
    {
        TEST_METHOD(DefaultInitializeTest)
        {
            dhorn::experimental::thread_pool pool;

            Assert::AreEqual(0u, pool.count());
            pool.join();
        }

        TEST_METHOD(SubmitSingleTaskTest)
        {
            dhorn::experimental::thread_pool pool;
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
    };
}

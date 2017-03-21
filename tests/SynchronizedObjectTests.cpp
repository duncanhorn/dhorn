/*
 * Duncan Horn
 *
 * SynchronizedObjectTests.cpp
 *
 * Tests for synchronized_object.h
 */
#include "stdafx.h"

#include <atomic>
#include <condition_variable>
#include <thread>
#include <vector>

#include <dhorn/experimental/synchronized_object.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        class copy_count
        {
        public:
            copy_count() :
                _copies(0)
            {
            }

            copy_count(copy_count &other) :
                _copies(++other._copies)
            {
            }

            copy_count &operator=(copy_count &other)
            {
                if (&other != this)
                {
                    this->_copies = ++other._copies;
                }

                return *this;
            }

            size_t copies() const
            {
                return this->_copies;
            }

        private:

            size_t _copies;
        };

        template <typename Ty, size_t Size>
        class dynamically_allocated_array
        {
        public:
            dynamically_allocated_array() :
                _copying{}
            {
            }

            dynamically_allocated_array(const dynamically_allocated_array &other) :
                dynamically_allocated_array()
            {
                this->copy(other);
            }

            dynamically_allocated_array &operator=(const dynamically_allocated_array &other)
            {
                if (&other != this)
                {
                    this->copy(other);
                }

                return *this;
            }

            Ty &operator[](size_t index)
            {
                return this->_vals[index];
            }

            const Ty &operator[](size_t index) const
            {
                return this->_vals[index];
            }

            auto begin()
            {
                return std::begin(this->_vals);
            }

            auto end()
            {
                return std::end(this->_vals);
            }

        private:

            void copy(const dynamically_allocated_array &)
            {
                // Keep track of the number of threads modifying the object at any given time. Therefore, if any value
                // is not 1, we know that there is a race
                ++this->_copying;

                for (auto &val : this->_vals)
                {
                    val = this->_copying.load();
                }

                --this->_copying;
            }

            Ty _vals[Size];
            std::atomic_size_t _copying;
        };



        TEST_CLASS(SynchronizedObjectTests)
        {
            TEST_METHOD(BasicLockingTest)
            {
                dhorn::experimental::synchronized_object<size_t> val = 0;
                std::vector<std::thread> threads;

                static const size_t num_threads = 12;
                static const size_t num_iterations = 10000;

                for (size_t i = 0; i < num_threads; ++i)
                {
                    threads.emplace_back([&]()
                    {
                        for (size_t j = 0; j < num_iterations; ++j)
                        {
                            val.execute_with_lock([](size_t &value, const auto &)
                            {
                                ++value;
                            });
                        }
                    });
                }

                for (auto &thread : threads)
                {
                    thread.join();
                }

                Assert::AreEqual(num_threads * num_iterations, val.copy_unlocked());
            }

            /*****
            TEST_METHOD(BasicIncorrectLockingTest)
            {
                dhorn::experimental::synchronized_object<size_t> val = 0;
                std::vector<std::thread> threads;

                static const size_t num_threads = 12;
                static const size_t num_iterations = 10000;

                for (size_t i = 0; i < num_threads; ++i)
                {
                    threads.emplace_back([&]()
                    {
                        for (size_t j = 0; j < num_iterations; ++j)
                        {
                            val.execute_without_lock([](size_t &value)
                            {
                                ++value;
                            });
                        }
                    });
                }

                for (auto &thread : threads)
                {
                    thread.join();
                }

                // The access to val was not thread safe; at least one should have screwed up. This technically can
                // fail unexpectedly, but it's highly unlikely
                Assert::AreNotEqual(num_threads * num_iterations, val.copy_unlocked());
            }
            *****/

            TEST_METHOD(BasicMonitorTest)
            {
                dhorn::experimental::synchronized_object<size_t> val = 0;
                std::condition_variable cond;
                std::vector<std::thread> threads;

                static const size_t num_threads = 12;

                for (size_t i = 0; i < num_threads; ++i)
                {
                    threads.emplace_back([&, index = i]()
                    {
                        val.execute_with_lock([&](size_t &value, std::unique_lock<std::mutex> &lock)
                        {
                            // Threads wait in the "reverse" order. I.e. "early" threads wait on "later" threads
                            cond.wait(lock, [&]() -> bool
                            {
                                return (num_threads - index - 1) == value;
                            });

                            // Don't know which thread we'll wake up, so notify all
                            ++value;
                            cond.notify_all();
                        });
                    });
                }

                for (auto &thread : threads)
                {
                    thread.join();
                }

                Assert::AreEqual(num_threads, val.copy_unlocked());
            }

            TEST_METHOD(RecursiveMutexTest)
            {
                dhorn::experimental::synchronized_object<size_t, std::recursive_mutex> val = 0;
                val.execute_with_lock([&](auto &, auto &)
                {
                    // Shouldn't deadlock
                    val.execute_with_lock([&](auto &value, auto &)
                    {
                        value = 42;
                    });
                });

                Assert::AreEqual(42u, val.copy_unlocked());
            }

            TEST_METHOD(CopyLockedLockedTest)
            {
                dhorn::experimental::synchronized_object<copy_count> val;
                std::vector<std::thread> threads;

                static const size_t num_threads = 12;
                static const size_t num_iterations = 1000;
                std::vector<size_t> check_vector(num_threads * num_iterations);

                for (size_t i = 0; i < num_threads; ++i)
                {
                    threads.emplace_back([&]()
                    {
                        for (size_t j = 0; j < num_iterations; ++j)
                        {
                            // The copy count of each copy should be unique when we lock
                            ++check_vector[val.copy_locked().copies() - 1];
                        }
                    });
                }

                for (auto &thread : threads)
                {
                    thread.join();
                }

                for (auto count : check_vector)
                {
                    Assert::AreEqual(1u, count);
                }
            }

            /*****
            TEST_METHOD(CopyUnlockedLockedTest)
            {
                dhorn::experimental::synchronized_object<copy_count> val;
                std::vector<std::thread> threads;

                static const size_t num_threads = 12;
                static const size_t num_iterations = 10000;
                std::vector<size_t> check_vector(num_threads * num_iterations);

                for (size_t i = 0; i < num_threads; ++i)
                {
                    threads.emplace_back([&]()
                    {
                        for (size_t j = 0; j < num_iterations; ++j)
                        {
                            // The copy count of each copy should be unique when we lock
                            ++check_vector[val.copy_unlocked().copies() - 1];
                        }
                    });
                }

                for (auto &thread : threads)
                {
                    thread.join();
                }

                bool pass = false;
                for (auto count : check_vector)
                {
                    // The access to val was not thread safe; at least one should have screwed up. This technically can
                    // fail unexpectedly, but it's highly unlikely
                    if (count != 1)
                    {
                        pass = true;
                        break;
                    }
                }

                Assert::IsTrue(pass);
            }
            *****/

            TEST_METHOD(SetLockedTest)
            {
                using array_type = dynamically_allocated_array<size_t, 1000>;
                dhorn::experimental::synchronized_object<array_type> val;
                std::vector<std::thread> threads;
                std::vector<array_type> thread_vals;

                static const size_t num_threads = 12;

                // Fill each array with the thread's index
                for (size_t i = 0; i < num_threads; ++i)
                {
                    thread_vals.emplace_back();
                }

                std::mutex mutex;
                std::condition_variable cond;
                size_t running = 0;
                for (size_t i = 0; i < num_threads; ++i)
                {
                    threads.emplace_back([&, index = i]()
                    {
                        {
                            std::unique_lock<std::mutex> lock(mutex);

                            // We need to wait for all threads to be running first to give the best shot at a race
                            // condition
                            ++running;
                            cond.wait(lock, [&]() -> bool
                            {
                                return (running == num_threads);
                            });
                            cond.notify_all();
                        }

                        val.set_locked(thread_vals[index]);
                    });
                }

                for (auto &thread : threads)
                {
                    thread.join();
                }

                val.execute_without_lock([&](array_type &arr)
                {
                    for (auto value : arr)
                    {
                        Assert::AreEqual(1u, value);
                    }
                });
            }

            TEST_METHOD(SetUnlockedTest)
            {
                using array_type = dynamically_allocated_array<size_t, 1000>;
                dhorn::experimental::synchronized_object<array_type> val;
                std::vector<std::thread> threads;
                std::vector<array_type> thread_vals;

                static const size_t num_threads = 12;

                // Fill each array with the thread's index
                for (size_t i = 0; i < num_threads; ++i)
                {
                    thread_vals.emplace_back();
                }

                std::mutex mutex;
                std::condition_variable cond;
                size_t running = 0;
                for (size_t i = 0; i < num_threads; ++i)
                {
                    threads.emplace_back([&, index = i]()
                    {
                        {
                            std::unique_lock<std::mutex> lock(mutex);

                            // We need to wait for all threads to be running first to give the best shot at a race
                            // condition
                            ++running;
                            cond.wait(lock, [&]() -> bool
                            {
                                return (running == num_threads);
                            });
                            cond.notify_all();
                        }

                        val.set_unlocked(thread_vals[index]);
                    });
                }

                for (auto &thread : threads)
                {
                    thread.join();
                }

                bool pass = false;
                val.execute_without_lock([&](array_type &arr)
                {
                    for (auto value : arr)
                    {
                        // The access to val was not thread safe; at least one should have screwed up. This technically
                        // can fail unexpectedly, especially if the last thread is lagging behind significantly, but
                        // it's highly unlikely
                        if (value != 1)
                        {
                            pass = true;
                            break;
                        }
                    }
                });

                Assert::IsTrue(true);
            }

            TEST_METHOD(LockNormalTest)
            {
                dhorn::experimental::synchronized_object<size_t> val = 0;
                std::vector<std::thread> threads;

                static const size_t num_threads = 12;
                static const size_t num_iterations = 10000;

                for (size_t i = 0; i < num_threads; ++i)
                {
                    threads.emplace_back([&]()
                    {
                        for (size_t j = 0; j < num_iterations; ++j)
                        {
                            auto lock = val.lock();
                            val.set_unlocked(val.copy_unlocked() + 1);
                        }
                    });
                }

                for (auto &thread : threads)
                {
                    thread.join();
                }

                Assert::AreEqual(num_threads * num_iterations, val.copy_unlocked());
            }

            TEST_METHOD(TryToLockTest)
            {
                dhorn::experimental::synchronized_object<size_t> val = 0;
                std::vector<std::thread> threads;

                static const size_t num_threads = 12;
                static const size_t num_iterations = 10000;

                for (size_t i = 0; i < num_threads; ++i)
                {
                    threads.emplace_back([&]()
                    {
                        for (size_t j = 0; j < num_iterations; ++j)
                        {
                            auto lock = val.lock(std::try_to_lock);
                            if (lock.owns_lock())
                            {
                                val.set_unlocked(val.copy_unlocked() + 1);
                            }
                        }
                    });
                }

                for (auto &thread : threads)
                {
                    thread.join();
                }

                // At least one attempt to get the lock should have failed, but at least one should have succeeded
                Assert::AreNotEqual(0u, val.copy_unlocked());
                Assert::AreNotEqual(num_threads * num_iterations, val.copy_unlocked());
            }

            TEST_METHOD(DeferLockTest)
            {
                dhorn::experimental::synchronized_object<size_t> val = 0;
                std::vector<std::thread> threads;

                static const size_t num_threads = 12;
                static const size_t num_iterations = 10000;

                for (size_t i = 0; i < num_threads; ++i)
                {
                    threads.emplace_back([&]()
                    {
                        for (size_t j = 0; j < num_iterations; ++j)
                        {
                            auto lock = val.lock(std::defer_lock);
                            lock.lock(); // Shouldn't deadlock
                            val.set_unlocked(val.copy_unlocked() + 1);
                        }
                    });
                }

                for (auto &thread : threads)
                {
                    thread.join();
                }

                Assert::AreEqual(num_threads * num_iterations, val.copy_unlocked());
            }
        };
    }
}

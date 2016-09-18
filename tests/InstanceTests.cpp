/*
 * Duncan Horn
 *
 * InstanceTests.cpp
 *
 * Tests for the dhorn instance class
 */
#include "stdafx.h"

#include <condition_variable>
#include <thread>
#include <vector>

#include <dhorn/experimental/instance.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        class test_class
        {
        public:
            test_class(void) :
                x(0)
            {
                ++instance_count;
                ++created_count;
            }

            ~test_class(void)
            {
                --instance_count;
                ++destroyed_count;
            }

            int x;

            static std::atomic_size_t instance_count;
            static std::atomic_size_t created_count;
            static std::atomic_size_t destroyed_count;
        };
        std::atomic_size_t test_class::instance_count = 0;
        std::atomic_size_t test_class::created_count = 0;
        std::atomic_size_t test_class::destroyed_count = 0;



        TEST_CLASS(InstanceTests)
        {
            TEST_METHOD_INITIALIZE(TestInit)
            {
                // Reset state
                test_class::instance_count = 0;
                test_class::created_count = 0;
                test_class::destroyed_count = 0;
            }

#pragma region GetInstance tests

            TEST_METHOD(LazyInitSharedPtrGetInstanceTest)
            {
                dhorn::lazy_init_instance_t<test_class> obj;

                // Lazy init shouldn't create until we access it
                Assert::AreEqual(0u, test_class::instance_count.load());

                auto ptr = obj.get();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsNotNull(ptr.get());

                // Calling get again should give the same value
                auto other = obj.get();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::AreEqual(1u, test_class::created_count.load());
                Assert::IsTrue(ptr == other);
            }

            TEST_METHOD(LazyInitRawPointerGetInstanceTest)
            {
                dhorn::lazy_init_instance_t<test_class, test_class *> obj;

                // Lazy init shouldn't create until we access it
                Assert::AreEqual(0u, test_class::instance_count.load());

                auto ptr = obj.get();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsNotNull(ptr);

                // Calling get again should give the same value
                auto other = obj.get();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::AreEqual(1u, test_class::created_count.load());
                Assert::IsTrue(ptr == other);
            }

            TEST_METHOD(EagerInitSharedPtrGetInstanceTest)
            {
                dhorn::eager_init_instance_t<test_class> obj;

                // Eager init creates the object right away
                Assert::AreEqual(1u, test_class::instance_count.load());

                auto ptr = obj.get();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsNotNull(ptr.get());

                // Calling get again should give the same value
                auto other = obj.get();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::AreEqual(1u, test_class::created_count.load());
                Assert::IsTrue(ptr == other);
            }

            TEST_METHOD(EagerInitRawPtrGetInstanceTest)
            {
                dhorn::eager_init_instance_t<test_class, test_class *> obj;

                // Eager init creates the object right away
                Assert::AreEqual(1u, test_class::instance_count.load());

                auto ptr = obj.get();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsNotNull(ptr);

                // Calling get again should give the same value
                auto other = obj.get();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::AreEqual(1u, test_class::created_count.load());
                Assert::IsTrue(ptr == other);
            }

            TEST_METHOD(AtomicExchangeSharedPtrGetInstanceTest)
            {
                dhorn::atomic_exchange_instance_t<test_class> obj;

                // Atomic exchange init shouldn't create until we access it
                Assert::AreEqual(0u, test_class::instance_count.load());

                auto ptr = obj.get();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsNotNull(ptr.get());

                // Calling get again should give the same value
                auto other = obj.get();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::AreEqual(1u, test_class::created_count.load());
                Assert::IsTrue(ptr == other);
            }

            TEST_METHOD(AtomicExchangeRawPtrGetInstanceTest)
            {
                dhorn::atomic_exchange_instance_t<test_class, test_class *> obj;

                // Atomic exchange init shouldn't create until we access it
                Assert::AreEqual(0u, test_class::instance_count.load());

                auto ptr = obj.get();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsNotNull(ptr);

                // Calling get again should give the same value
                auto other = obj.get();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::AreEqual(1u, test_class::created_count.load());
                Assert::IsTrue(ptr == other);
            }

#pragma endregion

#pragma region Destroy Tests

            TEST_METHOD(LazyInitSharedPtrDestroyTest)
            {
                {
                    std::shared_ptr<test_class> ptr;
                    {
                        dhorn::lazy_init_instance_t<test_class> obj;
                        ptr = obj.get();
                        Assert::AreEqual(1u, test_class::instance_count.load());
                    }

                    Assert::AreEqual(1u, test_class::instance_count.load());
                    Assert::AreEqual(0u, test_class::destroyed_count.load());
                }

                Assert::AreEqual(0u, test_class::instance_count.load());
                Assert::AreEqual(1u, test_class::destroyed_count.load());
            }

            TEST_METHOD(LazyInitRawPtrDestroyTest)
            {
                {
                    test_class *ptr;
                    {
                        dhorn::lazy_init_instance_t<test_class, test_class *> obj;
                        ptr = obj.get();
                        Assert::AreEqual(1u, test_class::instance_count.load());
                    }

                    // Even though ptr still references the object, no reference count is maintained
                    Assert::AreEqual(0u, test_class::instance_count.load());
                    Assert::AreEqual(1u, test_class::destroyed_count.load());
                }

                Assert::AreEqual(0u, test_class::instance_count.load());
                Assert::AreEqual(1u, test_class::destroyed_count.load());
            }

            TEST_METHOD(EagerInitSharedPtrDestroyTest)
            {
                {
                    std::shared_ptr<test_class> ptr;
                    {
                        dhorn::eager_init_instance_t<test_class> obj;
                        ptr = obj.get();
                        Assert::AreEqual(1u, test_class::instance_count.load());
                    }

                    Assert::AreEqual(1u, test_class::instance_count.load());
                    Assert::AreEqual(0u, test_class::destroyed_count.load());
                }

                Assert::AreEqual(0u, test_class::instance_count.load());
                Assert::AreEqual(1u, test_class::destroyed_count.load());
            }

            TEST_METHOD(EagerInitRawPtrDestroyTest)
            {
                {
                    test_class *ptr;
                    {
                        dhorn::eager_init_instance_t<test_class, test_class *> obj;
                        ptr = obj.get();
                        Assert::AreEqual(1u, test_class::instance_count.load());
                    }

                    // Even though ptr still references the object, no reference count is maintained
                    Assert::AreEqual(0u, test_class::instance_count.load());
                    Assert::AreEqual(1u, test_class::destroyed_count.load());
                }

                Assert::AreEqual(0u, test_class::instance_count.load());
                Assert::AreEqual(1u, test_class::destroyed_count.load());
            }

            TEST_METHOD(AtomicExchangeSharedPtrDestroyTest)
            {
                {
                    std::shared_ptr<test_class> ptr;
                    {
                        dhorn::atomic_exchange_instance_t<test_class> obj;
                        ptr = obj.get();
                        Assert::AreEqual(1u, test_class::instance_count.load());
                    }

                    Assert::AreEqual(1u, test_class::instance_count.load());
                    Assert::AreEqual(0u, test_class::destroyed_count.load());
                }

                Assert::AreEqual(0u, test_class::instance_count.load());
                Assert::AreEqual(1u, test_class::destroyed_count.load());
            }

            TEST_METHOD(AtomicExchangeRawPtrDestroyTest)
            {
                {
                    test_class *ptr;
                    {
                        dhorn::atomic_exchange_instance_t<test_class, test_class *> obj;
                        ptr = obj.get();
                        Assert::AreEqual(1u, test_class::instance_count.load());
                    }

                    // Even though ptr still references the object, no reference count is maintained
                    Assert::AreEqual(0u, test_class::instance_count.load());
                    Assert::AreEqual(1u, test_class::destroyed_count.load());
                }

                Assert::AreEqual(0u, test_class::instance_count.load());
                Assert::AreEqual(1u, test_class::destroyed_count.load());
            }

#pragma endregion

#pragma region Operator Overload Tests

            TEST_METHOD(LazyInitSharedPtrOperatorArrowTest)
            {
                dhorn::lazy_init_instance_t<test_class> obj;

                // operator-> should do an implicit get/initialization
                obj->x = 42;
                Assert::AreEqual(42, obj->x);
                Assert::AreEqual(1u, test_class::instance_count.load());
            }

            TEST_METHOD(LazyInitRawPtrOperatorArrowTest)
            {
                dhorn::lazy_init_instance_t<test_class, test_class *> obj;

                // operator-> should do an implicit get/initialization
                obj->x = 42;
                Assert::AreEqual(42, obj->x);
                Assert::AreEqual(1u, test_class::instance_count.load());
            }

            TEST_METHOD(LazyInitSharedPtrOperatorStarTest)
            {
                dhorn::lazy_init_instance_t<test_class> obj;

                // operator* should do an implicit get/initialization
                (*obj).x = 42;
                Assert::AreEqual(42, (*obj).x);
                Assert::AreEqual(1u, test_class::instance_count.load());
            }

            TEST_METHOD(LazyInitRawPtrOperatorStarTest)
            {
                dhorn::lazy_init_instance_t<test_class, test_class *> obj;

                // operator* should do an implicit get/initialization
                (*obj).x = 42;
                Assert::AreEqual(42, (*obj).x);
                Assert::AreEqual(1u, test_class::instance_count.load());
            }

            TEST_METHOD(EagerInitSharedPtrOperatorArrowTest)
            {
                dhorn::eager_init_instance_t<test_class> obj;

                // operator-> should do an implicit get/initialization
                obj->x = 42;
                Assert::AreEqual(42, obj->x);
                Assert::AreEqual(1u, test_class::instance_count.load());
            }

            TEST_METHOD(EagerInitRawPtrOperatorArrowTest)
            {
                dhorn::eager_init_instance_t<test_class, test_class *> obj;

                // operator-> should do an implicit get/initialization
                obj->x = 42;
                Assert::AreEqual(42, obj->x);
                Assert::AreEqual(1u, test_class::instance_count.load());
            }

            TEST_METHOD(EagerInitSharedPtrOperatorStarTest)
            {
                dhorn::eager_init_instance_t<test_class> obj;

                // operator* should do an implicit get/initialization
                (*obj).x = 42;
                Assert::AreEqual(42, (*obj).x);
                Assert::AreEqual(1u, test_class::instance_count.load());
            }

            TEST_METHOD(EagerInitRawPtrOperatorStarTest)
            {
                dhorn::eager_init_instance_t<test_class, test_class *> obj;

                // operator* should do an implicit get/initialization
                (*obj).x = 42;
                Assert::AreEqual(42, (*obj).x);
                Assert::AreEqual(1u, test_class::instance_count.load());
            }

            TEST_METHOD(AtomicExchangeSharedPtrOperatorArrowTest)
            {
                dhorn::atomic_exchange_instance_t<test_class> obj;

                // operator-> should do an implicit get/initialization
                obj->x = 42;
                Assert::AreEqual(42, obj->x);
                Assert::AreEqual(1u, test_class::instance_count.load());
            }

            TEST_METHOD(AtomicExchangeRawPtrOperatorArrowTest)
            {
                dhorn::atomic_exchange_instance_t<test_class, test_class *> obj;

                // operator-> should do an implicit get/initialization
                obj->x = 42;
                Assert::AreEqual(42, obj->x);
                Assert::AreEqual(1u, test_class::instance_count.load());
            }

            TEST_METHOD(AtomicExchangeSharedPtrOperatorStarTest)
            {
                dhorn::atomic_exchange_instance_t<test_class> obj;

                // operator* should do an implicit get/initialization
                (*obj).x = 42;
                Assert::AreEqual(42, (*obj).x);
                Assert::AreEqual(1u, test_class::instance_count.load());
            }

            TEST_METHOD(AtomicExchangeRawPtrOperatorStarTest)
            {
                dhorn::atomic_exchange_instance_t<test_class, test_class *> obj;

                // operator* should do an implicit get/initialization
                (*obj).x = 42;
                Assert::AreEqual(42, (*obj).x);
                Assert::AreEqual(1u, test_class::instance_count.load());
            }

#pragma endregion

#pragma region Concurrent Access Tests

            static const size_t test_iterations = 100;
            static const size_t test_threads = 12;

            TEST_METHOD(LazyInitConcurrentAccessTest)
            {
                for (size_t i = 0; i < test_iterations; ++i)
                {
                    // Reset state
                    test_class::instance_count = 0;
                    test_class::created_count = 0;
                    test_class::destroyed_count = 0;

                    dhorn::lazy_init_instance_t<test_class> obj;
                    std::vector<std::thread> threads;
                    std::condition_variable sync;
                    std::mutex mutex;

                    size_t running = 0;
                    std::vector<std::shared_ptr<test_class>> pointers(test_threads);
                    for (size_t j = 0; j < test_threads; ++j)
                    {
                        threads.emplace_back([&, j]()
                        {
                            {
                                std::unique_lock<std::mutex> lock(mutex);
                                ++running;
                                if (running == test_threads)
                                {
                                    sync.notify_all();
                                }
                                else
                                {
                                    sync.wait(lock);
                                }
                            }

                            pointers[j] = obj.get();
                        });
                    }

                    for (size_t j = 0; j < test_threads; ++j)
                    {
                        threads[j].join();
                        Assert::IsTrue(pointers[0] == pointers[j]);
                    }

                    // Only one instance should have been created
                    Assert::AreEqual(1u, test_class::instance_count.load());
                    Assert::AreEqual(1u, test_class::created_count.load());
                }
            }

            TEST_METHOD(EagerInitConcurrentAccessTest)
            {
                for (size_t i = 0; i < test_iterations; ++i)
                {
                    // Reset state
                    test_class::instance_count = 0;
                    test_class::created_count = 0;
                    test_class::destroyed_count = 0;

                    dhorn::eager_init_instance_t<test_class> obj;
                    std::vector<std::thread> threads;
                    std::condition_variable sync;
                    std::mutex mutex;

                    size_t running = 0;
                    std::vector<std::shared_ptr<test_class>> pointers(test_threads);
                    for (size_t j = 0; j < test_threads; ++j)
                    {
                        threads.emplace_back([&, j]()
                        {
                            {
                                std::unique_lock<std::mutex> lock(mutex);
                                ++running;
                                if (running == test_threads)
                                {
                                    sync.notify_all();
                                }
                                else
                                {
                                    sync.wait(lock);
                                }
                            }

                            pointers[j] = obj.get();
                        });
                    }

                    for (size_t j = 0; j < test_threads; ++j)
                    {
                        threads[j].join();
                        Assert::IsTrue(pointers[0] == pointers[j]);
                    }

                    // Only one instance should have been created
                    Assert::AreEqual(1u, test_class::instance_count.load());
                    Assert::AreEqual(1u, test_class::created_count.load());
                }
            }

            TEST_METHOD(AtomicExchangeConcurrentAccessTest)
            {
                bool doubleAccess = false;

                for (size_t i = 0; i < test_iterations; ++i)
                {
                    // Reset state
                    test_class::instance_count = 0;
                    test_class::created_count = 0;
                    test_class::destroyed_count = 0;

                    dhorn::atomic_exchange_instance_t<test_class> obj;
                    std::vector<std::thread> threads;
                    std::condition_variable sync;
                    std::mutex mutex;

                    size_t running = 0;
                    std::vector<std::shared_ptr<test_class>> pointers(test_threads);
                    for (size_t j = 0; j < test_threads; ++j)
                    {
                        threads.emplace_back([&, j]()
                        {
                            {
                                std::unique_lock<std::mutex> lock(mutex);
                                ++running;
                                if (running == test_threads)
                                {
                                    sync.notify_all();
                                }
                                else
                                {
                                    sync.wait(lock);
                                }
                            }

                            pointers[j] = obj.get();
                        });
                    }

                    for (size_t j = 0; j < test_threads; ++j)
                    {
                        threads[j].join();
                        Assert::IsTrue(pointers[0] == pointers[j]);
                    }

                    // May have created more than one instance, but only should still exist
                    Assert::AreEqual(1u, test_class::instance_count.load());
                    if (test_class::created_count.load() > 1)
                    {
                        doubleAccess = true;
                    }
                }

                // We should have had at least one race condition
                Assert::IsTrue(doubleAccess);
            }

#pragma endregion
        };
    }
}

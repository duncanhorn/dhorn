/*
 * InstanceTests.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn instance class
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/instance.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        class test_class
        {
        public:
            test_class(void)
            {
                ++instance_count;
            }

            ~test_class(void)
            {
                --instance_count;
            }

            static std::atomic_size_t instance_count;
        };
        std::atomic_size_t test_class::instance_count = 0;



        TEST_CLASS(InstanceTests)
        {
            TEST_METHOD_INITIALIZE(TestInit)
            {
                // Reset state
                test_class::instance_count = 0;
            }

#pragma region GetInstance tests

            TEST_METHOD(LazyInitSharedPtrGetInstanceTest)
            {
                dhorn::lazy_init_instance_t<test_class> obj;

                // Lazy init shouldn't create until we access it
                Assert::AreEqual(0u, test_class::instance_count.load());

                auto ptr = obj.get_instance();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsNotNull(ptr.get());

                // Calling get_instance again should give the same value
                auto other = obj.get_instance();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsTrue(ptr == other);
            }

            TEST_METHOD(LazyInitRawPointerGetInstanceTest)
            {
                dhorn::lazy_init_instance_t<test_class, test_class *> obj;

                // Lazy init shouldn't create until we access it
                Assert::AreEqual(0u, test_class::instance_count.load());

                auto ptr = obj.get_instance();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsNotNull(ptr);

                // Calling get_instance again should give the same value
                auto other = obj.get_instance();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsTrue(ptr == other);
            }

            TEST_METHOD(EagerInitSharedPtrGetInstanceTest)
            {
                dhorn::eager_init_instance_t<test_class> obj;

                // Eager init creates the object right away
                Assert::AreEqual(1u, test_class::instance_count.load());

                auto ptr = obj.get_instance();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsNotNull(ptr.get());

                // Calling get_instance again should give the same value
                auto other = obj.get_instance();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsTrue(ptr == other);
            }

            TEST_METHOD(EagerInitRawPtrGetInstanceTest)
            {
                dhorn::eager_init_instance_t<test_class, test_class *> obj;

                // Eager init creates the object right away
                Assert::AreEqual(1u, test_class::instance_count.load());

                auto ptr = obj.get_instance();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsNotNull(ptr);

                // Calling get_instance again should give the same value
                auto other = obj.get_instance();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsTrue(ptr == other);
            }

            TEST_METHOD(AtomicExchangeInitSharedPtrGetInstanceTest)
            {
                dhorn::atomic_exchange_instance_t<test_class> obj;

                // Atomic exchange init shouldn't create until we access it
                Assert::AreEqual(0u, test_class::instance_count.load());

                auto ptr = obj.get_instance();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsNotNull(ptr.get());

                // Calling get_instance again should give the same value
                auto other = obj.get_instance();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsTrue(ptr == other);
            }

            TEST_METHOD(AtomicExchangeInitRawPtrGetInstanceTest)
            {
                dhorn::atomic_exchange_instance_t<test_class, test_class *> obj;

                // Atomic exchange init shouldn't create until we access it
                Assert::AreEqual(0u, test_class::instance_count.load());

                auto ptr = obj.get_instance();
                Assert::AreEqual(1u, test_class::instance_count.load());
                Assert::IsNotNull(ptr);

                // Calling get_instance again should give the same value
                auto other = obj.get_instance();
                Assert::AreEqual(1u, test_class::instance_count.load());
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
                        ptr = obj.get_instance();
                        Assert::AreEqual(1u, test_class::instance_count.load());
                    }

                    Assert::AreEqual(1u, test_class::instance_count.load());
                }

                Assert::AreEqual(0u, test_class::instance_count.load());
            }

            TEST_METHOD(LazyInitRawPtrDestroyTest)
            {
                {
                    test_class *ptr;
                    {
                        dhorn::lazy_init_instance_t<test_class, test_class *> obj;
                        ptr = obj.get_instance();
                        Assert::AreEqual(1u, test_class::instance_count.load());
                    }

                    // Even though ptr still references the object, no reference count is maintained
                    Assert::AreEqual(0u, test_class::instance_count.load());
                }

                Assert::AreEqual(0u, test_class::instance_count.load());
            }

            TEST_METHOD(EagerInitSharedPtrDestroyTest)
            {
                {
                    std::shared_ptr<test_class> ptr;
                    {
                        dhorn::eager_init_instance_t<test_class> obj;
                        ptr = obj.get_instance();
                        Assert::AreEqual(1u, test_class::instance_count.load());
                    }

                    Assert::AreEqual(1u, test_class::instance_count.load());
                }

                Assert::AreEqual(0u, test_class::instance_count.load());
            }

            TEST_METHOD(EagerInitRawPtrDestroyTest)
            {
                {
                    test_class *ptr;
                    {
                        dhorn::eager_init_instance_t<test_class, test_class *> obj;
                        ptr = obj.get_instance();
                        Assert::AreEqual(1u, test_class::instance_count.load());
                    }

                    // Even though ptr still references the object, no reference count is maintained
                    Assert::AreEqual(0u, test_class::instance_count.load());
                }

                Assert::AreEqual(0u, test_class::instance_count.load());
            }

            TEST_METHOD(AtomicExchangeInitSharedPtrDestroyTest)
            {
                {
                    std::shared_ptr<test_class> ptr;
                    {
                        dhorn::atomic_exchange_instance_t<test_class> obj;
                        ptr = obj.get_instance();
                        Assert::AreEqual(1u, test_class::instance_count.load());
                    }

                    Assert::AreEqual(1u, test_class::instance_count.load());
                }

                Assert::AreEqual(0u, test_class::instance_count.load());
            }

            TEST_METHOD(AtomicExchangeInitRawPtrDestroyTest)
            {
                {
                    test_class *ptr;
                    {
                        dhorn::atomic_exchange_instance_t<test_class, test_class *> obj;
                        ptr = obj.get_instance();
                        Assert::AreEqual(1u, test_class::instance_count.load());
                    }

                    // Even though ptr still references the object, no reference count is maintained
                    Assert::AreEqual(0u, test_class::instance_count.load());
                }

                Assert::AreEqual(0u, test_class::instance_count.load());
            }

#pragma endregion
        };
    }
}

/*
 * Duncan Horn
 *
 * AsyncTests.cpp
 *
 * Tests for the async.h header
 */
#include "stdafx.h"

#include <dhorn/experimental/async.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace dhorn::experimental;

namespace dhorn::tests
{
    TEST_CLASS(AsyncTests)
    {
        TEST_METHOD_INITIALIZE(TestInitialize)
        {
            object_counter::reset();
        }

        TEST_METHOD_CLEANUP(TestCleanup)
        {
            Assert::AreEqual(static_cast<std::size_t>(0), object_counter::instance_count);
            object_counter::reset();
        }

        TEST_METHOD(DefaultConstructorTest)
        {
            async<int> a;
            Assert::ExpectException<std::future_error>([&]() { a.get(); });
        }

        TEST_METHOD(SimpleValueTest)
        {
            auto value = []() -> async<int>
            {
                co_return 42;
            }();

            Assert::AreEqual(42, value.get());
        }

        TEST_METHOD(SimpleReferenceCountTest)
        {
            {
                auto value = []() -> async<object_counter>
                {
                    co_return object_counter{};
                }();

                value.wait();
                Assert::AreEqual(static_cast<std::size_t>(1), object_counter::instance_count);
                Assert::AreEqual(static_cast<std::size_t>(0), object_counter::copy_count);

                decltype(auto) movedValue = value.get();
                Assert::AreEqual(static_cast<std::size_t>(1), object_counter::instance_count);
                Assert::AreEqual(static_cast<std::size_t>(0), object_counter::copy_count);
            }

            Assert::AreEqual(static_cast<std::size_t>(0), object_counter::instance_count);
            Assert::AreEqual(static_cast<std::size_t>(0), object_counter::copy_count);
            Assert::AreEqual(object_counter::constructed_count, object_counter::destructed_count);
        }

        TEST_METHOD(SimpleExceptionTest)
        {
            auto value = []() -> async<int>
            {
                co_await std::experimental::suspend_never{};
                throw std::invalid_argument("test");
                co_return 42; // ???
            }();

            Assert::ExpectException<std::invalid_argument>([&]() {value.get(); });
        }

        TEST_METHOD(ReferenceTest)
        {
            int x = 8;
            auto value = [&]() -> async<int&>
            {
                co_return x;
            }();

            decltype(auto) y = value.get();
            Assert::AreEqual(&x, &y);

            y = 42;
            Assert::AreEqual(42, x);
        }

        //TEST_METHOD(CopyConstructorTest)
        //{
        //    async<int> outer;

        //}
    };
}

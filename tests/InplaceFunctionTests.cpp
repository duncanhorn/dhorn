/*
 * Duncan Horn
 *
 * InplaceFunctionTests.cpp
 *
 * Tests for the dhorn::experimental::inplace_function type
 */
#include "stdafx.h"

#include <dhorn/experimental/inplace_function.h>

#include "object_counter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace dhorn::experimental;

namespace dhorn::tests
{
    struct test_object
    {
        int value = 42;

        const int& doit() { return value; }
    };

    TEST_CLASS(InplaceFunctionTests)
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

        TEST_METHOD(DefaultConstructionTest)
        {
            inplace_function<void()> fn;
            Assert::IsFalse(static_cast<bool>(fn));

            Assert::ExpectException<std::bad_function_call>([&]() { fn(); });
        }

        TEST_METHOD(NullptrConstructionTest)
        {
            inplace_function<void()> fn(nullptr);
            Assert::IsFalse(static_cast<bool>(fn));

            Assert::ExpectException<std::bad_function_call>([&]() { fn(); });
        }

        TEST_METHOD(NullFunctionPointerConstructionTest)
        {
            int (*pfn)() = nullptr;
            inplace_function<int()> fn(pfn);
            Assert::IsFalse(static_cast<bool>(fn));

            Assert::ExpectException<std::bad_function_call>([&]() { fn(); });

            std::size_t (std::string::*nullMemberFn)() const = nullptr;
            inplace_function<std::size_t(const std::string&)> fn2(nullMemberFn);
            Assert::IsFalse(static_cast<bool>(fn2));

            Assert::ExpectException<std::bad_function_call>([&]() { fn2(std::string()); });
        }

        TEST_METHOD(FunctionPointerConstructionTest)
        {
            inplace_function<std::size_t(const char*)> fn(std::strlen);
            Assert::IsTrue(static_cast<bool>(fn));

            Assert::AreEqual(static_cast<std::size_t>(3), fn("foo"));
        }

        TEST_METHOD(LambdaConstructionTest)
        {
            inplace_function<int()> fn([]() { return 42; });
            Assert::IsTrue(static_cast<bool>(fn));

            Assert::AreEqual(42, fn());
        }

        TEST_METHOD(MutableLambdaConstructionTest)
        {
            inplace_function<int()> fn([value = 0]() mutable { return ++value; });
            Assert::IsTrue(static_cast<bool>(fn));

            Assert::AreEqual(1, fn());
            Assert::AreEqual(2, fn());
        }

        TEST_METHOD(CopyConstructorTest)
        {
            int value = 42;
            inplace_function<int()> fn([&]() { return value; });
            inplace_function<int()> fn_copy(fn);
            Assert::IsTrue(static_cast<bool>(fn));
            Assert::IsTrue(static_cast<bool>(fn_copy));

            Assert::AreEqual(42, fn_copy());
            value = 8;
            Assert::AreEqual(8, fn_copy());

            inplace_function<int()> null_fn;
            inplace_function<int()> null_fn_copy(null_fn);
            Assert::IsFalse(static_cast<bool>(null_fn_copy));
            Assert::ExpectException<std::bad_function_call>([&]() { null_fn_copy(); });
        }

        TEST_METHOD(MoveConstructorTest)
        {
            int value = 42;
            inplace_function<int()> fn([&]() { return value; });
            inplace_function<int()> fn_copy(std::move(fn));
            Assert::IsFalse(static_cast<bool>(fn));
            Assert::IsTrue(static_cast<bool>(fn_copy));

            Assert::AreEqual(42, fn_copy());
            value = 8;
            Assert::AreEqual(8, fn_copy());

            inplace_function<int()> null_fn;
            inplace_function<int()> null_fn_copy(std::move(null_fn));
            Assert::IsFalse(static_cast<bool>(null_fn_copy));
            Assert::ExpectException<std::bad_function_call>([&]() { null_fn_copy(); });
        }

        TEST_METHOD(CopyConstructWithDifferentSizeTest)
        {
            int value = 42;
            inplace_function<int(), 32> fn_small([&]() { return value; });
            inplace_function<int(), 40> fn_large(fn_small);
            Assert::IsTrue(static_cast<bool>(fn_small));
            Assert::IsTrue(static_cast<bool>(fn_large));

            Assert::AreEqual(42, fn_large());
            value = 8;
            Assert::AreEqual(8, fn_large());

            inplace_function<int(), 32> null_fn;
            inplace_function<int(), 40> null_fn_copy(null_fn);
            Assert::IsFalse(static_cast<bool>(null_fn_copy));
            Assert::ExpectException<std::bad_function_call>([&]() { null_fn_copy(); });
        }

        TEST_METHOD(MoveConstructWithDifferentSizeTest)
        {
            int value = 42;
            inplace_function<int(), 32> fn_small([&]() { return value; });
            inplace_function<int(), 40> fn_large(std::move(fn_small));
            Assert::IsFalse(static_cast<bool>(fn_small));
            Assert::IsTrue(static_cast<bool>(fn_large));

            Assert::AreEqual(42, fn_large());
            value = 8;
            Assert::AreEqual(8, fn_large());

            inplace_function<int(), 32> null_fn;
            inplace_function<int(), 40> null_fn_copy(std::move(null_fn));
            Assert::IsFalse(static_cast<bool>(null_fn_copy));
            Assert::ExpectException<std::bad_function_call>([&]() { null_fn_copy(); });
        }

        TEST_METHOD(LargeObjectConstructionTest)
        {
            std::uint8_t large_array[inplace_function<void()>::max_size - sizeof(void*)];
            auto large_lambda = [large_array]()
            {
            };

            // Just a compilation test...
            inplace_function<void()> fn(large_lambda);

            std::uint8_t larger_array[inplace_function<void()>::max_size + 1];
            auto larger_lambda = [larger_array]()
            {
            };

            // TODO: Should we SFINAE here? static_assert is actually probably better...
            // Assert::IsFalse(std::is_constructible_v<inplace_function<void()>, decltype(larger_array)>);
            //inplace_function<void()> fn_fail(larger_lambda);
        }

        TEST_METHOD(MemberFunctionConstructionTest)
        {
            inplace_function<int(test_object&)> fn(&test_object::doit);
            Assert::IsTrue(static_cast<bool>(fn));

            test_object f;
            Assert::AreEqual(42, fn(f));
            f.value = 8;
            Assert::AreEqual(8, fn(f));
        }

        TEST_METHOD(MemberPointerConstructionTest)
        {
            inplace_function<int(test_object*)> fn(&test_object::value);
            Assert::IsTrue(static_cast<bool>(fn));

            test_object f;
            Assert::AreEqual(42, fn(&f));
            f.value = 8;
            Assert::AreEqual(8, fn(&f));
        }

        TEST_METHOD(DestructorTest)
        {
            {
                inplace_function<void()> fn([o = object_counter{}]() {});
                Assert::AreEqual(static_cast<std::size_t>(1), object_counter::instance_count);
                Assert::AreEqual(static_cast<std::size_t>(0), object_counter::copy_count);

                inplace_function<void()> fn_copy(fn);
                Assert::AreEqual(static_cast<std::size_t>(2), object_counter::instance_count);
                Assert::AreEqual(static_cast<std::size_t>(1), object_counter::copy_count);

                inplace_function<void()> fn_move(std::move(fn));
                Assert::AreEqual(static_cast<std::size_t>(2), object_counter::instance_count);
                Assert::AreEqual(static_cast<std::size_t>(1), object_counter::copy_count);
            }

            Assert::AreEqual(static_cast<std::size_t>(0), object_counter::instance_count);
            Assert::AreEqual(object_counter::constructed_count, object_counter::destructed_count);
        }

        TEST_METHOD(NullptrAssignmentTest)
        {
            inplace_function<void()> fn([]() {});
            fn = nullptr;
            Assert::IsFalse(static_cast<bool>(fn));

            Assert::ExpectException<std::bad_function_call>([&]() { fn(); });
        }

        TEST_METHOD(NullFunctionPointerAssignmentTest)
        {
            inplace_function<void(const std::string&)> fn([](const std::string&) {});
            void (*nullFn)(const std::string&) = nullptr;
            fn = nullFn;
            Assert::IsFalse(static_cast<bool>(fn));

            Assert::ExpectException<std::bad_function_call>([&]() { fn("foo"); });

            void (std::string::*nullMemberFnPtr)() const = nullptr;
            fn = nullMemberFnPtr;
            Assert::IsFalse(static_cast<bool>(fn));

            Assert::ExpectException<std::bad_function_call>([&]() { fn("foo"); });
        }

        TEST_METHOD(FunctionPointerAssignmentTest)
        {
            inplace_function<std::size_t(const char*)> fn;
            Assert::IsFalse(static_cast<bool>(fn));

            fn = strlen;
            Assert::IsTrue(static_cast<bool>(fn));
            Assert::AreEqual(static_cast<std::size_t>(3), fn("foo"));
        }

        TEST_METHOD(LambdaAssignmentTest)
        {
            inplace_function<int()> fn;
            Assert::IsFalse(static_cast<bool>(fn));

            fn = []() { return 42; };
            Assert::AreEqual(42, fn());
        }

        TEST_METHOD(MutableLambdaAssignmentTest)
        {
            inplace_function<int()> fn;
            Assert::IsFalse(static_cast<bool>(fn));

            fn = [value = 42]() mutable { return value++; };
            Assert::AreEqual(42, fn());
            Assert::AreEqual(43, fn());
        }

        TEST_METHOD(CopyAssignmentTest)
        {
            int value = 42;
            inplace_function<int()> fn([&]() { return value; });
            inplace_function<int()> fn_copy;
            Assert::IsFalse(static_cast<bool>(fn_copy));

            fn_copy = fn;
            Assert::IsTrue(static_cast<bool>(fn));
            Assert::IsTrue(static_cast<bool>(fn_copy));
            Assert::AreEqual(42, fn_copy());

            inplace_function<int()> null_fn;
            fn_copy = null_fn;
            Assert::IsFalse(static_cast<bool>(fn_copy));
            Assert::ExpectException<std::bad_function_call>([&]() { fn_copy(); });
        }

        TEST_METHOD(MoveAssignmentTest)
        {
            int value = 42;
            inplace_function<int()> fn([&]() { return value; });
            inplace_function<int()> fn_copy;
            Assert::IsFalse(static_cast<bool>(fn_copy));

            fn_copy = std::move(fn);
            Assert::IsFalse(static_cast<bool>(fn));
            Assert::IsTrue(static_cast<bool>(fn_copy));
            Assert::AreEqual(42, fn_copy());

            inplace_function<int()> null_fn;
            fn_copy = std::move(null_fn);
            Assert::IsFalse(static_cast<bool>(fn_copy));
            Assert::ExpectException<std::bad_function_call>([&]() { fn_copy(); });
        }

        TEST_METHOD(CopyAssignWithDifferentSizeTest)
        {
            int value = 42;
            inplace_function<int(), 32> fn_small([&]() { return value; });
            inplace_function<int(), 40> fn_large;
            Assert::IsFalse(static_cast<bool>(fn_large));

            fn_large = fn_small;
            Assert::IsTrue(static_cast<bool>(fn_small));
            Assert::IsTrue(static_cast<bool>(fn_large));

            Assert::AreEqual(42, fn_large());
            value = 8;
            Assert::AreEqual(8, fn_large());

            inplace_function<int(), 32> null_fn;
            fn_large = null_fn;
            Assert::IsFalse(static_cast<bool>(fn_large));
            Assert::ExpectException<std::bad_function_call>([&]() { fn_large(); });
        }

        TEST_METHOD(MoveAssignWithDifferentSizeTest)
        {
            int value = 42;
            inplace_function<int(), 32> fn_small([&]() { return value; });
            inplace_function<int(), 40> fn_large;
            Assert::IsFalse(static_cast<bool>(fn_large));

            fn_large = std::move(fn_small);
            Assert::IsFalse(static_cast<bool>(fn_small));
            Assert::IsTrue(static_cast<bool>(fn_large));

            Assert::AreEqual(42, fn_large());
            value = 8;
            Assert::AreEqual(8, fn_large());

            inplace_function<int(), 32> null_fn;
            fn_large = std::move(null_fn);
            Assert::IsFalse(static_cast<bool>(fn_large));
            Assert::ExpectException<std::bad_function_call>([&]() { fn_large(); });
        }

        TEST_METHOD(LargeObjectAssignmentTest)
        {
            std::uint8_t large_array[inplace_function<void()>::max_size - sizeof(void*)];
            auto large_lambda = [large_array]()
            {
            };

            inplace_function<void()> fn;
            Assert::IsFalse(static_cast<bool>(fn));

            // Just a compilation test...
            fn = large_lambda;

            std::uint8_t larger_array[inplace_function<void()>::max_size + 1];
            auto larger_lambda = [larger_array]()
            {
            };

            // TODO: Should we SFINAE here? static_assert is actually probably better...
            //fn = larger_lambda;
        }

        TEST_METHOD(MemberFunctionAssignmentTest)
        {
            inplace_function<int(test_object&)> fn;
            Assert::IsFalse(static_cast<bool>(fn));

            fn = &test_object::doit;
            Assert::IsTrue(static_cast<bool>(fn));

            test_object f;
            Assert::AreEqual(42, fn(f));
            f.value = 8;
            Assert::AreEqual(8, fn(f));
        }

        TEST_METHOD(MemberPointerAssignmentTest)
        {
            inplace_function<int(test_object*)> fn;
            Assert::IsFalse(static_cast<bool>(fn));

            fn = &test_object::value;
            Assert::IsTrue(static_cast<bool>(fn));

            test_object f;
            Assert::AreEqual(42, fn(&f));
            f.value = 8;
            Assert::AreEqual(8, fn(&f));
        }

        TEST_METHOD(AssignmentCopyMoveContentsTest)
        {
            {
                inplace_function<void()> fn;
                Assert::IsFalse(static_cast<bool>(fn));

                fn = [o = object_counter{}]() {};
                Assert::AreEqual(static_cast<std::size_t>(1), object_counter::instance_count);
                Assert::AreEqual(static_cast<std::size_t>(0), object_counter::copy_count);

                inplace_function<void()> other;
                Assert::IsFalse(static_cast<bool>(other));

                other = fn;
                Assert::AreEqual(static_cast<std::size_t>(2), object_counter::instance_count);
                Assert::AreEqual(static_cast<std::size_t>(1), object_counter::copy_count);

                other = std::move(fn);
                Assert::AreEqual(static_cast<std::size_t>(1), object_counter::instance_count);
                Assert::AreEqual(static_cast<std::size_t>(1), object_counter::copy_count);
            }

            Assert::AreEqual(static_cast<std::size_t>(0), object_counter::instance_count);
            Assert::AreEqual(object_counter::constructed_count, object_counter::destructed_count);
        }

        TEST_METHOD(SwapTest)
        {
            {
                inplace_function<int()> fn1;
                inplace_function<int()> fn2;
                Assert::IsFalse(static_cast<bool>(fn1));
                Assert::IsFalse(static_cast<bool>(fn2));
                Assert::ExpectException<std::bad_function_call>([&]() { fn1(); });
                Assert::ExpectException<std::bad_function_call>([&]() { fn2(); });

                fn1.swap(fn2);
                Assert::IsFalse(static_cast<bool>(fn1));
                Assert::IsFalse(static_cast<bool>(fn2));
                Assert::ExpectException<std::bad_function_call>([&]() { fn1(); });
                Assert::ExpectException<std::bad_function_call>([&]() { fn2(); });

                fn1 = [o = object_counter{}]() { return 42; };
                fn1.swap(fn2);
                Assert::AreEqual(static_cast<std::size_t>(1), object_counter::instance_count);
                Assert::AreEqual(static_cast<std::size_t>(0), object_counter::copy_count);
                Assert::IsFalse(static_cast<bool>(fn1));
                Assert::IsTrue(static_cast<bool>(fn2));
                Assert::ExpectException<std::bad_function_call>([&]() { fn1(); });
                Assert::AreEqual(42, fn2());

                fn1.swap(fn2);
                Assert::AreEqual(static_cast<std::size_t>(1), object_counter::instance_count);
                Assert::AreEqual(static_cast<std::size_t>(0), object_counter::copy_count);
                Assert::IsTrue(static_cast<bool>(fn1));
                Assert::IsFalse(static_cast<bool>(fn2));
                Assert::AreEqual(42, fn1());
                Assert::ExpectException<std::bad_function_call>([&]() { fn2(); });

                fn2 = [o = object_counter{}]() { return 8; };
                fn1.swap(fn2);
                Assert::AreEqual(static_cast<std::size_t>(2), object_counter::instance_count);
                Assert::AreEqual(static_cast<std::size_t>(0), object_counter::copy_count);
                Assert::IsTrue(static_cast<bool>(fn1));
                Assert::IsTrue(static_cast<bool>(fn2));
                Assert::AreEqual(8, fn1());
                Assert::AreEqual(42, fn2());
            }

            Assert::AreEqual(static_cast<std::size_t>(0), object_counter::instance_count);
        }

        TEST_METHOD(TargetTypeTest)
        {
            auto lambda = []() {};
            auto other_lambda = []() {};

            inplace_function<void()> fn(lambda);
            Assert::IsTrue(typeid(lambda) == fn.target_type());
            Assert::IsTrue(typeid(other_lambda) != fn.target_type());

            fn = other_lambda;
            Assert::IsTrue(typeid(lambda) != fn.target_type());
            Assert::IsTrue(typeid(other_lambda) == fn.target_type());
        }

        TEST_METHOD(TargetTest)
        {
            int value = 42;
            auto lambda = [&]() { return value; };
            using lambda_type = decltype(lambda);
            auto other_lambda = [&]() { return -value; };
            using other_lambda_type = decltype(other_lambda);

            inplace_function<int()> fn(lambda);
            Assert::IsNotNull(fn.target<lambda_type>());
            Assert::IsNull(fn.target<other_lambda_type>());
            Assert::AreEqual(42, (*fn.target<lambda_type>())());
            value = 8;
            Assert::AreEqual(8, (*fn.target<lambda_type>())());

            fn = other_lambda;
            Assert::IsNull(fn.target<lambda_type>());
            Assert::IsNotNull(fn.target<other_lambda_type>());
            Assert::AreEqual(-8, (*fn.target<other_lambda_type>())());
            value = 42;
            Assert::AreEqual(-42, (*fn.target<other_lambda_type>())());
        }

        TEST_METHOD(NullEqualityTest)
        {
            inplace_function<void()> fn;
            Assert::IsTrue(fn == nullptr);
            Assert::IsTrue(nullptr == fn);
            Assert::IsFalse(fn != nullptr);
            Assert::IsFalse(nullptr != fn);

            fn = []() {};
            Assert::IsFalse(fn == nullptr);
            Assert::IsFalse(nullptr == fn);
            Assert::IsTrue(fn != nullptr);
            Assert::IsTrue(nullptr != fn);
        }
    };
}

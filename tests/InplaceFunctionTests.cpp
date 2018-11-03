/*
 * Duncan Horn
 *
 * InplaceFunctionTests.cpp
 *
 * Tests for the dhorn::experimental::inplace_function type
 */

#include <dhorn/inplace_function.h>
#include <gtest/gtest.h>

#include "object_counter.h"

struct test_object
{
    int value = 42;

    const int& doit() { return value; }
};

struct InplaceFunctionTests : testing::Test
{
    virtual void SetUp() override
    {
        dhorn::tests::object_counter::reset();
    }

    virtual void TearDown() override
    {
        ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::instance_count);
        dhorn::tests::object_counter::reset();
    }
};

TEST_F(InplaceFunctionTests, DefaultConstructionTest)
{
    dhorn::inplace_function<void()> fn;
    ASSERT_FALSE(static_cast<bool>(fn));

    ASSERT_THROW(fn(), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, NullptrConstructionTest)
{
    dhorn::inplace_function<void()> fn(nullptr);
    ASSERT_FALSE(static_cast<bool>(fn));

    ASSERT_THROW(fn(), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, NullFunctionPointerConstructionTest)
{
    int (*pfn)() = nullptr;
    dhorn::inplace_function<int()> fn(pfn);
    ASSERT_FALSE(static_cast<bool>(fn));

    ASSERT_THROW(fn(), std::bad_function_call);

    std::size_t (std::string::*nullMemberFn)() const = nullptr;
    dhorn::inplace_function<std::size_t(const std::string&)> fn2(nullMemberFn);
    ASSERT_FALSE(static_cast<bool>(fn2));

    ASSERT_THROW(fn2(std::string()), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, FunctionPointerConstructionTest)
{
    dhorn::inplace_function<std::size_t(const char*)> fn(std::strlen);
    ASSERT_TRUE(static_cast<bool>(fn));

    ASSERT_EQ(static_cast<std::size_t>(3), fn("foo"));
}

TEST_F(InplaceFunctionTests, LambdaConstructionTest)
{
    dhorn::inplace_function<int()> fn([]() { return 42; });
    ASSERT_TRUE(static_cast<bool>(fn));

    ASSERT_EQ(42, fn());
}

TEST_F(InplaceFunctionTests, MutableLambdaConstructionTest)
{
    dhorn::inplace_function<int()> fn([value = 0]() mutable { return ++value; });
    ASSERT_TRUE(static_cast<bool>(fn));

    ASSERT_EQ(1, fn());
    ASSERT_EQ(2, fn());
}

TEST_F(InplaceFunctionTests, CopyConstructorTest)
{
    int value = 42;
    dhorn::inplace_function<int()> fn([&]() { return value; });
    dhorn::inplace_function<int()> fn_copy(fn);
    ASSERT_TRUE(static_cast<bool>(fn));
    ASSERT_TRUE(static_cast<bool>(fn_copy));

    ASSERT_EQ(42, fn_copy());
    value = 8;
    ASSERT_EQ(8, fn_copy());

    dhorn::inplace_function<int()> null_fn;
    dhorn::inplace_function<int()> null_fn_copy(null_fn);
    ASSERT_FALSE(static_cast<bool>(null_fn_copy));
    ASSERT_THROW(null_fn_copy(), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, MoveConstructorTest)
{
    int value = 42;
    dhorn::inplace_function<int()> fn([&]() { return value; });
    dhorn::inplace_function<int()> fn_copy(std::move(fn));
    ASSERT_FALSE(static_cast<bool>(fn));
    ASSERT_TRUE(static_cast<bool>(fn_copy));

    ASSERT_EQ(42, fn_copy());
    value = 8;
    ASSERT_EQ(8, fn_copy());

    dhorn::inplace_function<int()> null_fn;
    dhorn::inplace_function<int()> null_fn_copy(std::move(null_fn));
    ASSERT_FALSE(static_cast<bool>(null_fn_copy));
    ASSERT_THROW(null_fn_copy(), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, CopyConstructWithDifferentSizeTest)
{
    int value = 42;
    dhorn::inplace_function<int(), 32> fn_small([&]() { return value; });
    dhorn::inplace_function<int(), 40> fn_large(fn_small);
    ASSERT_TRUE(static_cast<bool>(fn_small));
    ASSERT_TRUE(static_cast<bool>(fn_large));

    ASSERT_EQ(42, fn_large());
    value = 8;
    ASSERT_EQ(8, fn_large());

    dhorn::inplace_function<int(), 32> null_fn;
    dhorn::inplace_function<int(), 40> null_fn_copy(null_fn);
    ASSERT_FALSE(static_cast<bool>(null_fn_copy));
    ASSERT_THROW(null_fn_copy(), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, MoveConstructWithDifferentSizeTest)
{
    int value = 42;
    dhorn::inplace_function<int(), 32> fn_small([&]() { return value; });
    dhorn::inplace_function<int(), 40> fn_large(std::move(fn_small));
    ASSERT_FALSE(static_cast<bool>(fn_small));
    ASSERT_TRUE(static_cast<bool>(fn_large));

    ASSERT_EQ(42, fn_large());
    value = 8;
    ASSERT_EQ(8, fn_large());

    dhorn::inplace_function<int(), 32> null_fn;
    dhorn::inplace_function<int(), 40> null_fn_copy(std::move(null_fn));
    ASSERT_FALSE(static_cast<bool>(null_fn_copy));
    ASSERT_THROW(null_fn_copy(), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, LargeObjectConstructionTest)
{
    std::uint8_t large_array[dhorn::inplace_function<void()>::max_size];
    auto large_lambda = [large_array]()
    {
        (void)large_array;
    };

    // Just a compilation test...
    dhorn::inplace_function<void()> fn(large_lambda);

    std::uint8_t larger_array[dhorn::inplace_function<void()>::max_size + 1];
    [[maybe_unused]] auto larger_lambda = [larger_array]()
    {
        (void)larger_array;
    };

    // TODO: Should we SFINAE here? static_assert is actually probably better...
    // ASSERT_FALSE(std::is_constructible_v<inplace_function<void()>, decltype(larger_array)>);
    //inplace_function<void()> fn_fail(larger_lambda);
}

TEST_F(InplaceFunctionTests, MemberFunctionConstructionTest)
{
    dhorn::inplace_function<int(test_object&)> fn(&test_object::doit);
    ASSERT_TRUE(static_cast<bool>(fn));

    test_object f;
    ASSERT_EQ(42, fn(f));
    f.value = 8;
    ASSERT_EQ(8, fn(f));
}

TEST_F(InplaceFunctionTests, MemberPointerConstructionTest)
{
    dhorn::inplace_function<int(test_object*)> fn(&test_object::value);
    ASSERT_TRUE(static_cast<bool>(fn));

    test_object f;
    ASSERT_EQ(42, fn(&f));
    f.value = 8;
    ASSERT_EQ(8, fn(&f));
}

TEST_F(InplaceFunctionTests, DestructorTest)
{
    {
        dhorn::inplace_function<void()> fn([o = dhorn::tests::object_counter{}]() {});
        ASSERT_EQ(static_cast<std::size_t>(1), dhorn::tests::object_counter::instance_count);
        ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::copy_count);

        dhorn::inplace_function<void()> fn_copy(fn);
        ASSERT_EQ(static_cast<std::size_t>(2), dhorn::tests::object_counter::instance_count);
        ASSERT_EQ(static_cast<std::size_t>(1), dhorn::tests::object_counter::copy_count);

        dhorn::inplace_function<void()> fn_move(std::move(fn));
        ASSERT_EQ(static_cast<std::size_t>(2), dhorn::tests::object_counter::instance_count);
        ASSERT_EQ(static_cast<std::size_t>(1), dhorn::tests::object_counter::copy_count);
    }

    ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(dhorn::tests::object_counter::constructed_count, dhorn::tests::object_counter::destructed_count);
}

TEST_F(InplaceFunctionTests, NullptrAssignmentTest)
{
    dhorn::inplace_function<void()> fn([]() {});
    fn = nullptr;
    ASSERT_FALSE(static_cast<bool>(fn));

    ASSERT_THROW(fn(), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, NullFunctionPointerAssignmentTest)
{
    dhorn::inplace_function<void(const std::string&)> fn([](const std::string&) {});
    void (*nullFn)(const std::string&) = nullptr;
    fn = nullFn;
    ASSERT_FALSE(static_cast<bool>(fn));

    ASSERT_THROW(fn("foo"), std::bad_function_call);

    void (std::string::*nullMemberFnPtr)() const = nullptr;
    fn = nullMemberFnPtr;
    ASSERT_FALSE(static_cast<bool>(fn));

    ASSERT_THROW(fn("foo"), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, FunctionPointerAssignmentTest)
{
    dhorn::inplace_function<std::size_t(const char*)> fn;
    ASSERT_FALSE(static_cast<bool>(fn));

    fn = strlen;
    ASSERT_TRUE(static_cast<bool>(fn));
    ASSERT_EQ(static_cast<std::size_t>(3), fn("foo"));
}

TEST_F(InplaceFunctionTests, LambdaAssignmentTest)
{
    dhorn::inplace_function<int()> fn;
    ASSERT_FALSE(static_cast<bool>(fn));

    fn = []() { return 42; };
    ASSERT_EQ(42, fn());
}

TEST_F(InplaceFunctionTests, MutableLambdaAssignmentTest)
{
    dhorn::inplace_function<int()> fn;
    ASSERT_FALSE(static_cast<bool>(fn));

    fn = [value = 42]() mutable { return value++; };
    ASSERT_EQ(42, fn());
    ASSERT_EQ(43, fn());
}

TEST_F(InplaceFunctionTests, CopyAssignmentTest)
{
    int value = 42;
    dhorn::inplace_function<int()> fn([&]() { return value; });
    dhorn::inplace_function<int()> fn_copy;
    ASSERT_FALSE(static_cast<bool>(fn_copy));

    fn_copy = fn;
    ASSERT_TRUE(static_cast<bool>(fn));
    ASSERT_TRUE(static_cast<bool>(fn_copy));
    ASSERT_EQ(42, fn_copy());

    dhorn::inplace_function<int()> null_fn;
    fn_copy = null_fn;
    ASSERT_FALSE(static_cast<bool>(fn_copy));
    ASSERT_THROW(fn_copy(), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, MoveAssignmentTest)
{
    int value = 42;
    dhorn::inplace_function<int()> fn([&]() { return value; });
    dhorn::inplace_function<int()> fn_copy;
    ASSERT_FALSE(static_cast<bool>(fn_copy));

    fn_copy = std::move(fn);
    ASSERT_FALSE(static_cast<bool>(fn));
    ASSERT_TRUE(static_cast<bool>(fn_copy));
    ASSERT_EQ(42, fn_copy());

    dhorn::inplace_function<int()> null_fn;
    fn_copy = std::move(null_fn);
    ASSERT_FALSE(static_cast<bool>(fn_copy));
    ASSERT_THROW(fn_copy(), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, CopyAssignWithDifferentSizeTest)
{
    int value = 42;
    dhorn::inplace_function<int(), 32> fn_small([&]() { return value; });
    dhorn::inplace_function<int(), 40> fn_large;
    ASSERT_FALSE(static_cast<bool>(fn_large));

    fn_large = fn_small;
    ASSERT_TRUE(static_cast<bool>(fn_small));
    ASSERT_TRUE(static_cast<bool>(fn_large));

    ASSERT_EQ(42, fn_large());
    value = 8;
    ASSERT_EQ(8, fn_large());

    dhorn::inplace_function<int(), 32> null_fn;
    fn_large = null_fn;
    ASSERT_FALSE(static_cast<bool>(fn_large));
    ASSERT_THROW(fn_large(), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, MoveAssignWithDifferentSizeTest)
{
    int value = 42;
    dhorn::inplace_function<int(), 32> fn_small([&]() { return value; });
    dhorn::inplace_function<int(), 40> fn_large;
    ASSERT_FALSE(static_cast<bool>(fn_large));

    fn_large = std::move(fn_small);
    ASSERT_FALSE(static_cast<bool>(fn_small));
    ASSERT_TRUE(static_cast<bool>(fn_large));

    ASSERT_EQ(42, fn_large());
    value = 8;
    ASSERT_EQ(8, fn_large());

    dhorn::inplace_function<int(), 32> null_fn;
    fn_large = std::move(null_fn);
    ASSERT_FALSE(static_cast<bool>(fn_large));
    ASSERT_THROW(fn_large(), std::bad_function_call);
}

TEST_F(InplaceFunctionTests, LargeObjectAssignmentTest)
{
    std::uint8_t large_array[dhorn::inplace_function<void()>::max_size];
    auto large_lambda = [large_array]()
    {
        (void)large_array;
    };

    dhorn::inplace_function<void()> fn;
    ASSERT_FALSE(static_cast<bool>(fn));

    // Just a compilation test...
    fn = large_lambda;

    std::uint8_t larger_array[dhorn::inplace_function<void()>::max_size + 1];
    [[maybe_unused]] auto larger_lambda = [larger_array]()
    {
        (void)larger_array;
    };

    // TODO: Should we SFINAE here? static_assert is actually probably better...
    //fn = larger_lambda;
}

TEST_F(InplaceFunctionTests, MemberFunctionAssignmentTest)
{
    dhorn::inplace_function<int(test_object&)> fn;
    ASSERT_FALSE(static_cast<bool>(fn));

    fn = &test_object::doit;
    ASSERT_TRUE(static_cast<bool>(fn));

    test_object f;
    ASSERT_EQ(42, fn(f));
    f.value = 8;
    ASSERT_EQ(8, fn(f));
}

TEST_F(InplaceFunctionTests, MemberPointerAssignmentTest)
{
    dhorn::inplace_function<int(test_object*)> fn;
    ASSERT_FALSE(static_cast<bool>(fn));

    fn = &test_object::value;
    ASSERT_TRUE(static_cast<bool>(fn));

    test_object f;
    ASSERT_EQ(42, fn(&f));
    f.value = 8;
    ASSERT_EQ(8, fn(&f));
}

TEST_F(InplaceFunctionTests, AssignmentCopyMoveContentsTest)
{
    {
        dhorn::inplace_function<void()> fn;
        ASSERT_FALSE(static_cast<bool>(fn));

        fn = [o = dhorn::tests::object_counter{}]() {};
        ASSERT_EQ(static_cast<std::size_t>(1), dhorn::tests::object_counter::instance_count);
        ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::copy_count);

        dhorn::inplace_function<void()> other;
        ASSERT_FALSE(static_cast<bool>(other));

        other = fn;
        ASSERT_EQ(static_cast<std::size_t>(2), dhorn::tests::object_counter::instance_count);
        ASSERT_EQ(static_cast<std::size_t>(1), dhorn::tests::object_counter::copy_count);

        other = std::move(fn);
        ASSERT_EQ(static_cast<std::size_t>(1), dhorn::tests::object_counter::instance_count);
        ASSERT_EQ(static_cast<std::size_t>(1), dhorn::tests::object_counter::copy_count);
    }

    ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(dhorn::tests::object_counter::constructed_count, dhorn::tests::object_counter::destructed_count);
}

TEST_F(InplaceFunctionTests, SwapTest)
{
    {
        dhorn::inplace_function<int()> fn1;
        dhorn::inplace_function<int()> fn2;
        ASSERT_FALSE(static_cast<bool>(fn1));
        ASSERT_FALSE(static_cast<bool>(fn2));
        ASSERT_THROW(fn1(), std::bad_function_call);
        ASSERT_THROW(fn2(), std::bad_function_call);

        fn1.swap(fn2);
        ASSERT_FALSE(static_cast<bool>(fn1));
        ASSERT_FALSE(static_cast<bool>(fn2));
        ASSERT_THROW(fn1(), std::bad_function_call);
        ASSERT_THROW(fn2(), std::bad_function_call);

        fn1 = [o = dhorn::tests::object_counter{}]() { return 42; };
        fn1.swap(fn2);
        ASSERT_EQ(static_cast<std::size_t>(1), dhorn::tests::object_counter::instance_count);
        ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::copy_count);
        ASSERT_FALSE(static_cast<bool>(fn1));
        ASSERT_TRUE(static_cast<bool>(fn2));
        ASSERT_THROW(fn1(), std::bad_function_call);
        ASSERT_EQ(42, fn2());

        fn1.swap(fn2);
        ASSERT_EQ(static_cast<std::size_t>(1), dhorn::tests::object_counter::instance_count);
        ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::copy_count);
        ASSERT_TRUE(static_cast<bool>(fn1));
        ASSERT_FALSE(static_cast<bool>(fn2));
        ASSERT_EQ(42, fn1());
        ASSERT_THROW(fn2(), std::bad_function_call);

        fn2 = [o = dhorn::tests::object_counter{}]() { return 8; };
        fn1.swap(fn2);
        ASSERT_EQ(static_cast<std::size_t>(2), dhorn::tests::object_counter::instance_count);
        ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::copy_count);
        ASSERT_TRUE(static_cast<bool>(fn1));
        ASSERT_TRUE(static_cast<bool>(fn2));
        ASSERT_EQ(8, fn1());
        ASSERT_EQ(42, fn2());
    }

    ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::instance_count);
}

TEST_F(InplaceFunctionTests, TargetTypeTest)
{
    auto lambda = []() {};
    auto other_lambda = []() {};

    dhorn::inplace_function<void()> fn(lambda);
    ASSERT_TRUE(typeid(lambda) == fn.target_type());
    ASSERT_TRUE(typeid(other_lambda) != fn.target_type());

    fn = other_lambda;
    ASSERT_TRUE(typeid(lambda) != fn.target_type());
    ASSERT_TRUE(typeid(other_lambda) == fn.target_type());
}

TEST_F(InplaceFunctionTests, TargetTest)
{
    int value = 42;
    auto lambda = [&]() { return value; };
    using lambda_type = decltype(lambda);
    auto other_lambda = [&]() { return -value; };
    using other_lambda_type = decltype(other_lambda);

    dhorn::inplace_function<int()> fn(lambda);
    ASSERT_TRUE(fn.target<lambda_type>() != nullptr);
    ASSERT_TRUE(fn.target<other_lambda_type>() == nullptr);
    ASSERT_EQ(42, (*fn.target<lambda_type>())());
    value = 8;
    ASSERT_EQ(8, (*fn.target<lambda_type>())());

    fn = other_lambda;
    ASSERT_TRUE(fn.target<lambda_type>() == nullptr);
    ASSERT_TRUE(fn.target<other_lambda_type>() != nullptr);
    ASSERT_EQ(-8, (*fn.target<other_lambda_type>())());
    value = 42;
    ASSERT_EQ(-42, (*fn.target<other_lambda_type>())());
}

TEST_F(InplaceFunctionTests, NullEqualityTest)
{
    dhorn::inplace_function<void()> fn;
    ASSERT_TRUE(fn == nullptr);
    ASSERT_TRUE(nullptr == fn);
    ASSERT_FALSE(fn != nullptr);
    ASSERT_FALSE(nullptr != fn);

    fn = []() {};
    ASSERT_FALSE(fn == nullptr);
    ASSERT_FALSE(nullptr == fn);
    ASSERT_TRUE(fn != nullptr);
    ASSERT_TRUE(nullptr != fn);
}

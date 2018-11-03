/*
 * Duncan Horn
 *
 * UniqueAnyTests.cpp
 *
 * Tests for the dhorn unique_any class
 */
#include "stdafx.h"

#include <dhorn/experimental/unique_any.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        static int count = 0;

        TEST_CLASS(UniqueAnyTests)
        {
            class test_class
            {
            public:
                test_class()
                {
                    ++count;
                }

                test_class(const test_class &)
                {
                    ++count;
                }

                ~test_class()
                {
                    --count;
                }

                bool operator!=(const test_class &) const { return false; }
                bool operator==(const test_class &) const { return false; }
            };

            TEST_METHOD_INITIALIZE(InitializeTest)
            {
                // Reset the count before each test invocation
                count = 0;
            }

            TEST_METHOD(PointerTest)
            {
                {
                    // Default construct should not affect the count
                    dhorn::experimental::unique_any<test_class *> ptr;
                    ASSERT_EQ(0, count);

                    ptr = new test_class();
                    ASSERT_EQ(1, count);

                    // Resetting the value should destroy the previous one
                    ptr = new test_class();
                    ASSERT_EQ(1, count);
                    ptr.reset(new test_class());
                    ASSERT_EQ(1, count);
                }

                // Should have released all resources
                ASSERT_EQ(0, count);
            }

            TEST_METHOD(ArrayTest)
            {
                {
                    // Default construct should not affect the count
                    dhorn::experimental::unique_any<test_class[]> ptr;
                    ASSERT_EQ(0, count);

                    // Calling new[] with a size of 5 should mean that count is now 5 as well
                    ptr = new test_class[5];
                    ASSERT_EQ(5, count);

                    // Resetting should call the destructor for *all* elements in the array
                    ptr = new test_class[6];
                    ASSERT_EQ(6, count);
                    ptr.reset(new test_class[2]);
                    ASSERT_EQ(2, count);
                }

                // Should have released all resources
                ASSERT_EQ(0, count);
            }

            TEST_METHOD(ValueTest)
            {
                std::vector<test_class> arr(5);
                ASSERT_EQ(5, count);

                {
                    dhorn::experimental::unique_any<std::vector<test_class>> val;
                    ASSERT_EQ(5, count);

                    // Assignment should copy here (i.e. it acts like a normal non-unique value)
                    val = arr;
                    ASSERT_EQ(10, count);

                    // Moving should transfer all ownership, though
                    val.reset(std::move(arr));
                    ASSERT_EQ(5, count);
                }

                // Should have released all resources
                ASSERT_EQ(0, count);
            }
        };



        TEST_CLASS(UniqueHandleTests)
        {
            HANDLE make_valid_handle(void)
            {
                // Should never fail. Use no sharing mode so we can ensure closing
                HANDLE h = CreateFile(L"foo.txt", GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

                ASSERT_NE(h, INVALID_HANDLE_VALUE, L"Handle not previously closed :(");
                return h;
            }

            HANDLE make_valid_handle2(void)
            {
                // Should never fail. Use no sharing mode so we can ensure closing
                HANDLE h = CreateFile(L"bar.txt", GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

                ASSERT_NE(h, INVALID_HANDLE_VALUE, L"Handle not previously closed :(");
                return h;
            }

            void verify_handle_closed(void)
            {
                ASSERT_TRUE(!!CloseHandle(make_valid_handle()));
            }

            HANDLE make_invalid_handle(void)
            {
                // Random garbage. Invalid with high probability
                return reinterpret_cast<HANDLE>(4810568);
            }

            TEST_METHOD(DefaultConstructorTest)
            {
                // Should construct and should be invalid
                dhorn::experimental::unique_handle handle;
                ASSERT_TRUE(static_cast<HANDLE>(handle) == INVALID_HANDLE_VALUE);
            }

            TEST_METHOD(HandleConstructorTest)
            {
                // Construct with handle should give no errors
                {
                    auto handle = make_valid_handle();
                    dhorn::experimental::unique_handle h(handle);

                    ASSERT_NE(handle, INVALID_HANDLE_VALUE);
                    ASSERT_EQ(handle, static_cast<HANDLE>(h));
                }

                // Make sure the handle was closed
                verify_handle_closed();

                // Constructing two with same handle should fail on close
                try
                {
                    {
                        auto handle = make_valid_handle();
                        dhorn::experimental::unique_handle x(handle);
                        dhorn::experimental::unique_handle y(handle);
                    }
                    Assert::Fail(L"Expected an exception");
                }
                catch (const std::system_error& e)
                {
                    ASSERT_TRUE(e.code().value() == ERROR_INVALID_HANDLE);
                }
            }

            TEST_METHOD(CopyConstructorTest)
            {
                // Copy constructor should give build errors. Since we can't test build errors with
                // unit tests, and since std::is_copy_constructable is not robust at all, uncomment
                // the second line below to verify it does not compile...
                dhorn::experimental::unique_handle x;
                //dhorn::experimental::unique_handle y(x);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                {
                    dhorn::experimental::unique_handle x(make_valid_handle());
                    dhorn::experimental::unique_handle y(std::move(x));
                }

                // No exceptions should be thrown and the HANDLE should be closed
                verify_handle_closed();
            }

            TEST_METHOD(DestructorTest)
            {
                // We've already tested the destructor (by way of no exceptions), but let's test it
                // some more!!!
                auto test_func = [&](std::function<void(HANDLE h)> func)
                {
                    HANDLE handle = make_valid_handle();
                    ASSERT_TRUE(handle != INVALID_HANDLE_VALUE);

                    func(handle);

                    // Handle should be closed. I.e. CloseHandle should fail
                    ASSERT_FALSE(!!CloseHandle(handle));
                };

                // TEST 1 : Construct two should throw exception
                test_func([](HANDLE handle)
                {
                    try
                    {
                        {
                            dhorn::experimental::unique_handle x(handle);
                            dhorn::experimental::unique_handle y(handle);
                        }
                        Assert::Fail(L"Expected an exception (TEST 1)");
                    }
                    catch (std::system_error& e)
                    {
                        ASSERT_TRUE(e.code().value() == ERROR_INVALID_HANDLE);
                    }
                });

                // TEST 2 : Construct one is fine
                test_func([](HANDLE handle)
                {
                    dhorn::experimental::unique_handle x(handle);
                });

                // TEST 3 : Bogus handle should give exception
                try
                {
                    {
                        dhorn::experimental::unique_handle x(make_invalid_handle());
                    }
                    Assert::Fail(L"Expected an exception (TEST 3)");
                }
                catch (std::system_error& e)
                {
                    ASSERT_TRUE(e.code().value() == ERROR_INVALID_HANDLE);
                }

                // TEST 4 : No-arg should not cause exception
                {
                    dhorn::experimental::unique_handle x;
                    (void)x;
                }

                // TEST 5 : Assign to handle
                test_func([](HANDLE handle)
                {
                    dhorn::experimental::unique_handle x;

                    // Ensure no optimizations
                    ASSERT_TRUE(static_cast<HANDLE>(x) == INVALID_HANDLE_VALUE);

                    x = handle;
                });

                // TEST 6 : Assign after creation
                test_func([&](HANDLE handle)
                {
                    HANDLE other = make_valid_handle2();
                    {
                        dhorn::experimental::unique_handle x(handle);
                        x = other;
                    }

                    ASSERT_FALSE(!!CloseHandle(other));
                });

                // TEST 7 : Assign to same should throw exception
                test_func([](HANDLE handle)
                {
                    try
                    {
                        {
                            dhorn::experimental::unique_handle x(handle);
                            dhorn::experimental::unique_handle y;

                            // Ensure no optimizations
                            ASSERT_TRUE(static_cast<HANDLE>(y) == INVALID_HANDLE_VALUE);

                            y = handle;
                        }
                        Assert::Fail(L"Expected an exception (TEST 7)");
                    }
                    catch (std::system_error& e)
                    {
                        ASSERT_TRUE(e.code().value() == ERROR_INVALID_HANDLE);
                    }
                });
            }

            TEST_METHOD(HandleAssignmentTest)
            {
                // Assignment should work
                {
                    dhorn::experimental::unique_handle x;
                    ASSERT_TRUE(static_cast<HANDLE>(x) == INVALID_HANDLE_VALUE);

                    x = make_valid_handle();
                }
                verify_handle_closed();

                // Re-assignment should work as well
                {
                    dhorn::experimental::unique_handle x = make_valid_handle();
                    x = make_valid_handle2();
                }
                verify_handle_closed();
                ASSERT_TRUE(!!CloseHandle(make_valid_handle2()));

                // Assigning the same handle to same object should fail
                try
                {
                    HANDLE handle = make_valid_handle();
                    {
                        dhorn::experimental::unique_handle x = handle;
                        x = handle;
                    }
                    Assert::Fail(L"Expected an exception");
                }
                catch (std::system_error& e)
                {
                    ASSERT_TRUE(e.code().value() == ERROR_INVALID_HANDLE);
                }
            }

            TEST_METHOD(CopyAssignmentTest)
            {
                // Copy assignment should give build errors. Since we can't test build errors with
                // unit tests, and since std::is_assignable is not robust at all, uncomment the
                // line below to verify it does not compile...
                dhorn::experimental::unique_handle x;
                dhorn::experimental::unique_handle y;

                ASSERT_TRUE(static_cast<HANDLE>(y) == INVALID_HANDLE_VALUE);

                // y = x;
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                dhorn::experimental::unique_handle x(make_valid_handle());
                {
                    dhorn::experimental::unique_handle y;

                    ASSERT_TRUE(static_cast<HANDLE>(y) == INVALID_HANDLE_VALUE);

                    y = std::move(x);
                    x = std::move(y);
                    y = std::move(x);
                }

                // y got destroyed, so it should have closed the handle as well
                verify_handle_closed();
            }

            TEST_METHOD(InvalidTest)
            {
                // Default should be invalid
                dhorn::experimental::unique_handle x;
                ASSERT_FALSE(x);

                // Assign to non-invalid
                x = make_valid_handle();
                ASSERT_TRUE(x);

                // Move should make invalid again
                dhorn::experimental::unique_handle y(std::move(x));
                ASSERT_FALSE(x);
                ASSERT_TRUE(y);

                x = std::move(y);
                ASSERT_TRUE(x);
                ASSERT_FALSE(y);
            }

            TEST_METHOD(ResetTest)
            {
                HANDLE handle = make_valid_handle();
                dhorn::experimental::unique_handle x(handle);
                {
                    x.reset();

                    // Calling reset twice should be harmless
                    x.reset();
                }
                // Handle should now be closed and x should be invalid
                ASSERT_TRUE(!!CloseHandle(make_valid_handle()));
                ASSERT_FALSE(x);

                // Calling reset on an invalid handle should be harmless
                dhorn::experimental::unique_handle y;
                y.reset();
                ASSERT_FALSE(y);
            }

            TEST_METHOD(SwapTest)
            {
                try
                {
                    HANDLE one = make_valid_handle();
                    HANDLE two = make_valid_handle2();
                    {
                        dhorn::experimental::unique_handle x(one);
                        {
                            dhorn::experimental::unique_handle y(two);
                            x.swap(y);
                        }
                        // one should be closed and two should still be active
                        verify_handle_closed();
                        ASSERT_FALSE(!!CloseHandle(one));
                        ASSERT_TRUE(!!CloseHandle(two));
                    }
                    Assert::Fail(L"Expected an exception");
                }
                catch (std::system_error& e)
                {
                    ASSERT_TRUE(e.code().value() == ERROR_INVALID_HANDLE);
                }
            }
        };
    }
}

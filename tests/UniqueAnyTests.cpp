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
                    Assert::AreEqual(0, count);

                    ptr = new test_class();
                    Assert::AreEqual(1, count);

                    // Resetting the value should destroy the previous one
                    ptr = new test_class();
                    Assert::AreEqual(1, count);
                    ptr.reset(new test_class());
                    Assert::AreEqual(1, count);
                }

                // Should have released all resources
                Assert::AreEqual(0, count);
            }

            TEST_METHOD(ArrayTest)
            {
                {
                    // Default construct should not affect the count
                    dhorn::experimental::unique_any<test_class[]> ptr;
                    Assert::AreEqual(0, count);

                    // Calling new[] with a size of 5 should mean that count is now 5 as well
                    ptr = new test_class[5];
                    Assert::AreEqual(5, count);

                    // Resetting should call the destructor for *all* elements in the array
                    ptr = new test_class[6];
                    Assert::AreEqual(6, count);
                    ptr.reset(new test_class[2]);
                    Assert::AreEqual(2, count);
                }

                // Should have released all resources
                Assert::AreEqual(0, count);
            }

            TEST_METHOD(ValueTest)
            {
                std::vector<test_class> arr(5);
                Assert::AreEqual(5, count);

                {
                    dhorn::experimental::unique_any<std::vector<test_class>> val;
                    Assert::AreEqual(5, count);

                    // Assignment should copy here (i.e. it acts like a normal non-unique value)
                    val = arr;
                    Assert::AreEqual(10, count);

                    // Moving should transfer all ownership, though
                    val.reset(std::move(arr));
                    Assert::AreEqual(5, count);
                }

                // Should have released all resources
                Assert::AreEqual(0, count);
            }
        };



        TEST_CLASS(UniqueHandleTests)
        {
            HANDLE make_valid_handle(void)
            {
                // Should never fail. Use no sharing mode so we can ensure closing
                HANDLE h = CreateFile(L"foo.txt", GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

                Assert::AreNotEqual(h, INVALID_HANDLE_VALUE, L"Handle not previously closed :(");
                return h;
            }

            HANDLE make_valid_handle2(void)
            {
                // Should never fail. Use no sharing mode so we can ensure closing
                HANDLE h = CreateFile(L"bar.txt", GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

                Assert::AreNotEqual(h, INVALID_HANDLE_VALUE, L"Handle not previously closed :(");
                return h;
            }

            void verify_handle_closed(void)
            {
                Assert::IsTrue(!!CloseHandle(make_valid_handle()));
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
                Assert::IsTrue(static_cast<HANDLE>(handle) == INVALID_HANDLE_VALUE);
            }

            TEST_METHOD(HandleConstructorTest)
            {
                // Construct with handle should give no errors
                {
                    auto handle = make_valid_handle();
                    dhorn::experimental::unique_handle h(handle);

                    Assert::AreNotEqual(handle, INVALID_HANDLE_VALUE);
                    Assert::AreEqual(handle, static_cast<HANDLE>(h));
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
                catch (const dhorn::experimental::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
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
                    Assert::IsTrue(handle != INVALID_HANDLE_VALUE);

                    func(handle);

                    // Handle should be closed. I.e. CloseHandle should fail
                    Assert::IsFalse(!!CloseHandle(handle));
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
                    catch (dhorn::experimental::win32_exception &e)
                    {
                        Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
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
                catch (dhorn::experimental::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
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
                    Assert::IsTrue(static_cast<HANDLE>(x) == INVALID_HANDLE_VALUE);

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

                    Assert::IsFalse(!!CloseHandle(other));
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
                            Assert::IsTrue(static_cast<HANDLE>(y) == INVALID_HANDLE_VALUE);

                            y = handle;
                        }
                        Assert::Fail(L"Expected an exception (TEST 7)");
                    }
                    catch (dhorn::experimental::win32_exception &e)
                    {
                        Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                    }
                });
            }

            TEST_METHOD(HandleAssignmentTest)
            {
                // Assignment should work
                {
                    dhorn::experimental::unique_handle x;
                    Assert::IsTrue(static_cast<HANDLE>(x) == INVALID_HANDLE_VALUE);

                    x = make_valid_handle();
                }
                verify_handle_closed();

                // Re-assignment should work as well
                {
                    dhorn::experimental::unique_handle x = make_valid_handle();
                    x = make_valid_handle2();
                }
                verify_handle_closed();
                Assert::IsTrue(!!CloseHandle(make_valid_handle2()));

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
                catch (dhorn::experimental::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                }
            }

            TEST_METHOD(CopyAssignmentTest)
            {
                // Copy assignment should give build errors. Since we can't test build errors with
                // unit tests, and since std::is_assignable is not robust at all, uncomment the
                // line below to verify it does not compile...
                dhorn::experimental::unique_handle x;
                dhorn::experimental::unique_handle y;

                Assert::IsTrue(static_cast<HANDLE>(y) == INVALID_HANDLE_VALUE);

                // y = x;
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                dhorn::experimental::unique_handle x(make_valid_handle());
                {
                    dhorn::experimental::unique_handle y;

                    Assert::IsTrue(static_cast<HANDLE>(y) == INVALID_HANDLE_VALUE);

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
                Assert::IsFalse(x);

                // Assign to non-invalid
                x = make_valid_handle();
                Assert::IsTrue(x);

                // Move should make invalid again
                dhorn::experimental::unique_handle y(std::move(x));
                Assert::IsFalse(x);
                Assert::IsTrue(y);

                x = std::move(y);
                Assert::IsTrue(x);
                Assert::IsFalse(y);
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
                Assert::IsTrue(!!CloseHandle(make_valid_handle()));
                Assert::IsFalse(x);

                // Calling reset on an invalid handle should be harmless
                dhorn::experimental::unique_handle y;
                y.reset();
                Assert::IsFalse(y);
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
                        Assert::IsFalse(!!CloseHandle(one));
                        Assert::IsTrue(!!CloseHandle(two));
                    }
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                }
            }
        };
    }
}

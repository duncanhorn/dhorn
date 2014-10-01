/*
 * Duncan Horn
 *
 * WindowsTests
 *
 * Tests for various Win32 functions and wrappers
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include "dhorn\windows\windows.h"
#include <functional>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        /*
         * unique_handle Tests
         */
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
                win32::unique_handle handle;
                Assert::IsTrue(static_cast<HANDLE>(handle) == INVALID_HANDLE_VALUE);
            }

            TEST_METHOD(HandleConstructorTest)
            {
                // Construct with handle should give no errors
                {
                    auto handle = make_valid_handle();
                    dhorn::win32::unique_handle h(handle);

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
                        dhorn::win32::unique_handle x(handle);
                        dhorn::win32::unique_handle y(handle);
                    }
                    Assert::Fail(L"Expected an exception");
                }
                catch (const dhorn::win32::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                }
            }

            TEST_METHOD(CopyConstructorTest)
            {
                // Copy constructor should give build errors. Since we can't test build errors with
                // unit tests, and since std::is_copy_constructable is not robust at all, uncomment
                // the second line below to verify it does not compile...
                dhorn::win32::unique_handle x;
                //dhorn::win32::unique_handle y(x);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                {
                    dhorn::win32::unique_handle x(make_valid_handle());
                    dhorn::win32::unique_handle y(std::move(x));
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
                            dhorn::win32::unique_handle x(handle);
                            dhorn::win32::unique_handle y(handle);
                        }
                        Assert::Fail(L"Expected an exception (TEST 1)");
                    }
                    catch (dhorn::win32::win32_exception &e)
                    {
                        Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                    }
                });

                // TEST 2 : Construct one is fine
                test_func([](HANDLE handle)
                {
                    dhorn::win32::unique_handle x(handle);
                });

                // TEST 3 : Bogus handle should give exception
                try
                {
                    {
                        dhorn::win32::unique_handle x(make_invalid_handle());
                    }
                    Assert::Fail(L"Expected an exception (TEST 3)");
                }
                catch (dhorn::win32::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                }

                // TEST 4 : No-arg should not cause exception
                {
                    dhorn::win32::unique_handle x;
                    (void)x;
                }

                // TEST 5 : Assign to handle
                test_func([](HANDLE handle)
                {
                    dhorn::win32::unique_handle x;

                    // Ensure no optimizations
                    Assert::IsTrue(static_cast<HANDLE>(x) == INVALID_HANDLE_VALUE);

                    x = handle;
                });

                // TEST 6 : Assign after creation
                test_func([&](HANDLE handle)
                {
                    HANDLE other = make_valid_handle2();
                    {
                        dhorn::win32::unique_handle x(handle);
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
                            dhorn::win32::unique_handle x(handle);
                            dhorn::win32::unique_handle y;

                            // Ensure no optimizations
                            Assert::IsTrue(static_cast<HANDLE>(y) == INVALID_HANDLE_VALUE);

                            y = handle;
                        }
                        Assert::Fail(L"Expected an exception (TEST 7)");
                    }
                    catch (dhorn::win32::win32_exception &e)
                    {
                        Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                    }
                });
            }

            TEST_METHOD(HandleAssignmentTest)
            {
                // Assignment should work
                {
                    dhorn::win32::unique_handle x;
                    Assert::IsTrue(static_cast<HANDLE>(x) == INVALID_HANDLE_VALUE);

                    x = make_valid_handle();
                }
                verify_handle_closed();

                // Re-assignment should work as well
                {
                    dhorn::win32::unique_handle x = make_valid_handle();
                    x = make_valid_handle2();
                }
                verify_handle_closed();
                Assert::IsTrue(!!CloseHandle(make_valid_handle2()));

                // Assigning the same handle to same object should fail
                try
                {
                    HANDLE handle = make_valid_handle();
                    {
                        dhorn::win32::unique_handle x = handle;
                        x = handle;
                    }
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::win32::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                }
            }

            TEST_METHOD(CopyAssignmentTest)
            {
                // Copy assignment should give build errors. Since we can't test build errors with
                // unit tests, and since std::is_assignable is not robust at all, uncomment the
                // line below to verify it does not compile...
                dhorn::win32::unique_handle x;
                dhorn::win32::unique_handle y;

                Assert::IsTrue(static_cast<HANDLE>(y) == INVALID_HANDLE_VALUE);

                // y = x;
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                dhorn::win32::unique_handle x(make_valid_handle());
                {
                    dhorn::win32::unique_handle y;

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
                dhorn::win32::unique_handle x;
                Assert::IsTrue(x.invalid());

                // Assign to non-invalid
                x = make_valid_handle();
                Assert::IsFalse(x.invalid());

                // Move should make invalid again
                dhorn::win32::unique_handle y(std::move(x));
                Assert::IsTrue(x.invalid());
                Assert::IsFalse(y.invalid());

                x = std::move(y);
                Assert::IsFalse(x.invalid());
                Assert::IsTrue(y.invalid());
            }

            TEST_METHOD(ReleaseTest)
            {
                HANDLE handle = make_valid_handle();
                dhorn::win32::unique_handle x(handle);
                {
                    x.release();

                    // Calling release twice should be harmless
                    x.release();
                }
                // Handle should now be closed and x should be invalid
                Assert::IsTrue(!!CloseHandle(make_valid_handle()));
                Assert::IsTrue(x.invalid());

                // Calling release on an invalid handle should be harmless
                dhorn::win32::unique_handle y;
                y.release();
                Assert::IsTrue(y.invalid());
            }

            TEST_METHOD(SwapTest)
            {
                try
                {
                    HANDLE one = make_valid_handle();
                    HANDLE two = make_valid_handle2();
                    {
                        dhorn::win32::unique_handle x(one);
                        {
                            dhorn::win32::unique_handle y(two);
                            x.swap(y);
                        }
                        // one should be closed and two should still be active
                        verify_handle_closed();
                        Assert::IsFalse(!!CloseHandle(one));
                        Assert::IsTrue(!!CloseHandle(two));
                    }
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::win32::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                }
            }
        };



        /*
         * shared_handle Tests
         */
        TEST_CLASS(SharedHandleTests)
        {
            HANDLE make_valid_handle_no_check(void)
            {
                return CreateFile(L"foo.txt", GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            }

            HANDLE make_valid_handle(void)
            {
                // Should never fail. Use no sharing mode so we can ensure closing
                HANDLE h = make_valid_handle_no_check();

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
                win32::shared_handle handle;
                Assert::IsTrue(static_cast<HANDLE>(handle) == INVALID_HANDLE_VALUE);
            }

            TEST_METHOD(HandleConstructorTest)
            {
                // Construct with handle should give no errors
                {
                    auto handle = make_valid_handle();
                    dhorn::win32::shared_handle h(handle);

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
                        dhorn::win32::shared_handle x(handle);
                        dhorn::win32::shared_handle y(handle);
                    }
                    Assert::Fail(L"Expected an exception");
                }
                catch (const dhorn::win32::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                }
            }

            TEST_METHOD(CopyConstructorTest)
            {
                {
                    // Copying should be fine
                    dhorn::win32::shared_handle x(make_valid_handle());
                    dhorn::win32::shared_handle y(x);
                }
                // Should have closed the handle
                verify_handle_closed();

                {
                    dhorn::win32::shared_handle x(make_valid_handle());
                    {
                        dhorn::win32::shared_handle y(x);
                    }

                    // Even though y closed its handle, x's handle should still be open and valid
                    Assert::AreEqual(make_valid_handle_no_check(), INVALID_HANDLE_VALUE);
                    Assert::IsTrue(!!WriteFile(x, "hello, world!", 12, nullptr, nullptr));
                }
                // Should have closed the handle
                verify_handle_closed();

                {
                    // vector of copies
                    dhorn::win32::shared_handle x(make_valid_handle());

                    std::vector<dhorn::win32::shared_handle> v;
                    for (int i = 0; i < 10; i++) { v.emplace_back(x); }
                }
                verify_handle_closed();
            }

            TEST_METHOD(MoveConstructorTest)
            {
                {
                    dhorn::win32::shared_handle x(make_valid_handle());
                    dhorn::win32::shared_handle y(std::move(x));
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
                            dhorn::win32::shared_handle x(handle);
                            dhorn::win32::shared_handle y(handle);
                        }
                        Assert::Fail(L"Expected an exception (TEST 1)");
                    }
                    catch (dhorn::win32::win32_exception &e)
                    {
                        Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                    }
                });

                // TEST 2 : Construct one is fine
                test_func([](HANDLE handle)
                {
                    dhorn::win32::shared_handle x(handle);
                });

                // TEST 3 : Bogus handle should give exception
                try
                {
                    {
                        dhorn::win32::shared_handle x(make_invalid_handle());
                    }
                    Assert::Fail(L"Expected an exception (TEST 3)");
                }
                catch (dhorn::win32::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                }

                // TEST 4 : No-arg should not cause exception
                {
                    dhorn::win32::shared_handle x;
                    (void)x;
                }

                // TEST 5 : Assign to handle
                test_func([](HANDLE handle)
                {
                    dhorn::win32::shared_handle x;

                    // Ensure no optimizations
                    Assert::IsTrue(static_cast<HANDLE>(x) == INVALID_HANDLE_VALUE);

                    x = handle;
                });

                // TEST 6 : Assign after creation
                test_func([&](HANDLE handle)
                {
                    HANDLE other = make_valid_handle2();
                    {
                        dhorn::win32::shared_handle x(handle);
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
                            dhorn::win32::shared_handle x(handle);
                            dhorn::win32::shared_handle y;

                            // Ensure no optimizations
                            Assert::IsTrue(static_cast<HANDLE>(y) == INVALID_HANDLE_VALUE);

                            y = handle;
                        }
                        Assert::Fail(L"Expected an exception (TEST 7)");
                    }
                    catch (dhorn::win32::win32_exception &e)
                    {
                        Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                    }
                });
            }

            TEST_METHOD(HandleAssignmentTest)
            {
                // Assignment should work
                {
                    dhorn::win32::shared_handle x;
                    Assert::IsTrue(static_cast<HANDLE>(x) == INVALID_HANDLE_VALUE);

                    x = make_valid_handle();
                }
                verify_handle_closed();

                // Re-assignment should work as well
                {
                    dhorn::win32::shared_handle x = make_valid_handle();
                    x = make_valid_handle2();
                }
                verify_handle_closed();
                Assert::IsTrue(!!CloseHandle(make_valid_handle2()));

                // Assigning the same handle to same object should fail
                try
                {
                    HANDLE handle = make_valid_handle();
                    {
                        dhorn::win32::shared_handle x = handle;
                        x = handle;
                    }
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::win32::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                }
            }

            TEST_METHOD(CopyAssignmentTest)
            {
                {
                    // Copying should be fine
                    dhorn::win32::shared_handle x(make_valid_handle());
                    dhorn::win32::shared_handle y;

                    Assert::IsTrue(static_cast<HANDLE>(y) == INVALID_HANDLE_VALUE);

                    y = x;
                }
                // Should have closed the handle
                verify_handle_closed();

                {
                    dhorn::win32::shared_handle x(make_valid_handle());
                    {
                        dhorn::win32::shared_handle y;

                        Assert::IsTrue(static_cast<HANDLE>(y) == INVALID_HANDLE_VALUE);

                        y = x;
                    }

                    // Even though y closed its handle, x's handle should still be open and valid
                    Assert::AreEqual(make_valid_handle_no_check(), INVALID_HANDLE_VALUE);
                    Assert::IsTrue(!!WriteFile(x, "hello, world!", 12, nullptr, nullptr));
                }
                // Should have closed the handle
                verify_handle_closed();

                {
                    // vector of copies
                    dhorn::win32::shared_handle x(make_valid_handle());

                    std::vector<dhorn::win32::shared_handle> v(10);
                    for (int i = 0; i < 10; i++) { v[i] = x; }
                }
                verify_handle_closed();
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                dhorn::win32::shared_handle x(make_valid_handle());
                {
                    dhorn::win32::shared_handle y;

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
                dhorn::win32::shared_handle x;
                Assert::IsTrue(x.invalid());

                // Assign to non-invalid
                x = make_valid_handle();
                Assert::IsFalse(x.invalid());

                // Move should make invalid again
                dhorn::win32::shared_handle y(std::move(x));
                Assert::IsTrue(x.invalid());
                Assert::IsFalse(y.invalid());

                x = std::move(y);
                Assert::IsFalse(x.invalid());
                Assert::IsTrue(y.invalid());
            }

            TEST_METHOD(ReleaseTest)
            {
                HANDLE handle = make_valid_handle();
                dhorn::win32::shared_handle x(handle);
                {
                    x.release();

                    // Calling release twice should be harmless
                    x.release();
                }
                // Handle should now be closed and x should be invalid
                Assert::IsTrue(!!CloseHandle(make_valid_handle()));
                Assert::IsTrue(x.invalid());

                // Calling release on an invalid handle should be harmless
                dhorn::win32::shared_handle y;
                y.release();
                Assert::IsTrue(y.invalid());
            }

            TEST_METHOD(SwapTest)
            {
                try
                {
                    HANDLE one = make_valid_handle();
                    HANDLE two = make_valid_handle2();
                    {
                        dhorn::win32::shared_handle x(one);
                        {
                            dhorn::win32::shared_handle y(two);
                            x.swap(y);
                        }
                        // one should be closed and two should still be active
                        verify_handle_closed();
                        Assert::IsFalse(!!CloseHandle(one));
                        Assert::IsTrue(!!CloseHandle(two));
                    }
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::win32::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_INVALID_HANDLE);
                }
            }
        };



        /*
         * File Management Tests
         */
        TEST_CLASS(FileManagementTests)
        {
            TEST_METHOD(CreateFileTest)
            {
                // Test with unique_handle
                {
                    // Write some data
                    dhorn::win32::unique_handle x;
                    x = dhorn::win32::create_file(L"foo.txt", GENERIC_READ | GENERIC_WRITE, 0,
                        nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);

                    char message[] = "Hello, world!";
                    Assert::IsTrue(!!WriteFile(x, message, sizeof(message) - 1, nullptr, nullptr));
                    x.release();

                    // Now read what we wrote
                    x = dhorn::win32::create_file(L"foo.txt", GENERIC_READ | GENERIC_WRITE, 0,
                        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);

                    char result[sizeof(message)]; result[sizeof(message) - 1] = '\0';
                    Assert::IsTrue(!!ReadFile(x, result, sizeof(result) - 1, nullptr, nullptr));
                    Assert::IsTrue(strcmp(message, result) == 0);
                }

                // Test with shared_handle
                {
                    dhorn::win32::shared_handle x;
                    x = dhorn::win32::create_file<dhorn::win32::shared_handle_traits>(L"foo.txt",
                        GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL);
                    dhorn::win32::shared_handle y = x;

                    // Write some data
                    char message[] = "Hello, world!";
                    Assert::IsTrue(!!WriteFile(x, message, sizeof(message) - 1, nullptr, nullptr));

                    // Now read what we wrote
                    Assert::IsTrue(SetFilePointer(y, 0, nullptr, FILE_BEGIN) !=
                        INVALID_SET_FILE_POINTER);
                    char result[sizeof(message)]; result[sizeof(message) - 1] = '\0';
                    Assert::IsTrue(!!ReadFile(y, result, sizeof(result) - 1, nullptr, nullptr));
                    Assert::IsTrue(strcmp(message, result) == 0);
                }

            }
        };
    }
}

/*
 * Duncan Horn
 *
 * WindowsTests
 *
 * Tests for various Win32 functions and wrappers
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include "dhorn/unique_any.h"
#include "dhorn/windows/windows.h"
#include <functional>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
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
                    dhorn::unique_handle x;
                    x = dhorn::win32::create_file(L"foo.txt", GENERIC_READ | GENERIC_WRITE, 0,
                        nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);

                    char message[] = "Hello, world!";
                    Assert::IsTrue(!!WriteFile(x, message, sizeof(message) - 1, nullptr, nullptr));
                    x.reset();

                    // Now read what we wrote
                    x = dhorn::win32::create_file(L"foo.txt", GENERIC_READ | GENERIC_WRITE, 0,
                        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);

                    char result[sizeof(message)]; result[sizeof(message) - 1] = '\0';
                    Assert::IsTrue(!!ReadFile(x, result, sizeof(result) - 1, nullptr, nullptr));
                    Assert::IsTrue(strcmp(message, result) == 0);
                }

                // Try to create bogus file (should throw)
                try
                {
                    {
                        dhorn::unique_handle h =
                            dhorn::win32::create_file(L"this\\is\\bogus.txt", GENERIC_READ, 0,
                            nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL);
                        (void)h;
                    }
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::win32::win32_exception &e)
                {
                    Assert::IsTrue(e.get_status() == ERROR_PATH_NOT_FOUND);
                }
            }
        };



        /*
         * Window Function Tests
         */
        TEST_CLASS(WindowFunctionTests)
        {
            TEST_METHOD(AdjustWindowRectTest)
            {
                // Should be larger than what we provide
                RECT input{ 0, 0, 100, 100 };
                RECT output = dhorn::win32::adjust_window_rect(input, WS_OVERLAPPEDWINDOW, false);
                Assert::IsTrue((output.right - output.left > 100));
                Assert::IsTrue((output.bottom - output.top) > 100);

                // Test the Ex version
                output = dhorn::win32::adjust_window_rect(input, WS_OVERLAPPED, false, WS_EX_OVERLAPPEDWINDOW);
                Assert::IsTrue((output.right - output.left > 100));
                Assert::IsTrue((output.bottom - output.top) > 100);
            }

            TEST_METHOD(AllowSetForegroundWindowTest)
            {
                // Allowing random pid to set foreground window should cause an exception with
                // high probability
                try
                {
                    dhorn::win32::allow_set_foreground_window(87322456);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::win32::win32_exception &)
                {
                }
            }
        };
    }
}

/*
 * Duncan Horn
 *
 * main.cpp
 *
 * Entry for window functional test
 */

#include "precomp.h"

#include "globals.h"

// Define globals here
dhorn::win32::window globals::window;
worker globals::worker;

int WINAPI wWinMain(
    _In_ dhorn::win32::instance_handle instance,
    _In_opt_ dhorn::win32::instance_handle /*prevInstance*/,
    _In_ wchar_t * /*cmdLine*/,
    _In_ int cmdShow)
{
    // Initialize the window
    dhorn::win32::window_class windowClass(L"Test Window");
    windowClass.use_defaults();
    windowClass.instance = instance;

    dhorn::win32::window_options options(L"Test Window");
    options.style = dhorn::win32::window_style::overlapped_window;
    options.width = 600;
    options.height = 400;

    globals::window.on_initialized([]()
    {
        // Start the worker
        globals::worker.start();
    });

    // Run the application!
    globals::window.run(windowClass, options, cmdShow);
    globals::worker.exit();
}

/*
 * Duncan Horn
 *
 * main.cpp
 *
 * Functional tests for the dhorn::d3d namespace. Namely the d3d_window class
 */

#include <dhorn/d3d.h>
#include <dhorn/d3d/colors.h>

#include "geometry.h"
#include "globals.h"

// Define the globals
dhorn::d3d::d3d_window globals::window;
dhorn::d3d::camera globals::camera;

#if defined(DEBUG) || defined(_DEBUG)
#define OUTPUT_PATH "Debug\\"
#else
#define OUTPUT_PATH "Release\\"
#endif


int WINAPI wWinMain(
    _In_ dhorn::win32::instance_handle instance,
    _In_ dhorn::win32::instance_handle /*prevInstance*/,
    _In_ wchar_t * /*cmdLine*/,
    _In_ int cmdShow)
{
    // Initialize the window
    dhorn::win32::window_class windowClass(L"D3D Test Window");
    windowClass.use_defaults();
    windowClass.instance = instance;

    dhorn::win32::window_options options(L"D3D Test Window");
    options.width = 600;
    options.height = 400;

    globals::window.set_background(dhorn::d3d::colors::burnt_orange);

    globals::window.on_initialized([&](void)
    {

    });

    // Run the application!
    globals::window.run(windowClass, options, cmdShow);
}

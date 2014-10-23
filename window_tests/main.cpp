/*
 * Duncan Horn
 *
 * main.cpp
 *
 * Entry for window functional test
 */

#include <dhorn/windows/window.h>

int WINAPI wWinMain(
    _In_ dhorn::win32::instance_handle instance,
    _In_ dhorn::win32::instance_handle prevInstance,
    _In_ wchar_t *cmdLine,
    _In_ int cmdShow)
{
    dhorn::win32::window window;
    dhorn::win32::window_class windowClass(L"Test Window");
    dhorn::win32::window_options options(L"Test Window");

    windowClass.use_defaults();
    windowClass.instance = instance;

    options.style = dhorn::win32::window_style::overlapped_window;

    dhorn::win32::callback_handler createHandler(dhorn::win32::window_message::key_down, 1, false,
        [](dhorn::win32::window *sender, uintptr_t, intptr_t) -> std::pair<bool, uintptr_t>
    {
        sender->post_async([]()
        {
            int x = 1;
        });

        return std::make_pair(true, 0);
    });
    window.add_callback_handler(createHandler);

    window.run(windowClass, options, cmdShow);
}

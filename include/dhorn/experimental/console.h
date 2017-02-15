/*
 * Duncan Horn
 *
 * console.h
 *
 * Utility functions for dealing with console output. The primary 
 */
#pragma once

#if !(defined WIN32_LEAN_AND_MEAN) && !(defined DHORN_NO_WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN 1
#endif

#if !(defined NOMINMAX) && !(defined DHORN_NO_NOMINMAX)
#define NOMINMAX 1
#endif

#include <Windows.h>

#include <cstdint>
#include <iostream>
#include <string>

#include "../bitmask.h"
#include "math.h"
#include "../scope_guard.h"

namespace dhorn::experimental
{
    /*
     * console_device
     *
     * Represents a console device (input, output, or error) that is required for several of the console functions.
     */
    enum class console_device : uint32_t
    {
        input = STD_INPUT_HANDLE,
        output = STD_OUTPUT_HANDLE,
        error = STD_ERROR_HANDLE,
    };



    /*
     * console_color
     *
     * Represents a foreground or background color within the console.
     */
    enum class console_color : uint8_t
    {
        black = 0x00,
        dark_gray = FOREGROUND_INTENSITY,

        dark_red = FOREGROUND_RED,
        red = (dark_red | FOREGROUND_INTENSITY),

        dark_green = FOREGROUND_GREEN,
        green = (dark_green | FOREGROUND_INTENSITY),

        dark_blue = FOREGROUND_BLUE,
        blue = (dark_blue | FOREGROUND_INTENSITY),

        dark_magenta = (dark_red | dark_blue),
        magenta = (dark_magenta | FOREGROUND_INTENSITY),

        dark_cyan = (dark_green | dark_blue),
        cyan = (dark_cyan | FOREGROUND_INTENSITY),

        dark_yellow = (dark_red | dark_green),
        yellow = (dark_yellow | FOREGROUND_INTENSITY),

        gray = (dark_red | dark_green | dark_blue),
        white = (gray | FOREGROUND_INTENSITY),
    };



#pragma region Helpers

    namespace details
    {
        inline CONSOLE_SCREEN_BUFFER_INFO console_info(HANDLE device)
        {
            CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
            if (!::GetConsoleScreenBufferInfo(device, &bufferInfo))
            {
                throw std::system_error(::GetLastError(), std::system_category());
            }

            return bufferInfo;
        }

        inline CONSOLE_SCREEN_BUFFER_INFO console_info(console_device device)
        {
            return console_info(::GetStdHandle(static_cast<DWORD>(device)));
        }
    }

#pragma endregion



    /*
     * console
     */
    struct console
    {
        /*
         * console::bounds
         *
         * Returns the size of the console buffer that is visible in units of rows x columns as well as the
         * position of the console buffer that is visible in the top left corner as a (row, column) pair.
         */
        static rect<int16_t> bounds(void)
        {
            auto info = details::console_info(console_device::output);
            return
            {
                info.srWindow.Left,
                info.srWindow.Top,
                info.srWindow.Right - info.srWindow.Left + 1,
                info.srWindow.Bottom - info.srWindow.Top + 1
            };
        }

        /*
         * console::buffer_size
         *
         * Returns the size of the console buffer in units of rows x columns. Note how this differs from the
         * bounds, which only includes the size of the console buffer that is visible to the user.
         */
        static size<int16_t> buffer_size(void)
        {
            auto info = details::console_info(console_device::output);
            return{ info.dwSize.X, info.dwSize.Y };
        }

        /*
         * console::cursor_position
         *
         * Returns the position of the cursor within the console buffer. Note that this is the position relative to
         * the console buffer, and *not* the visible window (i.e. not relative to the bounds).
         */
        static point<int16_t> cursor_position(void)
        {
            auto info = details::console_info(console_device::output);
            return{ info.dwCursorPosition.X, info.dwCursorPosition.Y };
        }

        /*
         * console::set_title
         *
         * Sets the title of the console window.
         */
        static void set_title(const wchar_t *title)
        {
            if (!::SetConsoleTitle(title))
            {
                throw std::system_error(::GetLastError(), std::system_category());
            }
        }

        /*
         * console::foreground
         *
         * Returns the color that's currently being used for the foreground text of the console window.
         */
        static console_color foreground(void)
        {
            auto info = details::console_info(console_device::output);
            return static_cast<console_color>(info.wAttributes & 0x0F);
        }

        /*
         * console::set_foreground
         *
         * Sets the foreground text color of the console window. Note that this only applies to the text that has
         * yet to be printed out. This function returns a scope exit function that, when destroyed, reverts the
         * foreground color back to what it was before. For example:
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * // Foreground color is currently gray
         * {
         *     auto printRed = console::set_foreground(console_color::red);
         *     // Foreground color is now red
         * }
         * // Foreground color is gray again
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * // Foreground color is currently gray
         * {
         *     auto printRed = console::set_foreground(console_color::red);
         *     // Foreground color is now red
         *     printRed.cancel();
         * }
         * // Foreground color is still red
         */
        static auto set_foreground(console_color color)
        {
            auto handle = ::GetStdHandle(static_cast<DWORD>(console_device::output));
            auto info = details::console_info(handle);
            auto result = make_scope_guard([attr = info.wAttributes, handle]() noexcept
            {
                ::SetConsoleTextAttribute(handle, attr);
            });

            info.wAttributes = clear_flag(info.wAttributes, 0x0F);
            info.wAttributes = set_flag(info.wAttributes, static_cast<uint8_t>(color));
            if (!::SetConsoleTextAttribute(handle, info.wAttributes))
            {
                result.cancel();
                throw std::system_error(::GetLastError(), std::system_category());
            }

            return result;
        }

        /*
         * console::background
         *
         * Returns the color that's currently being used for the background of the console window.
         */
        static console_color background(void)
        {
            auto info = details::console_info(console_device::output);
            return static_cast<console_color>((info.wAttributes >> 4) & 0x0F);
        }

        /*
         * console::set_background
         *
         * Sets the background color of the console window. Note that this only applies to the text that has yet to
         * be printed out. This function returns a scope exit function that, when destroyed, reverts the background
         * color back to what it was before. For example:
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * // Background color is currently black
         * {
         *     auto printRed = console::set_background(console_color::red);
         *     // Background color is now red
         * }
         * // Background color is black again
         * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         * // Background color is currently gray
         * {
         *     auto printRed = console::set_background(console_color::red);
         *     // Background color is now red
         *     printRed.cancel();
         * }
         * // Background color is still red
         */
        static auto set_background(console_color color)
        {
            auto handle = ::GetStdHandle(static_cast<DWORD>(console_device::output));
            auto info = details::console_info(handle);
            auto result = make_scope_guard([attr = info.wAttributes, handle]() noexcept
            {
                ::SetConsoleTextAttribute(handle, attr);
            });

            info.wAttributes = clear_flag(info.wAttributes, 0x00F0);
            info.wAttributes = set_flag(info.wAttributes, static_cast<uint8_t>(color) << 4);
            if (!::SetConsoleTextAttribute(handle, info.wAttributes))
            {
                result.cancel();
                throw std::system_error(::GetLastError(), std::system_category());
            }

            return result;
        }

        /*
         * console::set_colors
         *
         * Sets both the foreground text color as well as the background color of the console. 
         */
        inline auto set_colors(console_color foregroundColor, console_color backgroundColor)
        {
            auto handle = ::GetStdHandle(static_cast<DWORD>(console_device::output));
            auto info = details::console_info(handle);
            auto result = make_scope_guard([attr = info.wAttributes, handle]() noexcept
            {
                ::SetConsoleTextAttribute(handle, attr);
            });

            info.wAttributes = clear_flag(info.wAttributes, 0x00FF);
            info.wAttributes = set_flag(info.wAttributes, static_cast<uint8_t>(foregroundColor));
            info.wAttributes = set_flag(info.wAttributes, static_cast<uint8_t>(backgroundColor) << 4);
            if (!::SetConsoleTextAttribute(handle, info.wAttributes))
            {
                result.cancel();
                throw std::system_error(::GetLastError(), std::system_category());
            }

            return result;
        }
    };
}

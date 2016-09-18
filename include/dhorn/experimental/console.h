/*
 * Duncan Horn
 *
 * console.h
 *
 * Utility functions for dealing with console output. The primary 
 */
#pragma once

#include <cstdint>
#include <iostream>

#include "bitmask.h"
#include "math.h"
#include "scope_exit.h"
#include "windows_exception.h"

namespace dhorn
{
    namespace experimental
    {
        /*
         * console_device
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
         * Represents a foreground or background color within the console
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
                    throw_last_error();
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
         * Console Attributes
         */
#pragma region Console Size

        inline size<int16_t> console_size(console_device device = console_device::output)
        {
            auto info = details::console_info(device);
            return{ info.dwSize.X, info.dwSize.Y };
        }

        inline point<int16_t> console_cursor_position(console_device device = console_device::output)
        {
            auto info = details::console_info(device);
            return{ info.dwCursorPosition.X, info.dwCursorPosition.Y };
        }

        inline console_color console_foreground_color(console_device device = console_device::output)
        {
            auto info = details::console_info(device);
            return static_cast<console_color>(info.wAttributes & 0x0F);
        }

        inline console_color console_background_color(console_device device = console_device::output)
        {
            auto info = details::console_info(device);
            return static_cast<console_color>((info.wAttributes >> 4) & 0x0F);
        }

#pragma endregion



        /*
         * Console Text Modifications
         */
#pragma region Console Text Modifications

        inline auto change_console_foreground(console_color color, console_device device = console_device::output)
        {
            auto handle = ::GetStdHandle(static_cast<DWORD>(device));
            auto info = details::console_info(handle);
            auto result = make_scope_exit([attr = info.wAttributes, handle]() noexcept
            {
                ::SetConsoleTextAttribute(handle, attr);
            });

            info.wAttributes = clear_flag(info.wAttributes, 0x0F);
            info.wAttributes = set_flag(info.wAttributes, static_cast<uint8_t>(color));
            if (!::SetConsoleTextAttribute(handle, info.wAttributes))
            {
                result.cancel();
                throw_last_error();
            }

            return result;
        }

        inline auto change_console_background(console_color color, console_device device = console_device::output)
        {
            auto handle = ::GetStdHandle(static_cast<DWORD>(device));
            auto info = details::console_info(handle);
            auto result = make_scope_exit([attr = info.wAttributes, handle]() noexcept
            {
                ::SetConsoleTextAttribute(handle, attr);
            });

            info.wAttributes = clear_flag(info.wAttributes, 0x00F0);
            info.wAttributes = set_flag(info.wAttributes, static_cast<uint8_t>(color) << 4);
            if (!::SetConsoleTextAttribute(handle, info.wAttributes))
            {
                result.cancel();
                throw_last_error();
            }

            return result;
        }

        inline auto change_console_colors(
            console_color foregroundColor,
            console_color backgroundColor,
            console_device device = console_device::output)
        {
            auto handle = ::GetStdHandle(static_cast<DWORD>(device));
            auto info = details::console_info(handle);
            auto result = make_scope_exit([attr = info.wAttributes, handle]() noexcept
            {
                ::SetConsoleTextAttribute(handle, attr);
            });

            info.wAttributes = clear_flag(info.wAttributes, 0x00FF);
            info.wAttributes = set_flag(info.wAttributes, static_cast<uint8_t>(foregroundColor));
            info.wAttributes = set_flag(info.wAttributes, static_cast<uint8_t>(backgroundColor) << 4);
            if (!::SetConsoleTextAttribute(handle, info.wAttributes))
            {
                result.cancel();
                throw_last_error();
            }

            return result;
        }

#pragma endregion
    }
}

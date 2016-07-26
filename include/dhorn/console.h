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
    /*
     * console_device
     */
    enum class console_device : uint32_t
    {
        input   = STD_INPUT_HANDLE,
        output  = STD_OUTPUT_HANDLE,
        error   = STD_ERROR_HANDLE,
    };



    /*
     * console_color
     *
     * Represents a foreground or background color within the console
     */
    enum class console_color : uint8_t
    {
        red     = 0x04,
        green   = 0x02,
        blue    = 0x01,
        magenta = (red | blue),
        cyan    = (green | blue),
        yellow  = (red | green),

        white   = (red | green | blue),
        black   = 0x00,
    };



    /*
     * console_color_intensity
     *
     * Represents the intensity of a foreground or background color used within the console
     */
    enum class console_color_intensity : uint8_t
    {
        not_intense = 0x00,
        intense     = 0x08,
    };



#pragma region Helpers

    namespace details
    {
        inline CONSOLE_SCREEN_BUFFER_INFO console_info(_In_ HANDLE device)
        {
            CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
            if (!::GetConsoleScreenBufferInfo(device, &bufferInfo))
            {
                throw_last_error();
            }

            return bufferInfo;
        }

        inline CONSOLE_SCREEN_BUFFER_INFO console_info(_In_ console_device device)
        {
            return console_info(::GetStdHandle(static_cast<DWORD>(device)));
        }
    }

#pragma endregion



    /*
     * Console Attributes
     */
#pragma region Console Size

    inline size<int16_t> console_size(_In_ console_device device)
    {
        auto info = details::console_info(device);
        return { info.dwSize.X, info.dwSize.Y };
    }

    inline point<int16_t> console_cursor_position(_In_ console_device device)
    {
        auto info = details::console_info(device);
        return { info.dwCursorPosition.X, info.dwCursorPosition.Y };
    }

    inline console_color console_foreground_color(_In_ console_device device)
    {
        auto info = details::console_info(device);
        return static_cast<console_color>(info.wAttributes & 0x07);
    }

    inline console_color console_foreground_intensity(_In_ console_device device)
    {
        auto info = details::console_info(device);
        return static_cast<console_color>(info.wAttributes & 0x08);
    }

    inline console_color console_background_color(_In_ console_device device)
    {
        auto info = details::console_info(device);
        return static_cast<console_color>((info.wAttributes >> 4) & 0x07);
    }

    inline console_color console_background_intensity(_In_ console_device device)
    {
        auto info = details::console_info(device);
        return static_cast<console_color>((info.wAttributes >> 4) & 0x08);
    }

#pragma endregion



    /*
     * Console Text Modifications
     */
#pragma region Console Text Modifications

    inline auto change_console_foreground(
        _In_ console_device device,
        _In_ console_color color,
        _In_ console_color_intensity intensity = console_color_intensity::intense)
    {
        auto handle = ::GetStdHandle(static_cast<DWORD>(device));
        auto info = details::console_info(handle);
        auto result = make_scope_exit([attr = info.wAttributes, handle]() noexcept
        {
            ::SetConsoleTextAttribute(handle, attr);
        });

        auto flags = static_cast<uint8_t>(color) | static_cast<uint8_t>(intensity);
        info.wAttributes = clear_flag(info.wAttributes, 0x0F);
        info.wAttributes = set_flag(info.wAttributes, flags);
        if (!::SetConsoleTextAttribute(handle, info.wAttributes))
        {
            result.cancel();
            throw_last_error();
        }

        return result;
    }

    inline auto change_console_background(
        _In_ console_device device,
        _In_ console_color color,
        _In_ console_color_intensity intensity = console_color_intensity::intense)
    {
        auto handle = ::GetStdHandle(static_cast<DWORD>(device));
        auto info = details::console_info(handle);
        auto result = make_scope_exit([attr = info.wAttributes, handle]() noexcept
        {
            ::SetConsoleTextAttribute(handle, attr);
        });

        auto flags = static_cast<uint8_t>(color) | static_cast<uint8_t>(intensity);
        info.wAttributes = clear_flag(info.wAttributes, 0x00F0);
        info.wAttributes = set_flag(info.wAttributes, flags << 4);
        if (!::SetConsoleTextAttribute(handle, info.wAttributes))
        {
            result.cancel();
            throw_last_error();
        }

        return result;
    }

    inline auto change_console_colors(
        _In_ console_device device,
        _In_ console_color foregroundColor,
        _In_ console_color backgroundColor,
        _In_ console_color_intensity foregroundIntensity = console_color_intensity::intense,
        _In_ console_color_intensity backgroundIntensity = console_color_intensity::intense)
    {
        auto handle = ::GetStdHandle(static_cast<DWORD>(device));
        auto info = details::console_info(handle);
        auto result = make_scope_exit([attr = info.wAttributes, handle]() noexcept
        {
            ::SetConsoleTextAttribute(handle, attr);
        });

        auto foregroundFlags = static_cast<uint8_t>(foregroundColor) | static_cast<uint8_t>(foregroundIntensity);
        auto backgroundFlags = static_cast<uint8_t>(backgroundColor) | static_cast<uint8_t>(backgroundIntensity);

        info.wAttributes = clear_flag(info.wAttributes, 0x00FF);
        info.wAttributes = set_flag(info.wAttributes, foregroundFlags);
        info.wAttributes = set_flag(info.wAttributes, backgroundFlags << 4);
        if (!::SetConsoleTextAttribute(handle, info.wAttributes))
        {
            result.cancel();
            throw_last_error();
        }

        return result;
    }

#pragma endregion
}

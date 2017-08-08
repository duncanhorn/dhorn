/*
 * Duncan Horn
 *
 * file_time_clock.h
 *
 * Even though there's a good chance that on Windows - at least with the Visual C++ STL implementation - that
 * std::chrono::system_clock will be a wrapper around GetSystemTimeAsFileTime, (1) it's not guaranteed, and (2) it would
 * appear as though the epoch for system_clock::time_point is NOT the same as GetSystemTimeAsFileTime; it appears to be
 * relative to the Linux epoch.
 *
 * This file_time_clock implementation is an alternative that is guaranteed to return GetSystemTimeAsFileTime when `now`
 * is invoked.
 */
#pragma once

#include <chrono>
#include <windows.h>

namespace dhorn::windows
{
    /*
     * FILETIME <--> std::uint64_t conversion helpers
     */
    inline constexpr std::uint64_t filetime_to_uint(FILETIME value)
    {
        return (static_cast<std::uint64_t>(value.dwHighDateTime) << 32) | value.dwLowDateTime;
    }

    inline constexpr FILETIME uint_to_filetime(std::uint64_t value)
    {
        return
        {
            static_cast<DWORD>(value & 0x00000000FFFFFFFF),
            static_cast<DWORD>((value >> 32) & 0x00000000FFFFFFFF)
        };
    }



    /*
     * file_time_clock
     */
    struct file_time_clock
    {
        using rep = std::uint64_t;
        using period = std::ratio_multiply<std::ratio<100, 1>, std::nano>;

        using duration = std::chrono::duration<rep, period>;
        using time_point = std::chrono::time_point<file_time_clock>;

        static constexpr bool is_steady = false;

        static time_point now(void) noexcept
        {
            FILETIME time;
            ::GetSystemTimePreciseAsFileTime(&time);
            return time_point(duration(filetime_to_uint(time)));
        }
    };
}

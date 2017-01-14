/*
 * Duncan Horn
 *
 * tick_count_clock.h
 *
 * 
 */
#pragma once

#include <chrono>
#include <windows.h>

namespace dhorn::windows
{
    struct tick_count_clock
    {
        using rep = uint64_t;
        using period = std::milli;

        using duration = std::chrono::duration<rep, period>;
        using time_point = std::chrono::time_point<tick_count_clock>;

        static constexpr bool is_steady = false;

        static time_point now() noexcept
        {
            return time_point(duration(::GetTickCount64()));
        }
    };
}

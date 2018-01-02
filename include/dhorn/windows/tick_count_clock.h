/*
 * Duncan Horn
 *
 * tick_count_clock.h
 *
 * A clock that wraps the GetTickCount64 function. This clock is useful when an efficient steady clock is desired and
 * lower resolution is acceptable.
 */
#pragma once

#include <chrono>
#include <windows.h>

namespace dhorn::windows
{
    /*
     * tick_count_clock
     */
    struct tick_count_clock
    {
        using rep = std::uint64_t;
        using period = std::milli;

        using duration = std::chrono::duration<rep, period>;
        using time_point = std::chrono::time_point<tick_count_clock>;

        static constexpr bool is_steady = true;

        static time_point now(void) noexcept
        {
            return time_point(duration(::GetTickCount64()));
        }
    };
}

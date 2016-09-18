/*
 * Duncan Horn
 *
 * string.h
 *
 * Helpers for dealing with strings
 */
#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace dhorn
{
    /*
     * trim
     */
#pragma region trim

    template <typename CharT, typename Traits, typename Alloc>
    inline void trim(std::basic_string<CharT, Traits, Alloc> &str)
    {
        // It's more efficient to remove from the end of the string first so that we have fewer characters to shift
        // when removing from the front
        str.erase(std::find_if(str.rbegin(), str.rend(), [&](CharT ch) -> bool
        {
            return !isspace(static_cast<int>(ch));
        }).base(), std::end(str));

        str.erase(std::begin(str), std::find_if(std::begin(str), std::end(str), [&](CharT ch) -> bool
        {
            return !isspace(static_cast<int>(ch));
        }));
    }

    template <typename CharT, typename Traits, typename Alloc, typename Itr>
    inline void trim(std::basic_string<CharT, Traits, Alloc> &str, Itr begin, Itr end)
    {
        // It's more efficient to remove from the end of the string first so that we have fewer characters to shift
        // when removing from the front
        str.erase(std::find_if(str.rbegin(), str.rend(), [&](CharT ch) -> bool
        {
            return !std::any_of(begin, end, [&](CharT value) -> bool { return ch == value; });
        }).base(), std::end(str));

        str.erase(std::begin(str), std::find_if(std::begin(str), std::end(str), [&](CharT ch) -> bool
        {
            return !std::any_of(begin, end, [&](CharT value) -> bool { return ch == value; });
        }));
    }

#pragma endregion



    /*
     * split
     */
#pragma region split

    template <typename CharT, typename Traits, typename Alloc, typename Itr>
    inline std::vector<std::basic_string<CharT, Traits, Alloc>> split(
        const std::basic_string<CharT, Traits, Alloc> &str,
        Itr begin,
        Itr end)
    {
        using string_type = std::basic_string<CharT, Traits, Alloc>;
        std::vector<string_type> result;

        auto endItr = std::end(str);
        for (auto itr = std::begin(str); true; ++itr)
        {
            auto seqEnd = std::find_if(itr, endItr, [&](CharT ch) -> bool
            {
                return std::any_of(begin, end, [&](CharT value) -> bool { return ch == value; });
            });
            result.push_back(string_type(itr, seqEnd));
            itr = seqEnd;

            if (itr == endItr)
            {
                break;
            }
        }

        return result;
    }

    template <typename CharT, typename Traits, typename Alloc>
    inline std::vector<std::basic_string<CharT, Traits, Alloc>> split(
        const std::basic_string<CharT, Traits, Alloc> &str,
        CharT ch)
    {
        const CharT arr[] = { ch };
        return split(str, std::begin(arr), std::end(arr));
    }

#pragma endregion
}

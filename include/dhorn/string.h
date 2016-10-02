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
     *
     * Removes all characters from the start and end of a string that match a particular criteria. There are three
     * overloads for the trim function: one that takes a function object whose signature should be `bool(CharT)`,
     * another that takes no extra arguments and calls the former overload with the `isspace` function, and a third that
     * takes two iterators and calls the first with a function object that returns true if any value in the range is
     * equal to the input character.
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * auto str = "   foo bar  \r\n\t"s;
     *
     * // Overload 1
     * auto str1 = trim(str, [](char ch) { return ch == ' '; });
     *
     * // Overload 2
     * auto str2 = trim(str);
     *
     * // Overload 3
     * char arr[] = { ' ', '\r', '\t' };
     * auto str3 = trim(str, std::begin(arr), std::end(arr));
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * The above example results in the following:
     *      str1: "foo bar  \r\n\t"
     *      str2: "foo bar"
     *      str3: "foo bar  \r\n"
     */
#pragma region trim

    template <typename CharT, typename Traits, typename Alloc, typename Func>
    inline std::basic_string<CharT, Traits, Alloc> trim(
        const std::basic_string<CharT, Traits, Alloc> &str,
        const Func &fn)
    {
        auto back = std::find_if(str.rbegin(), str.rend(), fn).base();
        auto front = std::find_if(std::begin(str), back, fn);
        return { front, back };
    }

    template <typename CharT, typename Traits, typename Alloc>
    inline std::basic_string<CharT, Traits, Alloc> trim(const std::basic_string<CharT, Traits, Alloc> &str)
    {
        return trim(str, [&](CharT ch) -> bool
        {
            return !isspace(static_cast<int>(ch));
        });
    }

    template <typename CharT, typename Traits, typename Alloc, typename Itr>
    inline std::basic_string<CharT, Traits, Alloc> trim(
        const std::basic_string<CharT, Traits, Alloc> &str,
        Itr begin,
        Itr end)
    {
        return trim(str, [&](CharT ch) -> bool
        {
            return !std::any_of(begin, end, [&](CharT value) -> bool { return ch == value; });
        });
    }

#pragma endregion



    /*
     * split
     *
     * Splits the specified string at each character matching a specified criteria. There are three overloads for the
     * split function: one that takes a function object whose signature should be `bool(CharT)`, another that takes a
     * single character to split at, and a third that takes a begin/end iterator representing a range of characters to
     * split at.
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * auto str = "foo bar. foobar "s;
     *
     * // Overload 1
     * auto vec1 = split(str, [](char ch) { return ch == ' '; });
     *
     * // Overload 2
     * auto vec2 = split(str, ' ');
     *
     * // Overload 3
     * char arr[] = { ' ', '.' };
     * auto vec3 = split(str, std::begin(arr), std::end(arr));
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * The above example results in the following:
     *      vec1: { "foo", "bar.", "foobar", "" }
     *      vec2: { "foo", "bar.", "foobar", "" }
     *      vec3: { "foo", "bar", "", "foobar", "" }
     */
#pragma region split

    template <typename CharT, typename Traits, typename Alloc, typename Func>
    inline std::vector<std::basic_string<CharT, Traits, Alloc>> split(
        const std::basic_string<CharT, Traits, Alloc> &str,
        const Func &fn)
    {
        using string_type = std::basic_string<CharT, Traits, Alloc>;
        std::vector<string_type> result;

        auto endItr = std::end(str);
        for (auto itr = std::begin(str); true; ++itr)
        {
            auto seqEnd = std::find_if(itr, endItr, fn);
            result.emplace_back(itr, seqEnd);

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
        return split(str, [&](CharT other) -> bool
        {
            return other == ch;
        });
    }

    template <typename CharT, typename Traits, typename Alloc, typename Itr>
    inline std::vector<std::basic_string<CharT, Traits, Alloc>> split(
        const std::basic_string<CharT, Traits, Alloc> &str,
        Itr begin,
        Itr end)
    {
        return split(str, [&](CharT ch) -> bool
        {
            return std::any_of(begin, end, [&](CharT value) -> bool { return ch == value; });
        });
    }

#pragma endregion
}

/*
 * Duncan Horn
 *
 * algorithm.h
 *
 * A set of utility/helper functions that follow the same patterns as the STL algorithm header. Many of these are just
 * convenience wrappers around functions in the standard `algorithm` header.
 */
#pragma once

#include <algorithm>
#include <iterator>

namespace dhorn
{
    /*
     * split/split_if
     *
     * Used to split a range into a separate range whose elements are constructed using iterator pairs from all
     * sub-ranges in the input range that exclude the specified value/condition. The most common scenario is to split
     * strings into vectors of strings at specific character(s). For example, the following will split at spaces:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *  auto str = "foo bar  foobar "s;
     *  std::vector<std::string> words;
     *  split(begin(str), end(str), ' ', std::back_inserter(words));
     *  // words = { "foo", "bar", "", "foobar", "" }
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * Values are written to the output range via assignment with an initializer_list. I.e. via `*itr = { begin, end }`.
     */
#pragma region split

    template <typename ForwardItr, typename OutputItr, typename Ty>
    OutputItr split(ForwardItr front, ForwardItr back, OutputItr output, const Ty& value)
    {
        while (true)
        {
            auto pos = std::find(front, back, value);
            *output = { front, pos };
            ++output;

            if (pos == back)
            {
                break;
            }

            ++pos;
            front = pos;
        }

        return output;
    }

    template <typename ForwardItr, typename OutputItr, typename UnaryPredicate>
    OutputItr split_if(ForwardItr front, ForwardItr back, OutputItr output, UnaryPredicate&& cond)
    {
        while (true)
        {
            auto pos = std::find_if(front, back, cond);
            *output = { front, pos };
            ++output;

            if (pos == back)
            {
                break;
            }

            ++pos;
            front = pos;
        }

        return output;
    }

#pragma endregion
}

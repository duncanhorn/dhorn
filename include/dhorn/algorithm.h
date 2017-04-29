/*
 * Duncan Horn
 *
 * algorithm.h
 *
 * 
 */
#pragma once

#include <algorithm>
#include <iterator>

namespace dhorn
{
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

/*
 * Duncan Horn
 *
 * linear_animation.h
 *
 * Represents a linear animation. That is, an animation whose key frames are interpolated linearly. That is, for a set
 * of key frames { a_0, a_1, a_2, ..., a_N } ordered by time value, and a time value t:
 *
 *      1.  The animation has no effect for t < a_0.time
 *      2.  The animation applies a linearly interpolated value between a_x.value and a_y.value where a_x.time <= t and
 *          a_y.time > t
 *      3.  Once t >= x_N, the animation applies the value x_N.value and returns animation_state::completed
 */
#pragma once

#include <map>

#include "animation.h"

namespace dhorn
{
    template <typename Ty>
    class linear_animation :
        public garbage::key_frame_animation<Ty>
    {
        using MyBase = garbage::key_frame_animation<Ty>;

    public:

        /*
         * Overloaded animation functions
         */
        virtual animation_state on_update(_In_ duration elapsedTime)
        {
            auto state = MyBase::on_update(elapsedTime);

            // Ignore any updates until the animation has started
            if (this->begun())
            {
                // TODO
            }

            return state;
        }
    };
}

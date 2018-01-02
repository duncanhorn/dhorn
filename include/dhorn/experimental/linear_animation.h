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

#include <cassert>

#include "animation.h"

namespace dhorn
{
    namespace experimental
    {
        template <typename Ty>
        class linear_animation :
            public details::key_frame_animation<Ty>
        {
            using MyBase = details::key_frame_animation<Ty>;

        public:
            /*
             * Constructor(s)/Destructor
             */
            linear_animation(void) :
                _left(this->next()),
                _right(this->next()),
                _slope{},
                _offset{}
            {
            }

            template <typename Func>
            linear_animation(Func &&func) :
                MyBase(std::forward<Func>(func)),
                _left(this->next()),
                _right(this->next()),
                _slope{},
                _offset{}
            {
            }



            /*
             * Overloaded animation functions
             */
            virtual animation_state on_update(typename MyBase::duration elapsedTime)
            {
                auto state = MyBase::on_update(elapsedTime);

                // Ignore any updates until the animation has started
                if (this->begun())
                {
                    auto next = this->next();
                    auto prev = next;
                    --prev;
                    if (this->completed())
                    {
                        // Once completed, we force the value to the final value
                        assert(state == animation_state::completed);
                        this->update(prev->second);
                    }
                    else
                    {
                        if ((this->_left != prev) || (this->_right != next))
                        {
                            this->_left = prev;
                            this->_right = next;
                            this->update_params();
                        }

                        auto val = this->_slope * seconds_fraction(this->elapsed_time()) + this->_offset;
                        this->update(static_cast<Ty>(val));
                    }
                }

                return state;
            }



        private:

            void update_params(void)
            {
                auto t_0 = seconds_fraction(this->_left->first);
                auto t_1 = seconds_fraction(this->_right->first);
                auto &v_0 = this->_left->second;
                auto &v_1 = this->_right->second;

                this->_slope = static_cast<Ty>((v_1 - v_0) / (t_1 - t_0));
                this->_offset = static_cast<Ty>(v_0 - this->_slope * t_0);
            }

            inline constexpr double seconds_fraction(typename MyBase::duration duration) const
            {
                return static_cast<double>(duration.count() * MyBase::duration::period::num) /
                    MyBase::duration::period::den;
            }

            typename MyBase::iterator_type _left;
            typename MyBase::iterator_type _right;
            Ty _slope;
            Ty _offset;
        };
    }
}

/*
 * Duncan Horn
 *
 * discrete_animation.h
 *
 * 
 */
#pragma once

#include "animation.h"

namespace dhorn
{
    /*
     * discrete_animation
     */
    template <typename Ty>
    class discrete_animation final :
        public garbage::key_frame_animation<Ty>
    {
        using MyBase = garbage::key_frame_animation<Ty>;

    public:
        /*
         * Constructor(s)/Destructor
         */
        discrete_animation(void) :
            _prev(this->next())
        {
        }



        /*
         * Overloaded animation functions
         */
        virtual animation_state on_update(_In_ duration elapsedTime)
        {
            auto state = MyBase::on_update(elapsedTime);

            // Ignore any updates until the animation has started
            if (this->begun())
            {
                // Only update if our next iterator changes
                auto next = this->next();
                if (next != this->_prev)
                {
                    this->_prev = next;

                    // (next - 1) guaranteed to be a valid iterator
                    --next;
                    this->update(next->second);
                }
            }

            return state;
        }



    private:

        iterator_type _prev;
    };
}

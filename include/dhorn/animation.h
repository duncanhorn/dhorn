/*
 * Duncan Horn
 *
 * animation.h
 *
 * The "base" class for an animation, along with all relevant types and structures required for supporting an animation
 * instance. Unlike what you might think, clients generally should not be dealing with animation instances directly,
 * unless the animation type is a custom type and a special effect is desired that could not otherwise be accomplished
 * without direct access to the object. Instead, clients should generally use the animation_handle returned by
 * animation_manager.
 */
#pragma once

#include <chrono>
#include <memory>

namespace dhorn
{
    enum class animation_state
    {
        pending   = 1,
        running   = 2,
        paused    = 3,
        completed = 4,
    };



    /*
     * animation Base Class
     */
    class animation
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        virtual ~animation(void)
        {
        }



        /*
         * Public virtual Function(s)
         */
        virtual void on_begin(void)
        {
        }

        virtual void on_pause(void)
        {
        }

        virtual void on_resume(void)
        {
        }

        virtual void on_completed(void)
        {
        }

        virtual void on_cancelled(void)
        {
        }

        virtual animation_state on_update(_In_ std::chrono::high_resolution_clock::duration elapsedTime)
        {
            // By default, immediately transition to the completed state
            return animation_state::completed;
        }

        virtual void on_state_change(_In_ animation_state /*newState*/)
        {
        }
    };
}

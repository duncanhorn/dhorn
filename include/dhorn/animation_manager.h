/*
 * Duncan Horn
 *
 * animation_manager.h
 *
 * Manages currently active animations at a level of abstraction above that of the UI framework being used. The
 * animation_manager has the concept of four different stages for animations: pending, running, paused, and completed.
 * The pending state is for animations that are queued, but not yet processed. Note that pausing an animation in this
 * state will put the animation into to the 'paused' state, and will re-enter the 'pending' state when resumed. The
 * running state indicates that the animation's on_update function will get called once for every call to update on
 * animation_manager. The paused state indicates that the animation has started (i.e. on_begin has been called), but
 * the client has expressed a desire to temporarily disable further updates to the animation. Attempting to pause an
 * already paused animation will no-op, and in similar fashion, attempting to resume a running animation will no-op as
 * well. The completed state indicates that the animation has finished (i.e. on_update has returned the value
 * animation_state::completed). Once an animation reaches the completed state, it remains there until all references to
 * the associated animation_handle have been released, at which point the animation instance will be destroyed.
 *
 * The general "flow" of events is:
 *
 *      1. The client creates an animation instance and transfers ownership over to the animation_manager instance.
 */
#pragma once

#include <chrono>
#include <map>

#include "animation.h"

namespace dhorn
{
    // Forward declarations
    class animation_manager;



    /*
     * Types
     */
    using animation_cookie = size_t;



    namespace garbage
    {
        /*
         * animation_state_data
         */
        struct animation_state_data final
        {
            animation_state state;

            // Constructor
            animation_state_data() :
                state(animation_state::pending)
            {
            }
        };
    }



    /*
     * animation_handle
     */
    class animation_handle final
    {
    public:



    private:

        animation_manager *_manager;
    };



    /*
     * animation_manager
     */
    class animation_manager final
    {
    public:
        /*
         * Constructor(s)/Destructor
         */



    private:

        struct animation_info
        {

        };

        std::map<animation_cookie, animation_info> _animationInfo;
    };
}

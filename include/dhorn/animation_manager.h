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
 *      1.  The client creates an animation instance and transfers ownership over to the animation_manager instance.
 *          In return, the animation_manager returns an animation_handle instance. The client can use this handle to
 *          track the progress of the animation (among other things described below). Note that animation_manager deals
 *          with animation instances using std::shared_ptr, so ownership can technically be shared if a smart_ptr is
 *          supplied.
 *      2.  The client calls animation_manager::update which will put the animation into the running state and call
 *          the animation's on_begin function (assuming the animation has not been paused)
 *      3.  Once the animation finishes (returns animation_state::completed), animation_manager will transfer the
 *          animation instance to its completed collection and call the animation's on_completed function. The
 *          animation will remain in this state until all references to the corresponding animation_handle have been
 *          released. Note that if the animation_handle loses all of its references prior to entering the completed
 *          state, the animation won't get destroyed until after the animation completes, at which time it will get
 *          destroyed immediately.
 *
 * Note that if the client wishes to pause, cancel, or resume an animation, he/she can do so by calling the
 * corresponding function on animation_manager with the corresponding animation_handle. Also note that the 'cancelled'
 * state is the treated same as the 'completed' state and obeys the same animation lifetime management.
 */
#pragma once

#include <cassert>
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



    /*
     * animation_handle
     */
    class animation_handle final
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        animation_handle(_In_ animation_manager *owner, _In_ animation_cookie cookie) :
            _owner(owner),
            _cookie(cookie)
        {
        }

        ~animation_handle(void)
        {
            // TODO: Notify of destruction
        }

        // Cannot copy (otherwise we screw up the reference count)
        animation_handle(_In_ const animation_handle &) = delete;
        animation_handle &operator=(_In_ const animation_handle &) = delete;



        /*
         * Public Functions
         */
        animation_cookie id(void) const
        {
            return this->_cookie;
        }



    private:

        animation_manager *_owner;
        animation_cookie _cookie;
    };



    /*
     * animation_manager
     */
    class animation_manager final
    {
        /*
         * Private types
         */
        using clock = std::chrono::high_resolution_clock;
        using time_point = clock::time_point;



        /*
         * Internal animation_info state object
         */
        struct animation_info
        {
            std::shared_ptr<animation> instance;
            animation_state state;
            time_point prev_time;

            // Constructor
            animation_info(
                _In_ std::shared_ptr<animation> animation,
                _In_ animation_manager *owner,
                _In_ animation_cookie cookie) :
                instance(std::move(animation)),
                state(animation_state::running),
                prev_time(clock::now())
            {
                instance->on_begin();
            }

            void update_state(_In_ animation_state state)
            {
                if (state != this->state)
                {
                    this->state = state;
                    instance->on_state_change(this->state);

                    switch (state)
                    {
                    case animation_state::running:
                        // We call on_begin on construction, so this must be from a resume
                        this->instance->on_resume();
                        break;

                    case animation_state::paused:
                        this->instance->on_pause();
                        break;

                    case animation_state::canceled:
                        this->instance->on_canceled();

                        // vvv fall through vvv
                    case animation_state::completed:
                        this->instance->on_completed();
                        break;
                    }
                }
            }
        };
        using AnimationMap = std::map<animation_cookie, animation_info>;



    public:
        /*
         * Public types
         */
        using duration = clock::duration;



        /*
         * Constructor(s)/Destructor
         */
        animation_manager(void) :
            _nextCookie(1)
        {
        }



        /*
         * Client functions
         */
        void update(void)
        {
            auto now = clock::now();

            for (auto &pair : this->_animationInfo)
            {
                auto &info = pair.second;

                if (garbage::is_running(info.state))
                {
                    auto elapsedTime = now - info.prev_time;
                    info.prev_time = now;

                    info.update_state(info.instance->on_update(elapsedTime));
                }
            }
        }

        std::shared_ptr<animation_handle> submit(_In_ animation *instance)
        {
            this->submit(std::make_shared<animation>(instance));
        }

        std::shared_ptr<animation_handle> submit(_In_ std::shared_ptr<animation> instance)
        {
            auto pair = this->_animationInfo.emplace(std::move(instance), this, NextCookie());
            return std::make_shared<animation_handle>(this, pair.first->first);
        }

        bool pause(_In_ animation_handle *handle)
        {
            auto itr = this->FindInfo(handle->id());
            auto &info = itr->second;
            if (!garbage::is_running(info.state))
            {
                // Cannot be paused if not running
                return false;
            }

            info.update_state(animation_state::paused);
            return true;
        }

        bool resume(_In_ animation_handle *handle)
        {
            auto itr = this->FindInfo(handle->id());
            auto &info = itr->second;
            if (!garbage::is_paused(info.state))
            {
                // Can't resume if not paused
                return false;
            }

            info.update_state(animation_state::running);
            return true;
        }

        bool cancel(_In_ animation_handle *handle)
        {
            auto itr = this->FindInfo(handle->id());
            auto &info = itr->second;
            if (garbage::is_complete(info.state))
            {
                // Can't cancel if already complete
                return false;
            }

            info.update_state(animation_state::canceled);
            return true;
        }

        animation_state query_state(_In_ animation_handle *handle) const
        {
            return this->FindInfo(handle->id())->second.state;
        }



    private:

        animation_cookie NextCookie(void)
        {
            while (this->_animationInfo.find(this->_nextCookie) != std::end(this->_animationInfo))
            {
                ++this->_nextCookie;
            }

            return this->_nextCookie;
        }

        AnimationMap::iterator FindInfo(_In_ animation_cookie cookie)
        {
            auto itr = this->_animationInfo.find(cookie);
            if (itr == std::end(this->_animationInfo))
            {
                throw std::out_of_range("animation not found");
            }

            return itr;
        }

        AnimationMap::const_iterator FindInfo(_In_ animation_cookie cookie) const
        {
            auto itr = this->_animationInfo.find(cookie);
            if (itr == std::end(this->_animationInfo))
            {
                throw std::out_of_range("animation not found");
            }

            return itr;
        }

        // Animation state
        AnimationMap _animationInfo;
        animation_cookie _nextCookie;
    };
}

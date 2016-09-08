/*
 * Duncan Horn
 *
 * animation_manager.h
 *
 * Manages currently active animations at a level of abstraction above that of the UI framework being used. The
 * animation_manager has the concept of four different stages for animations: pending, running, paused, and completed.
 * The pending state is for animations that have been created, but have not yet been submitted to an animation_manager
 * instance. Thus, it is not possible for an animation to re-enter the pending state, nor is it possible for an
 * animation to transition from the pending state to any state other than running. The running state indicates that the
 * animation's on_update function will get called once for every call to update on animation_manager. The paused state
 * indicates that the animation has started, but the client has expressed a desire to temporarily disable further
 * updates to the animation. Attempting to pause an already paused animation will no-op, and in similar fashion,
 * attempting to resume a running animation will no-op as well. The completed state indicates that the animation has
 * finished (i.e. on_update has returned the value animation_state::completed). Once an animation reaches the completed
 * state, it remains there until all references to the associated animation_handle have been released, at which point
 * the animation instance will be destroyed. There is a fifth state - canceled - which will remove the animation from
 * the list of currently running animations, though this state is just a transient state and will immediately
 * transition to the completed state.
 *
 * The general "flow" of events is:
 *
 *      1.  The client creates an animation instance and transfers ownership over to the animation_manager instance.
 *          In return, the animation_manager returns an animation_handle instance. The client can use this handle to
 *          track the progress of the animation (among other things described below). Note that animation_manager deals
 *          with animation instances using std::shared_ptr, so ownership can technically be shared if a smart_ptr is
 *          supplied.
 *      2.  On submission, the animation_manager will transition the animation to the running state. Note that there is
 *          no call to on_update at this time (since the animation has made no progress).
 *      3.  The client calls animation_manager::update for every frame, and as long as the animation has not been
 *          paused, canceled, or completed, animation_manager will call the animation's on_update function
 *      4.  Once the animation finishes (returns animation_state::completed), animation_manager will transfer the
 *          animation instance to its completed collection. The animation will remain in this state until all
 *          references to the corresponding animation_handle have been released. Note that if the animation_handle
 *          loses all of its references prior to entering the completed state, the animation won't get destroyed until
 *          after the animation completes, at which time it will get destroyed immediately.
 *
 *  *** NOTE: Since animation_manager deals with std::shared_ptr instances, the term "destroyed" for each animation ***
 *  ***       instance is meant to indicate that the reference to the object is lost. I.e. if ownership is          ***
 *  ***       completely transferred to animation_manager, then the reference count will drop to zero and the       ***
 *  ***       animation will get destroyed. However, if the client retains a reference to the animation object,     ***
 *  ***       then the object will stay alive, but animation_manager will have no knowledge of the existence of the ***
 *  ***       animation.                                                                                            ***
 *
 * Note that if the client wishes to pause, cancel, or resume an animation, he/she can do so by calling the
 * corresponding function on animation_manager with the corresponding animation_handle.
 */
#pragma once

#include <cassert>
#include <map>

#include "animation.h"
#include "functional.h"

namespace dhorn
{
    // Forward declarations
    class animation_manager;



    /*
     * Types
     */
    using animation_cookie = size_t;
    static const animation_cookie invalid_animation_cookie = 0;



    /*
     * animation_handle
     */
    class animation_handle final
    {
        friend class animation_manager;

        using DestroyCallback = std::function<void(animation_cookie)>;

    public:
        /*
         * Constructor(s)/Destructor
         */
        animation_handle(animation_cookie cookie, DestroyCallback callback) :
            _cookie(cookie),
            _callback(std::move(callback))
        {
        }

        ~animation_handle(void)
        {
            this->_callback(this->_cookie);
        }

        // Cannot copy (otherwise we screw up the reference count)
        animation_handle(const animation_handle &) = delete;
        animation_handle &operator=(const animation_handle &) = delete;



        /*
         * Public Functions
         */
        animation_cookie id(void) const
        {
            return this->_cookie;
        }



    private:

        void NotifyAnimationManagerDestroyed()
        {
            // There is nothing to notify when we are destroyed anymore, so just nop
            this->_callback = [](animation_cookie) {};
        }

        animation_cookie _cookie;
        DestroyCallback _callback;
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
            std::function<void(void)> notify_destroyed;
            animation_state state;
            time_point prev_time;
            bool has_references;

            // Constructor
            animation_info(std::shared_ptr<animation> animation, std::function<void(void)> notifyDestroyed) :
                instance(std::move(animation)),
                notify_destroyed(std::move(notifyDestroyed)),
                state(animation_state::running),
                prev_time(clock::now()),
                has_references(true)
            {
                instance->on_state_change(animation_state::running);
            }

            void update_state(animation_state newState)
            {
                if (newState != this->state)
                {
                    this->state = newState;
                    instance->on_state_change(this->state);
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

        ~animation_manager(void)
        {
            for (auto &pair : this->_animationInfo)
            {
                pair.second.notify_destroyed();
            }
        }



        /*
         * Client functions
         */
        void update(void)
        {
            auto now = clock::now();

            for (auto itr = std::begin(this->_animationInfo); itr != std::end(this->_animationInfo); )
            {
                // Force info to fall out of scope before calling TryRemove to help prevent logic errors
                {
                    auto &info = itr->second;
                    if (details::is_running(info.state))
                    {
                        auto elapsedTime = now - info.prev_time;
                        info.prev_time = now;

                        info.update_state(info.instance->on_update(elapsedTime));
                    }

                    // We are expected to transfer any canceled animation to the completed state prior to destroying
                    if (info.state == animation_state::canceled)
                    {
                        info.update_state(animation_state::completed);
                    }
                }

                // NOTE: TryRemove must be last call since the iterator (and its data) may become invalid
                auto prev = itr;
                ++itr;
                TryRemove(prev);
            }
        }

        std::shared_ptr<animation_handle> submit(animation *instance)
        {
            return this->submit(std::shared_ptr<animation>(instance));
        }

        std::shared_ptr<animation_handle> submit(std::shared_ptr<animation> instance)
        {
            auto cookie = this->NextCookie();
            auto result = std::make_shared<animation_handle>(
                cookie,
                bind_member_function(&animation_manager::AnimationHandleDestroyedCallback, this));

            auto pair = this->_animationInfo.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(cookie),
                std::forward_as_tuple(
                    std::move(instance),
                    std::bind(&animation_handle::NotifyAnimationManagerDestroyed, result.get())));
            assert(pair.second);

            return result;
        }

        bool pause(const animation_handle *handle)
        {
            auto itr = this->FindInfo(handle->id());
            auto &info = itr->second;
            if (!details::is_running(info.state))
            {
                // Cannot be paused if not running
                return false;
            }

            info.update_state(animation_state::paused);
            return true;
        }

        bool resume(const animation_handle *handle)
        {
            auto itr = this->FindInfo(handle->id());
            auto &info = itr->second;
            if (!details::is_paused(info.state))
            {
                // Can't resume if not paused
                return false;
            }

            info.update_state(animation_state::running);
            return true;
        }

        bool cancel(const animation_handle *handle)
        {
            auto itr = this->FindInfo(handle->id());
            auto &info = itr->second;
            if (details::is_complete(info.state))
            {
                // Can't cancel if already complete
                return false;
            }

            info.update_state(animation_state::canceled);
            return true;
        }

        animation_state query_state(const animation_handle *handle) const
        {
            return this->FindInfo(handle->id())->second.state;
        }



    private:

        animation_cookie NextCookie(void)
        {
            while ((this->_nextCookie == invalid_animation_cookie) ||
                   (this->_animationInfo.find(this->_nextCookie) != std::end(this->_animationInfo)))
            {
                ++this->_nextCookie;
            }

            return this->_nextCookie;
        }

        AnimationMap::iterator FindInfo(animation_cookie cookie)
        {
            auto itr = this->_animationInfo.find(cookie);
            if (itr == std::end(this->_animationInfo))
            {
                throw std::out_of_range("animation not found");
            }

            return itr;
        }

        AnimationMap::const_iterator FindInfo(animation_cookie cookie) const
        {
            auto itr = this->_animationInfo.find(cookie);
            if (itr == std::end(this->_animationInfo))
            {
                throw std::out_of_range("animation not found");
            }

            return itr;
        }

        void AnimationHandleDestroyedCallback(animation_cookie cookie)
        {
            // We don't call TryRemove here since it could be possible that the last reference to the animation_handle
            // was released off of the UI thread (which is not desired, but is a lot harder for clients to control). If
            // this is the case, and we do call TryRemove while the main update loop is running, things could get into
            // a pretty bad state very easily... Therefore, just let the update loop do the remove
            auto itr = this->FindInfo(cookie);
            auto &info = itr->second;
            info.notify_destroyed = []() {};
            info.has_references = false;
        }

        bool TryRemove(const AnimationMap::iterator &pos)
        {
            auto &info = pos->second;
            if (!info.has_references && details::is_complete(info.state))
            {
                this->_animationInfo.erase(pos);
                return true;
            }

            return false;
        }

        // Animation state
        AnimationMap _animationInfo;
        animation_cookie _nextCookie;
    };
}

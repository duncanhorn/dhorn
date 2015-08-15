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
#include <functional>
#include <map>
#include <memory>

#include "event_source.h"

namespace dhorn
{
    /*
     * animation_state
     */
    enum class animation_state
    {
        pending   = 0,
        running   = 1,
        paused    = 2,
        completed = 3,
        canceled  = 4,
    };



    /*
     * Common animation helpers
     */
    namespace garbage
    {
        // Helpers surrounding animation_state in case the enum ever expands. We have three conceptual states: running,
        // paused, and completed, though we have more than three "true" states
        inline constexpr bool is_complete(_In_ animation_state state)
        {
            return (state == animation_state::completed) || (state == animation_state::canceled);
        }

        inline constexpr bool is_running(_In_ animation_state state)
        {
            return state == animation_state::running;
        }

        inline constexpr bool is_paused(_In_ animation_state state)
        {
            return state == animation_state::paused;
        }
    }



    /*
     * animation Base Class
     */
    class animation
    {
        using EventSourceType = event_source<void(animation_state /*prev*/, animation_state /*new*/)>;
        using CallbackType = EventSourceType::callback_type;

    public:
        /*
         * Public types
         */
        using duration = std::chrono::high_resolution_clock::duration;



        /*
         * Constructor(s)/Destructor
         */
        animation(void) :
            _currentState(animation_state::pending)
        {
        }

        virtual ~animation(void)
        {
        }



        /*
         * Non-virtual public function(s)
         */
        event_cookie add_on_begin(_In_ CallbackType callback)
        {
            return this->_beginEventSource.add(std::move(callback));
        }

        void remove_on_begin(_In_ event_cookie cookie)
        {
            this->_beginEventSource.remove(cookie);
        }

        event_cookie add_on_pause(_In_ CallbackType callback)
        {
            return this->_pauseEventSource.add(std::move(callback));
        }

        void remove_on_pause(_In_ event_cookie cookie)
        {
            this->_pauseEventSource.remove(cookie);
        }

        event_cookie add_on_resume(_In_ CallbackType callback)
        {
            return this->_resumeEventSource.add(std::move(callback));
        }

        void remove_on_resume(_In_ event_cookie cookie)
        {
            this->_resumeEventSource.remove(cookie);
        }

        event_cookie add_on_canceled(_In_ CallbackType callback)
        {
            return this->_canceledEventSource.add(std::move(callback));
        }

        void remove_on_canceled(_In_ event_cookie cookie)
        {
            this->_canceledEventSource.remove(cookie);
        }

        event_cookie add_on_completed(_In_ CallbackType callback)
        {
            return this->_completedEventSource.add(std::move(callback));
        }

        void remove_on_completed(_In_ event_cookie cookie)
        {
            this->_completedEventSource.remove(cookie);
        }



        /*
         * Public virtual Function(s)
         */
        virtual animation_state on_update(_In_ duration /*elapsedTime*/)
        {
            // By default, immediately transition to the completed state
            return animation_state::completed;
        }

        virtual void on_state_change(_In_ animation_state newState)
        {
            auto oldState = this->_currentState;
            this->_currentState = newState;

            switch (newState)
            {
            case animation_state::running:
                if (oldState == animation_state::pending)
                {
                    this->_beginEventSource.invoke_all(oldState, newState);
                }
                else if (oldState == animation_state::paused)
                {
                    this->_resumeEventSource.invoke_all(oldState, newState);
                }
                else
                {
                    assert(false);
                }
                break;

            case animation_state::paused:
                if (oldState == animation_state::running)
                {
                    this->_pauseEventSource.invoke_all(oldState, newState);
                }
                else
                {
                    assert(false);
                }
                break;

            case animation_state::canceled:
                assert((oldState != animation_state::canceled) && (oldState != animation_state::completed));
                this->_canceledEventSource.invoke_all(oldState, newState);
                break;

            case animation_state::completed:
                assert(oldState != animation_state::completed);
                this->_completedEventSource.invoke_all(oldState, newState);
                break;

            default:
                assert(false);
                break;
            }
        }



    private:

        // State
        animation_state _currentState;

        // Event sources
        EventSourceType _beginEventSource;
        EventSourceType _pauseEventSource;
        EventSourceType _resumeEventSource;
        EventSourceType _canceledEventSource;
        EventSourceType _completedEventSource;
    };



    /*
     * Helper derivatives of animation
     */
    namespace garbage
    {
        /*
         * key_frame_animation. This maintains a set of (time, value) ordered based off 'time.' The key_frame_animation
         * class works based off of a 'next' iterator that describes the next key frame that will be encountered. For
         * example, if the animation has not yet begun, then next will be equal to std::begin(_keyFrames), and if the
         * animation has completed, then next will be equal to std::end(_keyFrames). Note that there is no concept of
         * 'current' iterator since one may not exist (e.g. _keyFrames is empty, or the first key frame has not yet
         * been encountered). Derived classes should use the begun() and completed() functions appropriately.
         */
        template <typename Ty>
        class key_frame_animation :
            public animation
        {
        protected:

            /*
             * Protected types
             */
            using container_type = std::map<duration, Ty>;
            using iterator_type = typename container_type::iterator;
            using update_function = std::function<void(const Ty &)>;



        public:
            /*
             * Constructor(s)/Destructor
             */
            key_frame_animation(void) :
                _totalElapsedTime(0),
                _next(std::begin(this->_keyFrames))
            {
            }

            template <typename Func>
            key_frame_animation(_In_ const Func &func) :
                key_frame_animation()
            {
                this->set_callback(func);
            }



            /*
             * Overloaded on_update. Any derived class should call this function to ensure proper state
             */
            virtual animation_state on_update(_In_ duration elapsedTime)
            {
                this->_totalElapsedTime += elapsedTime;
                return this->completed() ? animation_state::completed : animation_state::running;
            }



            /*
             * Public functions
             */
            template <typename Func>
            void set_callback(_In_ const Func &func)
            {
                this->_updateFunc = func;
            }

            void add_key_frame(_In_ duration time, _In_ Ty &value)
            {
                this->_keyFrames.emplace(time, value);

                if (this->_next != std::begin(this->_keyFrames))
                {
                    // We may need to update our _next iterator. This can at most move back by one, so just go ahead
                    // and do that and then force the update of the _next iterator
                    --this->_next;
                    this->next();
                }
            }

            void add_key_frame(_In_ duration time, _In_ Ty &&value)
            {
                this->_keyFrames.emplace(time, std::move(value));

                if (this->_next != std::begin(this->_keyFrames))
                {
                    // We may need to update our _next iterator. This can at most move back by one, so just go ahead
                    // and do that and then force the update of the _next iterator
                    --this->_next;
                    this->next();
                }
            }



        protected:

            iterator_type next(void)
            {
                while ((this->_next != std::end(this->_keyFrames)) && (this->_next->first <= this->_totalElapsedTime))
                {
                    ++this->_next;
                }

                return this->_next;
            }

            bool begun(void)
            {
                return this->next() != std::begin(this->_keyFrames);
            }

            bool completed(void)
            {
                return this->next() == std::end(this->_keyFrames);
            }

            duration elapsed_time(void) const
            {
                return this->_totalElapsedTime;
            }

            void update(_In_ const Ty &value)
            {
                if (this->_updateFunc)
                {
                    this->_updateFunc(value);
                }
            }



        private:

            duration _totalElapsedTime;
            container_type _keyFrames;
            iterator_type _next;
            update_function _updateFunc;
        };
    }
}

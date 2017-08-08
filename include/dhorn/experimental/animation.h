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
    namespace experimental
    {
        /*
         * animation_state
         */
        enum class animation_state
        {
            pending = 0,
            running = 1,
            paused = 2,
            completed = 3,
            canceled = 4,
        };



        /*
         * Common animation helpers
         */
        namespace details
        {
            // Helpers surrounding animation_state in case the enum ever expands. We have four conceptual states: pending,
            // running, paused, and completed, though we have more than four "true" states
            inline constexpr bool is_complete(animation_state state)
            {
                return (state == animation_state::completed) || (state == animation_state::canceled);
            }

            inline constexpr bool is_running(animation_state state)
            {
                return state == animation_state::running;
            }

            inline constexpr bool is_paused(animation_state state)
            {
                return state == animation_state::paused;
            }

            inline constexpr bool is_pending(animation_state state)
            {
                return state == animation_state::pending;
            }
        }



        /*
         * animation Base Class
         */
        class animation
        {
            using EventSourceType = event_source<void(animation *, animation_state /*prev*/, animation_state /*new*/)>;
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
            event_cookie add_state_change(const CallbackType &callback)
            {
                return this->_stateChangeEventSource.add(callback);
            }

            void remove_state_change(event_cookie cookie)
            {
                this->_stateChangeEventSource.remove(cookie);
            }



            /*
             * Public virtual Function(s)
             */
            virtual animation_state on_update(duration /*elapsedTime*/)
            {
                // By default, immediately transition to the completed state
                return animation_state::completed;
            }

            virtual void on_state_change(animation_state newState)
            {
                const auto oldState = this->_currentState;
                if (oldState != newState)
                {
                    this->_currentState = newState;
                    this->_stateChangeEventSource.invoke_all(this, oldState, newState);
                }
            }



        private:

            // State
            animation_state _currentState;

            // Event sources
            EventSourceType _stateChangeEventSource;
        };



        /*
         * Helper derivatives of animation
         */
        namespace details
        {
            /*
             * key_frame_animation. This maintains a set of (time, value) ordered based off 'time.' The key_frame_animation
             * class works based off of a 'next' iterator that describes the next key frame that will be encountered. For
             * example, if the animation has not yet begun, then next will be equal to begin(_keyFrames), and if the
             * animation has completed, then next will be equal to end(_keyFrames). Note that there is no concept of
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
                    _next(this->_keyFrames.begin())
                {
                }

                key_frame_animation(update_function func) :
                    key_frame_animation()
                {
                    this->set_callback(std::move(func));
                }



                /*
                 * Overloaded on_update. Any derived class should call this function to ensure proper state
                 */
                virtual animation_state on_update(duration elapsedTime)
                {
                    this->_totalElapsedTime += elapsedTime;
                    this->next();
                    return this->completed() ? animation_state::completed : animation_state::running;
                }



                /*
                 * Public functions
                 */
                void set_callback(const update_function &func)
                {
                    this->_updateFunc = func;
                }

                void add_key_frame(duration time, const Ty &value)
                {
                    this->_keyFrames.emplace(time, value);

                    if (this->_next != this->_keyFrames.begin())
                    {
                        // We may need to update our _next iterator. This can at most move back by one, so just go ahead
                        // and do that since we force the update of the _next iterator
                        --this->_next;
                    }

                    this->next();
                }

                void add_key_frame(duration time, Ty &&value)
                {
                    this->_keyFrames.emplace(time, std::move(value));

                    if (this->_next != this->_keyFrames.begin())
                    {
                        // We may need to update our _next iterator. This can at most move back by one, so just go ahead
                        // and do that since we force the update of the _next iterator
                        --this->_next;
                    }

                    this->next();
                }



            protected:

                const iterator_type &next(void)
                {
                    while ((this->_next != this->_keyFrames.end()) && (this->_next->first <= this->_totalElapsedTime))
                    {
                        ++this->_next;
                    }

                    return this->_next;
                }

                bool begun(void) const
                {
                    return this->_next != this->_keyFrames.begin();
                }

                bool completed(void) const
                {
                    return this->_next == this->_keyFrames.end();
                }

                duration elapsed_time(void) const
                {
                    return this->_totalElapsedTime;
                }

                void update(const Ty &value)
                {
                    if (this->_updateFunc)
                    {
                        this->_updateFunc(value);
                    }
                }



            private:

                duration _totalElapsedTime{ 0 };
                container_type _keyFrames;
                iterator_type _next;
                update_function _updateFunc;
            };
        }
    }
}

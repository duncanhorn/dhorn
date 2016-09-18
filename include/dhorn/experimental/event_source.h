/*
 * Duncan Horn
 *
 * event_source.h
 *
 * Represents a source of event firing in event-driven scenarios. Note that the event_source maintains order of what's
 * inserted and will fire events in the same order that they are added to the event_source
 */
#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <utility>

namespace dhorn
{
    /*
     * Type Definitions
     */
#pragma region Type Definitions

    using event_cookie = size_t;
    static const event_cookie invalid_event_cookie = 0;

#pragma endregion



    /*
     * event_source
     */
    template <typename Func>
    class event_source;

    template <typename ReturnType, typename... Args>
    class event_source<ReturnType(Args...)> final
    {
        using CallbackType = std::function<ReturnType(Args...)>;
        using storage_type = std::map<event_cookie, CallbackType>;

    public:

        /*
         * Type Definitions
         */
        using callback_type = CallbackType;
        using size_type = typename storage_type::size_type;



        /*
         * Constructor(s)/Destructor
         */
        event_source(void) :
            _nextEventCookie(invalid_event_cookie)
        {
        }

        event_source(event_source &&other) :
            _eventTargets(std::move(other._eventTargets)),
            _nextEventCookie(other._nextEventCookie)
        {
        }

        // Cannot copy (does not make sense to...)
        event_source(const event_source &) = delete;
        event_source &operator=(const event_source &) = delete;



        /*
         * Operators
         */
        event_source &operator=(event_source &&other)
        {
            this->_eventTargets = std::move(other._eventTargets);
            this->_nextEventCookie = other._nextEventCookie;
        }



        /*
         * Public Functions
         */
        event_cookie add(const callback_type &func)
        {
            auto itr = this->_eventTargets.emplace(++this->_nextEventCookie, func);
            assert(itr.second);

            return this->_nextEventCookie;
        }

        void remove(event_cookie cookie)
        {
            this->_eventTargets.erase(this->FindEvent(cookie));
        }

        void invoke_one(Args ...args) const
        {
            auto itr = std::begin(this->_eventTargets);
            if (itr != std::end(this->_eventTargets))
            {
                itr->second(args...);
            }
        }

        template <typename ResultFunc>
        void invoke_one(Args ...args, const ResultFunc &func) const
        {
            // Allow callers to handle failures
            auto itr = std::begin(this->_eventTargets);
            if (itr != std::end(this->_eventTargets))
            {
                func(itr->second(args...));
            }
        }

        void invoke_all(Args ...args) const
        {
            for (auto &pair : this->_eventTargets)
            {
                pair.second(std::forward<Args>(args)...);
            }
        }

        template <typename ResultFunc>
        void invoke_all(Args ...args, const ResultFunc &func)
        {
            for (auto &pair : this->_eventTargets)
            {
                // Allow callers to handle failures
                func(pair.second(std::forward<Args>(args)...));
            }
        }

        size_type size(void) const
        {
            return this->_eventTargets.size();
        }

        bool empty(void) const
        {
            return this->_eventTargets.empty();
        }



    private:

        typename storage_type::const_iterator FindEvent(event_cookie cookie) const
        {
            auto itr = this->_eventTargets.find(cookie);

            if (itr == std::end(this->_eventTargets))
            {
                throw std::out_of_range("Event does not exist");
            }

            return itr;
        }

        storage_type _eventTargets;
        event_cookie _nextEventCookie;
    };
}

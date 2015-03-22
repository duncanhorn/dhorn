/*
 * event_source.h
 *
 * Duncan Horn
 *
 * Represents a source of event firing in event-driven scenarios.
 */
#pragma once

#include <algorithm>
#include <functional>
#include <list>
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
    template <typename EventFuncType>
    class event_source final
    {
        using event_function = std::function<EventFuncType>;
        using element_type = std::pair<event_cookie, event_function>;
        using storage_type = std::list<element_type>;

    public:

        /*
         * Type Definitions
         */
        using size_type = typename storage_type::size_type;



        /*
         * Constructor(s)/Destructor
         */
        event_source(void) :
            _nextEventCookie(invalid_event_cookie)
        {
        }

        event_source(_Inout_ event_source &&other) :
            _eventTargets(std::move(other._eventTargets)),
            _nextEventCookie(invalid_event_cookie)
        {
        }

        // Cannot copy (does not make sense to...)
        event_source(_In_ const event_source &) = delete;
        event_source &operator=(_In_ const event_source &) = delete;



        /*
         * Public Functions
         */
        event_cookie add(_In_ event_function func)
        {
            this->_eventTargets.emplace_back(++this->_nextEventCookie, std::move(func));
            return this->_nextEventCookie;
        }

        void remove(_In_ event_cookie cookie)
        {
            this->_eventTargets.erase(this->FindEvent(cookie));
        }

        void invoke_one(void) const
        {
            this->_eventTargets.front().second();
        }

        template <typename ResultFunc>
        void invoke_one(_In_ const ResultFunc &func) const
        {
            // Allow callers to handle failures
            func(this->_eventTargets.front().second());
        }

        void invoke_all(void) const
        {
            for (auto &pair : this->_eventTargets)
            {
                pair.second();
            }
        }

        template <typename ResultFunc>
        void invoke_all(_In_ const ResultFunc &func)
        {
            for (auto &pair : this->_eventTargets)
            {
                // Allow callers to handle failures
                func(pair.second());
            }
        }

        size_type size(void) const
        {
            return this->_eventTargets.size();
        }



    private:

        typename storage_type::const_iterator FindEvent(_In_ event_cookie cookie) const
        {
            auto itr = std::find_if(std::begin(this->_eventTargets), std::end(this->_eventTargets),
                [&](_In_ const element_type &val) -> bool
            {
                return val.first == cookie;
            });

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

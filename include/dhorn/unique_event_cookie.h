/*
 * Duncan Horn
 *
 * basic_unique_event_cookie.h
 *
 * An RAII wrapper around the event_cookie type that comes from an event_source.
 */
#pragma once

#include "event_source.h"

namespace dhorn
{
    /*
     * unique_event_cookie
     */
#pragma region unique_event_cookie

    template <typename DestroyFuncType = void(event_cookie)>
    class basic_unique_event_cookie final
    {
        using destroy_func_type = std::function<DestroyFuncType>;

    public:
        /*
         * Constructor(s)/Destructor
         */
        basic_unique_event_cookie(void) :
            _cookie(invalid_event_cookie)
        {
        }

        basic_unique_event_cookie(basic_unique_event_cookie &&other) :
            basic_unique_event_cookie()
        {
            this->swap(other);
        }

        basic_unique_event_cookie(event_cookie cookie, destroy_func_type destroyFunc) :
            _cookie(cookie),
            _destroyFunc(std::move(destroyFunc))
        {
        }

        ~basic_unique_event_cookie(void)
        {
            this->Destroy();
        }

        // Cannot copy
        basic_unique_event_cookie(const basic_unique_event_cookie &) = delete;
        basic_unique_event_cookie &operator=(const basic_unique_event_cookie &) = delete;



        /*
         * Operators
         */
        basic_unique_event_cookie &operator=(basic_unique_event_cookie &&other)
        {
            this->swap(other);
        }

        operator event_cookie(void) const
        {
            return this->_cookie;
        }



        /*
         * Public Functions
         */
        void reset(void)
        {
            this->Destroy();
        }

        void reset(event_cookie cookie, const destroy_func_type &destroyFunc)
        {
            this->Destroy();
            this->_cookie = cookie;
            this->_destroyFunc = destroyFunc;
        }

        event_cookie release(void)
        {
            auto cookie = this->_cookie;
            this->_cookie = invalid_event_cookie;
            return cookie;
        }

        void swap(basic_unique_event_cookie &other)
        {
            std::swap(this->_cookie, other._cookie);
            std::swap(this->_destroyFunc, other._destroyFunc);
        }



    private:

        void Destroy(void)
        {
            if (this->_cookie != invalid_event_cookie)
            {
                this->_destroyFunc(this->_cookie);
                this->_cookie = invalid_event_cookie;
            }
        }

        event_cookie _cookie;
        destroy_func_type _destroyFunc;
    };

    template <typename DestroyFuncType>
    basic_unique_event_cookie<DestroyFuncType> make_event_cookie(
        event_cookie cookie,
        const std::function<DestroyFuncType> &destroyFunc)
    {
        return basic_unique_event_cookie<DestroyFuncType>(cookie, destroyFunc);
    }



    // C++ still requires a template argument list for types with default values for all template arguments. Hence, if
    // we do not do this typedef, the syntax would be dhorn::unique_event_cookie<> which is a bit odd...
    using unique_event_cookie = basic_unique_event_cookie<void(event_cookie)>;

#pragma endregion
}



#ifndef _DHORN_NO_STD

namespace std
{
    template <typename DestroyFuncType>
    void swap(
        dhorn::basic_unique_event_cookie<DestroyFuncType> &lhs,
        dhorn::basic_unique_event_cookie<DestroyFuncType> &rhs)
    {
        lhs.swap(rhs);
    }
}

#endif

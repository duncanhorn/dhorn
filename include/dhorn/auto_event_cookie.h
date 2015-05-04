/*
 * basic_auto_event_cookie.h
 *
 * Duncan Horn
 *
 * An RAII wrapper around the event_cookie type that comes from an event_source.
 */
#pragma once

#include "event_source.h"

namespace dhorn
{
    /*
     * auto_event_cookie
     */
#pragma region auto_event_cookie

    template <typename DestroyFuncType = void(event_cookie)>
    class basic_auto_event_cookie final
    {
        using destroy_func_type = std::function<DestroyFuncType>;

    public:
        /*
         * Constructor(s)/Destructor
         */
        basic_auto_event_cookie(void) :
            _cookie(invalid_event_cookie)
        {
        }

        basic_auto_event_cookie(_Inout_ basic_auto_event_cookie &&other) :
            basic_auto_event_cookie()
        {
            this->swap(other);
        }

        basic_auto_event_cookie(_In_ event_cookie cookie, _In_ destroy_func_type destroyFunc) :
            _cookie(cookie),
            _destroyFunc(std::move(destroyFunc))
        {
        }

        ~basic_auto_event_cookie(void)
        {
            this->Destroy();
        }

        // Cannot copy
        basic_auto_event_cookie(const basic_auto_event_cookie &) = delete;
        basic_auto_event_cookie &operator=(_In_ const basic_auto_event_cookie &) = delete;



        /*
         * Operators
         */
        basic_auto_event_cookie &operator=(_Inout_ basic_auto_event_cookie &&other)
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

        void reset(_In_ event_cookie cookie, _In_ destroy_func_type destroyFunc)
        {
            this->Destroy();
            this->_cookie = cookie;
            this->_destroyFunc = std::move(destroyFunc);
        }

        event_cookie release(void)
        {
            auto cookie = this->_cookie;
            this->_cookie = invalid_event_cookie;
            return cookie;
        }

        void swap(_Inout_ basic_auto_event_cookie &other)
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



    // C++ still requires a template argument list for types with default values for all template arguments. Hence, if
    // we do not do this typedef, the syntax would be dhorn::auto_event_cookie<> which is a bit odd...
    using auto_event_cookie = basic_auto_event_cookie<void(event_cookie)>;

#pragma endregion
}



#ifndef _DHORN_NO_STD

namespace std
{
    template <typename DestroyFuncType>
    void swap(
        _Inout_ dhorn::basic_auto_event_cookie<DestroyFuncType> &lhs,
        _Inout_ dhorn::basic_auto_event_cookie<DestroyFuncType> &rhs)
    {
        lhs.swap(rhs);
    }
}

#endif

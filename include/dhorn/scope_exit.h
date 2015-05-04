/*
 * Duncan Horn
 *
 * scope_exit.h
 *
 * Simple way to declare an object whose destructor functionality you can define. E.g. if you are needing to set some
 * variable at the end of a function regardless of whether or not there was an exception, then you could do:
 *
 *  dhorn::scope_exit cleanup([&]() { this->_updating = false; });
 */
#pragma once

#include <functional>

namespace dhorn
{
    /*
     * scope_exit
     */
    class scope_exit
    {
    public:
        scope_exit(_In_ std::function<void(void)> func) :
            _func(func)
        {
        }

        ~scope_exit(void)
        {
            if (this->_func)
            {
                _func();
            }
        }

        void cancel(void)
        {
            this->_func = nullptr;
        }

    private:

        std::function<void(void)> _func;
    };
}

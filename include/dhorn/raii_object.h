/*
 * Duncan Horn
 *
 * raii_object.h
 *
 * Simple way to declare an object whose destructor functionality you can define. E.g. if you are
 * needing to set some variable at the end of a function regardless of whether or not there was an
 * exception, then you could do:
 *
 *  dhorn::raii_object raii([&]() { this->_updating = false; });
 */
#pragma once

#include <functional>

namespace dhorn
{
    /*
     * raii_object
     */
    class raii_object
    {
    public:
        raii_object(_In_ std::function<void(void)> func) :
            _func(func)
        {
        }

        ~raii_object(void)
        {
            _func();
        }



    private:

        std::function<void(void)> _func;
    };
}

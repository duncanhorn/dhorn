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
    template <typename FuncTy>
    class scope_exit
    {
    public:
        scope_exit(FuncTy func) :
            _func(std::move(func))
        {
        }

        ~scope_exit(void) noexcept(noexcept(this->_func()))
        {
            if (!this->_cancelled)
            {
                this->_func();
            }
        }

        // Can't copy, but can move
        scope_exit(const scope_exit &) = delete;
        scope_exit &operator=(const scope_exit &) = delete;

        scope_exit(scope_exit &&other) :
            _func(std::move(other._func))
        {
            this->_cancelled = other._cancelled;
            other._cancelled = true;
        }



        void cancel(void)
        {
            this->_cancelled = true;
        }

    private:

        FuncTy _func;
        bool _cancelled = false;
    };

    template <typename FuncTy>
    inline scope_exit<std::decay_t<FuncTy>> make_scope_exit(FuncTy&& func)
    {
        return scope_exit<std::decay_t<FuncTy>>(std::forward<FuncTy>(func));
    }
}

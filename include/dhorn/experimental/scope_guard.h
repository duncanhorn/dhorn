/*
 * Duncan Horn
 *
 * scope_guard.h
 *
 * An RAII wrapper for invoking a specified function object at the destruction of the scope_guard object, or manually
 * via its `operator()` function. A scope_guard object will only invoke the function object once, and will no-op if it
 * has previously been invoked, or if it has been manually cancelled. For example:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * void doit(int &value)
 * {
 *     auto incrementValue = make_scope_guard([&]() { ++value; });
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * If the above function is invoked as-is, when `doit` returns, `value` will be incremented. If the following code is
 * added to the example above:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *     incrementValue();
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Then the result will still be the same (value is incremented once). Note the phrasing "incremented once." That is,
 * `value` does not get incremented again as a result of the scope_guard getting destroyed. E.g. the following example
 * results in the same (`value` is incremented once):
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *     incrementValue();
 *     incrementValue();
 *     incrementValue();
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * However, if the original example was changed to:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *     incrementValue.cancel();
 *     incrementValue();
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Then `value` is *not* incremented at all. Note, however, that if the order is reversed:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *     incrementValue();
 *     incrementValue.cancel();
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Then `value` is still incremented once since the scope_guard was invoked before it was cancelled.
 *
 * Note that scope_guard's destructor is noexcept (just like any other destructor by default) and therefore
 * std::terminate will get called if the function object throws an exception that it does not handle.
 */
#pragma once

#include <functional>

namespace dhorn
{
    /*
     * scope_guard
     */
    template <typename FuncTy>
    class scope_guard
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        template <typename Func>
        explicit scope_guard(Func &&func) :
            _func(std::forward<Func>(func))
        {
        }

        ~scope_guard(void)
        {
            if (this->_shouldInvoke)
            {
                this->_func();
            }
        }

        // Can't copy, but can move
        scope_guard(const scope_guard &) = delete;
        scope_guard &operator=(const scope_guard &) = delete;

        scope_guard(scope_guard &&other) :
            _func(std::move(other._func))
        {
            this->_shouldInvoke = other._shouldInvoke;
            other._shouldInvoke = false;
        }



        /*
         * cancel
         *
         * Sets a bit on the scope_guard to prevent it from executing the function object, if it hasn't already.
         */
        void cancel(void)
        {
            this->_shouldInvoke = false;
        }

        /*
         * operator()
         *
         * Invokes the function object if it hasn't been cancelled or executed already.
         */
        void operator()(void)
        {
            // Can only invoke once
            if (this->_shouldInvoke)
            {
                this->_shouldInvoke = false;
                this->_func();
            }
        }



    private:

        FuncTy _func;
        bool _shouldInvoke = true;
    };



    /*
     * make_scope_guard
     *
     * Makes a scope_guard object for the input function object (generally a lambda).
     */
    template <typename FuncTy>
    inline scope_guard<std::decay_t<FuncTy>> make_scope_guard(FuncTy&& func)
    {
        return scope_guard<std::decay_t<FuncTy>>(std::forward<FuncTy>(func));
    }
}

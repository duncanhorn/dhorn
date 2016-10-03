/*
 * Duncan Horn
 *
 * scope_guard.h
 *
 * 


 * Simple way to declare an object whose destructor functionality you can define. E.g. if you are needing to set some
 * variable at the end of a function regardless of whether or not there was an exception, then you could do:
 *
 *  dhorn::scope_guard cleanup([&]() { this->_updating = false; });
 */
#pragma once

#include <functional>

namespace dhorn
{
    namespace experimental
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

        template <typename FuncTy>
        inline scope_guard<std::decay_t<FuncTy>> make_scope_guard(FuncTy&& func)
        {
            return scope_guard<std::decay_t<FuncTy>>(std::forward<FuncTy>(func));
        }
    }
}

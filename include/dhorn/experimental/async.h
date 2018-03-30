/*
 * Duncan Horn
 *
 * async.h
 *
 * A future-like type optimized for use with coroutines
 */
#pragma once

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <experimental/resumable>
#include <future> // TODO: Don't piggy-back off future?
#include <mutex>
#include <optional>

#include "../crtp_base.h"
#include "../debug.h"

namespace dhorn::experimental
{
    // Forward Declaration of async, as it's needed for the declaration of get_return_object()
    template <typename Ty>
    class async;



    namespace details
    {
        /*
         * ref_counted_suspend
         *
         * Used for `final_suspend` for reference counted types. Returns `true` for `await_ready` iff the reference
         * count (initialized with the decremented value) reaches zero
         */
        template <typename Ty>
        struct ref_counted_suspend
        {
            Ty ref_count;

            bool await_ready()
            {
                return ref_count <= 0;
            }

            void await_suspend(std::experimental::coroutine_handle<>)
            {
            }

            void await_resume()
            {
            }
        };



        /*
         * async_promise_type
         *
         * The `promise_type` for `async<T>`. This type is similar to `std::promise` - or perhaps more accurately, the
         * shared state created by the `std::promise` - but is _not_ intended to be used/constructed by client code.
         * This provides a few benefits:
         *
         *      1.  The compiler ensures that either a value or exception must be set before the coroutine completes.
         *          I.e. must `co_return` - or throw - in all code paths, so `broken_promise` is not possible.
         *      2.  Coroutines synchronize with themselves and therefore data races are avoided with no additional work
         *      3.  The `async_promise_type` is allocated by the compiler as a part of the coroutine's state, unlike
         *          `std::promise` where the shared state is allocated separately
         *
         * These simplicites make the state machine far simpler and thus more efficient.
         */
        template <typename Ty>
        class async_promise_type;

        template <typename Ty>
        class async_promise_type_base :
            public crtp_base<async_promise_type<Ty>>
        {
        public:

            /*
             * PromiseType
             */
            async<Ty> get_return_object();

            auto initial_suspend()
            {
                return std::experimental::suspend_never{};
            }

            auto final_suspend()
            {
                assert(this->derived().done());
                //assert(this->_value || this->_exception);
                return ref_counted_suspend<int>{ --this->_refCount };
            }

            void set_exception(std::exception_ptr exception)
            {
                std::lock_guard<std::mutex> guard(this->_mutex);
                //assert(!this->_value && !this->_exception);
                this->_exception = exception;
                this->_cond.notify_all();
            }



            /*
             * Implementation
             */
            //std::experimental::coroutine_handle<async_promise_type> handle() noexcept
            //{
            //    return std::experimental::coroutine_handle<async_promise_type>::from_promise(*this);
            //}

            //Ty& get()
            //{
            //    std::unique_lock<std::mutex> lock(this->_mutex);
            //    this->_cond.wait(lock, [&]() { return done(); });

            //    // NOTE: Once we get this far, the operation is complete and taking the value by ref is OKAY
            //    if (this->_value)
            //    {
            //        return *this->_value;
            //    }
            //    else
            //    {
            //        assert(this->_exception);
            //        std::rethrow_exception(this->_exception);
            //    }
            //}

            //void wait()
            //{
            //    std::unique_lock<std::mutex> lock(this->_mutex);
            //    this->_cond.wait(lock, [&]() { return done(); });
            //}

            //template <typename Rep, typename Period>
            //std::future_status wait_for(const std::chrono::duration<Rep, Period>& timeoutDuration) // const
            //{
            //    std::unique_lock<std::mutex> lock(this->_mutex);
            //    auto done = this->_cond.wait_for(lock, timeoutDuration, [&]() { return done(); });
            //    return done ? std::future_status::ready : std::future_status::timeout;
            //}

            //template <typename Clock, typename Duration>
            //std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& timeoutTime) // const
            //{
            //    std::unique_lock<std::mutex> lock(this->_mutex);
            //    auto done = this->_cond.wait_until(lock, timeoutTime, [&]() { return done(); });
            //    return done ? std::future_status::ready : std::future_status::timeout;
            //}

            int add_reference() noexcept
            {
                auto result =  ++this->_refCount;
                assert(result > 1);
                return result;
            }

            //int release() noexcept
            //{
            //    auto result = --this->_refCount;
            //    assert(result >= 0);

            //    if (result == 0)
            //    {
            //        handle().destroy();
            //    }

            //    return result;
            //}



        protected:

            //bool done()
            //{
            //    assert(!this->_mutex.try_lock() || (this->_mutex.unlock(), false));
            //    return this->_value || this->_exception;
            //}

            std::mutex _mutex;
            std::condition_variable _cond;

            std::atomic_int _refCount{ 1 };
            std::exception_ptr _exception;
        };

        template <typename Ty>
        class async_promise_type
        {
            using storage = std::conditional_t<std::is_reference_v<Ty>,
                std::reference_wrapper<std::remove_reference_t<Ty>>,
                Ty>;

        public:

            /*
             * PromiseType
             */
            async<Ty> get_return_object();

            auto initial_suspend()
            {
                return std::experimental::suspend_never{};
            }

            auto final_suspend()
            {
                assert(this->_value || this->_exception);
                return ref_counted_suspend<int>{ --this->_refCount };
            }

            void set_exception(std::exception_ptr exception)
            {
                std::lock_guard<std::mutex> guard(this->_mutex);
                assert(!this->_value && !this->_exception);
                this->_exception = exception;
                this->_cond.notify_all();
            }

            // TODO! How to handle async<void>? Specialization?
            //void return_void()
            //{
            //}

            template <typename ValueTy, std::enable_if_t<std::is_convertible_v<ValueTy, Ty>, int> = 0>
            void return_value(ValueTy&& value)
            {
                std::lock_guard<std::mutex> guard(this->_mutex);
                assert(!this->_value && !this->_exception);
                this->_value = std::forward<ValueTy>(value);
                this->_cond.notify_all();
            }



            /*
             * Implementation
             */
            std::experimental::coroutine_handle<async_promise_type> handle() noexcept
            {
                return std::experimental::coroutine_handle<async_promise_type>::from_promise(*this);
            }

            Ty& get()
            {
                std::unique_lock<std::mutex> lock(this->_mutex);
                this->_cond.wait(lock, [&]() { return done(); });

                // NOTE: Once we get this far, the operation is complete and taking the value by ref is OKAY
                if (this->_value)
                {
                    return *this->_value;
                }
                else
                {
                    assert(this->_exception);
                    std::rethrow_exception(this->_exception);
                }
            }

            void wait()
            {
                std::unique_lock<std::mutex> lock(this->_mutex);
                this->_cond.wait(lock, [&]() { return done(); });
            }

            template <typename Rep, typename Period>
            std::future_status wait_for(const std::chrono::duration<Rep, Period>& timeoutDuration) // const
            {
                std::unique_lock<std::mutex> lock(this->_mutex);
                auto done = this->_cond.wait_for(lock, timeoutDuration, [&]() { return done(); });
                return done ? std::future_status::ready : std::future_status::timeout;
            }

            template <typename Clock, typename Duration>
            std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& timeoutTime) // const
            {
                std::unique_lock<std::mutex> lock(this->_mutex);
                auto done = this->_cond.wait_until(lock, timeoutTime, [&]() { return done(); });
                return done ? std::future_status::ready : std::future_status::timeout;
            }

            int add_reference() noexcept
            {
                auto result =  ++this->_refCount;
                assert(result > 1);
                return result;
            }

            int release() noexcept
            {
                auto result = --this->_refCount;
                assert(result >= 0);

                if (result == 0)
                {
                    handle().destroy();
                }

                return result;
            }



        private:

            bool done()
            {
                assert_lock_held(this->_mutex);
                return this->_value || this->_exception;
            }

            std::mutex _mutex;
            std::condition_variable _cond;

            std::optional<storage> _value;
            std::atomic_int _refCount{ 1 };
            std::exception_ptr _exception;
        };
    }



    /*
     * async
     */
    template <typename Ty>
    class async
    {
    public:
        /*
         * Public Types
         */
        using promise_type = details::async_promise_type<Ty>;



    private:

        friend class promise_type;

        // Construct w/ promise
        async(promise_type& promise) noexcept :
            _handle(promise.handle())
        {
            promise.add_reference();
        }



    public:

        /*
         * Constructor(s)/Destructor
         */
        async() = default;

        async(const async& other) = delete;
        async(async&& other) noexcept :
            _handle(std::move(other._handle))
        {
        }

        ~async()
        {
            if (this->_handle)
            {
                reset();
            }
        }



        /*
         * Assignment
         */
        async& operator=(const async&) = delete;
        async& operator=(async&& other) noexcept
        {
            std::swap(this->_handle, other._handle);
        }



        /*
         * Result
         */
        Ty get()
        {
            if (!this->_handle)
            {
                throw std::future_error(std::future_errc::no_state);
            }

            Ty result = std::forward<Ty>(this->_handle.promise().get());
            reset();
            return result;
        }



        /*
         * State
         */
        bool valid() const noexcept
        {
            return static_cast<bool>(this->_handle);
        }

        void wait() // const
        {
            assert(valid());
#ifndef DHORN_ASYNC_WAIT_UNCHECKED
            if (!valid())
            {
                throw std::future_error(std::future_errc::no_state);
            }
#endif

            this->_handle.promise().wait();
        }

        template <typename Rep, typename Period>
        std::future_status wait_for(const std::chrono::duration<Rep, Period>& timeoutDuration) // const
        {
            assert(valid());
#ifndef DHORN_ASYNC_WAIT_UNCHECKED
            if (!valid())
            {
                throw std::future_error(std::future_errc::no_state);
            }
#endif

            return this->_handle.promise().wait_for(timeoutDuration);
        }

        template <typename Clock, typename Duration>
        std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& timeoutTime) // const
        {
            assert(valid());
#ifndef DHORN_ASYNC_WAIT_UNCHECKED
            if (!valid())
            {
                throw std::future_error(std::future_errc::no_state);
            }
#endif

            return this->_handle.promise().wait_until(timeoutTime);
        }

        // TODO: share



    private:

        void reset() noexcept
        {
            assert(this->_handle);
            this->_handle.promise().release();
            this->_handle = nullptr;
        }

        std::experimental::coroutine_handle<promise_type> _handle;
    };



    /*
     * Implementation of promise_type forward declarations
     */
    namespace details
    {
        template <typename Ty>
        async<Ty> async_promise_type<Ty>::get_return_object()
        {
            return async<Ty>(*this);
        }
    }
}

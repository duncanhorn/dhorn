/*
 * Duncan Horn
 *
 * thread_pool.h
 *
 * Maintains a set of threads that consumers can queue work onto. This set of threads is separated into two different
 * categories: running threads and available threads. Running threads are threads that are actively processing work.
 * Available threads are threads that are waiting for work. That is, if there are five available threads and you
 * schedule five units of work, then all five tasks are guaranteed to begin execution without the possiblity of needing
 * to wait on a previous unit of work to complete. Note, however, that this does _not_ imply that tasks scheduled when
 * there are no available threads are guaranteed to wait for a running task to complete. Instead, the number of running
 * threads is compared to the maximum number of allowed threads. If there are fewer running threads than maximally
 * allowed, a new thread is created for the task and added to the set of running threads. By default, there is no max
 * number of threads that a thread_pool can create, so any work that is scheduled is guaranteed to not have to wait on
 * any prior work to complete before starting (though, of course, it may have to wait for a thread to spin up). If the
 * max number of threads is reduced to a value smaller than the number of running threads, no work is terminated
 * pre-maturely. As tasks complete, those threads will be cleaned up until the total number of threads in the
 * thread_pool (running + availabe) is equal to the max count.
 *
 * In addition to the max number of running threads, there are several other properties that can be configured to
 * achieve desired behavior/performance:
 *
 *      min_threads             The minimum number of threads allowed in the thread pool at any given time. Note that if
 *                              this value starts out as non-zero, then this number of threads will get created on
 *                              construction of the thread_pool.
 *      max_threads             Already discussed. This is the maximum nuber of threads that the thread_pool will create
 *                              to service tasks.
 *      max_available_threads   The maximum number of threads that the thread_pool will allow at any given time that are
 *                              _not_ actively processing any work. I.e. this allows the thread_pool to start releasing
 *                              resources that are no longer actively being used.
 */
#pragma once

#include <algorithm>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

#include "debug.h"
#include "functional.h"
#include "scope_guard.h"

namespace dhorn
{
#pragma region Creation Behaviors

    /*
     * default_thread_creation_behavior
     *
     * Thread creation behavior that does nothing. This is the default thread creation behavior used by thread_pool.
     * Custom behaviors can return an RAII type to specify cleanup behavior.
     */
    struct default_thread_creation_behavior
    {
        void operator()()
        {
        }
    };

#pragma endregion



#pragma region Thread Pool Traits

    /*
     * default_thread_pool_traits
     *
     * The default thread_pool configuration is to have no min or max number of threads by default.
     */
    struct default_thread_pool_traits
    {
        // No-op on thread creation
        using creation_behavior = default_thread_creation_behavior;

        // Allow an "infinite" number of threads by default
        static constexpr size_t initial_max_threads()
        {
            return std::numeric_limits<size_t>::max();
        }

        // Don't force thread creation on construction
        static constexpr size_t initial_min_threads()
        {
            return 0;
        }

        // By default, don't let the number of non-running threads add up too much
        static size_t initial_max_available_threads()
        {
            return std::max(4u, std::thread::hardware_concurrency());
        }
    };



    /*
     * single_thread_thread_pool_traits
     *
     * Thread pool traits type that, unless overridden, will always have exactly one thread running. This is useful if
     * consumers want a single dedicated thread that they can run tasks on. E.g. tasks that need to be synchronized with
     * respect to one another that potentially may be long-running and shouldn't block the threads that submit the tasks
     */
    struct single_thread_thread_pool_traits
    {
        // No-op on thread creation
        using creation_behavior = default_thread_creation_behavior;

        // We always want one thread
        static constexpr size_t initial_max_threads()
        {
            return 1;
        }

        // We always want one thread, even on creation
        static constexpr size_t initial_min_threads()
        {
            return 1;
        }

        // Since min == max, this value doesn't really matter. One is technically the truth
        static constexpr size_t initial_max_available_threads()
        {
            return 1;
        }
    };

#pragma endregion



    /*
     * thread_pool_priority
     *
     * The priority in which to queue up new tasks to run. Note that these values are only ever meaningful if there is a
     * non-infinite max number of threads allowed.
     */
    enum class thread_pool_priority
    {
        low     = 0,
        normal  = 1,
        high    = 2,
    };



    namespace details
    {
        /*
         * thread_creation_behavior_result
         *
         * Some creation behaviors (such as the default) don't require cleanup and return void. We can't assign the
         * result of a void-returning function to a variable, so return something else that'll get optimized away.
         */
        struct thread_creation_behavior_result
        {
            template <typename CreationBehavior>
            using behavior_result = decltype(CreationBehavior()());

            template <typename CreationBehavior>
            static const bool is_behavior_result_void = std::is_void_v<behavior_result<CreationBehavior>>;

            template <typename CreationBehavior, std::enable_if_t<is_behavior_result_void<CreationBehavior>, int> = 0>
            static auto start(CreationBehavior& value)
            {
                value();

                // Compiler warns if we assign a value such as int to an otherwise unused variable, so make sure that we
                // return a struct instead
                struct nil{};
                return nil{};
            }

            template <typename CreationBehavior, std::enable_if_t<!is_behavior_result_void<CreationBehavior>, int> = 0>
            static auto start(CreationBehavior& value)
            {
                return value();
            }
        };



#pragma region Thread Pool Task Information

        /*
         * thread_pool_task_type
         *
         * Describes how the thread_pool thread should handle the task
         */
        enum class thread_pool_task_type
        {
            // Lets the thread know that the task has valid work for it to execute
            execute,

            // Signals to the thread that it should terminate execution
            shutdown,
        };



        /*
         * thread_pool_task
         *
         * Describes a unit of work that a thread_pool threads execute.
         */
        struct thread_pool_task
        {
            thread_pool_task_type type;
            std::function<void(void)> operation;
        };

#pragma endregion



        /*
         * thread_pool_impl
         *
         * It's very clear that we need shared storage for communication, but our desired usage pattern for thread_pool
         * is to not require any specific lifetime management technique. Thus, thread_pool simply just references a
         * shared_ptr of thread_pool_impl, which is more or less the "real" implementation.
         */
        template <typename CreationBehavior>
        class thread_pool_impl :
            public std::enable_shared_from_this<thread_pool_impl<CreationBehavior>>
        {
            void assert_locked() const
            {
                assert_lock_held(this->_mutex);
            }

        public:
            /*
             * Constructor(s)/Destructor
             */
            thread_pool_impl(size_t minThreads, size_t maxThreads, size_t maxWaiting) :
                _minThreads(minThreads),
                _maxThreads(maxThreads),
                _maxWaitingThreads(maxWaiting)
            {
            }

            // No copies allowed
            thread_pool_impl(const thread_pool_impl&) = delete;
            thread_pool_impl& operator=(const thread_pool_impl&) = delete;

            // shared_from_this doesn't work during construction, so we need a separate 'start' function
            void start()
            {
                std::lock_guard<std::mutex> guard(this->_mutex); // Since we assert
                ensure_thread_count();
            }



            /*
             * Information
             */
            size_t count() const
            {
                std::lock_guard<std::mutex> guard(this->_mutex);
                return this->_threadCount;
            }



            /*
             * Shutdown
             */
            void join()
            {
                std::unordered_map<std::thread::id, std::thread> threads;
                {
                    std::lock_guard<std::mutex> guard(this->_mutex);
                    validate_running();

                    this->_running = false;
                    this->_threads.swap(threads);
                }

                // Wake up all waiting threads so that they can shut down
                this->_taskAvailable.notify_all();

                for (auto& pair : threads)
                {
                    pair.second.join();
                }
            }

            void detach()
            {
                std::unordered_map<std::thread::id, std::thread> threads;
                {
                    std::lock_guard<std::mutex> guard(this->_mutex);
                    validate_running();

                    this->_running = false;
                    this->_threads.swap(threads);
                }

                // Wake up all waiting threads so that they can shut down
                this->_taskAvailable.notify_all();

                for (auto& pair : threads)
                {
                    pair.second.detach();
                }
            }



            /*
             * Task Submission
             */
            void submit(thread_pool_priority priority, std::function<void(void)>&& func)
            {
                std::lock_guard<std::mutex> guard(this->_mutex);
                validate_running();

                emplace_task(priority, std::move(func));

                if ((this->_waitingThreads < this->_taskList.size()) && (this->_threadCount < this->_maxThreads))
                {
                    create_thread();
                }
            }



            /*
             * Configuration
             */
#pragma region Thread Pool Configuration

            size_t get_max_waiting_threads() const
            {
                std::lock_guard<std::mutex> guard(this->_mutex);
                return this->_maxWaitingThreads;
            }

            void change_max_waiting_threads(size_t value)
            {
                std::lock_guard<std::mutex> guard(this->_mutex);
                validate_running();

                this->_maxWaitingThreads = value;
                ensure_thread_count();
            }

            size_t get_max_threads() const
            {
                std::lock_guard<std::mutex> guard(this->_mutex);
                return this->_maxThreads;
            }

            void change_max_threads(size_t value)
            {
                std::lock_guard<std::mutex> guard(this->_mutex);
                validate_running();

                if (value < this->_minThreads)
                {
                    throw std::invalid_argument("Can't set max_threads to a value less than min_threads");
                }

                this->_maxThreads = value;
                ensure_thread_count();
            }

            size_t get_min_threads() const
            {
                std::lock_guard<std::mutex> guard(this->_mutex);
                return this->_minThreads;
            }

            void change_min_threads(size_t value)
            {
                std::lock_guard<std::mutex> guard(this->_mutex);
                validate_running();

                if (value > this->_maxThreads)
                {
                    throw std::invalid_argument("Can't set min_threads to a value greater than max_threads");
                }

                this->_minThreads = value;
                ensure_thread_count();
            }

#pragma endregion



        private:

            void validate_running() const
            {
                assert_locked();
                if (!this->_running)
                {
                    throw std::invalid_argument("Thread pool has already been shut down");
                }
            }

            void create_thread()
            {
                assert_locked();
                assert(this->_running);

                std::thread thread([sharedThis = this->shared_from_this()]() mutable
                {
                    auto noifyShutdown = make_scope_guard([&]()
                    {
                        std::lock_guard<std::mutex> guard(sharedThis->_mutex);

                        auto itr = sharedThis->_threads.find(std::this_thread::get_id());
                        if (itr != std::end(sharedThis->_threads))
                        {
                            // We won't be calling join, so we need to let the std::thread destruct nicely
                            itr->second.detach();
                            sharedThis->_threads.erase(itr);
                        }
                        else
                        {
                            // It must be the case that we are shutting down
                            assert(!sharedThis->_running);
                            assert(sharedThis->_threads.empty());
                        }
                    });

                    // If the creation behavior throws, std::terminate gets called (via standard), so it's not necessary
                    // that we clean up properly in such cases (e.g. by decrementing _waitingThreads)
                    [[maybe_unused]]
                    auto threadCleanup = thread_creation_behavior_result::start(sharedThis->_creationBehavior);

                    for (auto task = sharedThis->initial_task();
                        task.type != thread_pool_task_type::shutdown;
                        task = sharedThis->next_task())
                    {
                        switch (task.type)
                        {
                        case thread_pool_task_type::execute:
                            task.operation();
                            break;

                            // Ignore if the type is something else
                        default:
                            break;
                        }
                    }
                });

                // Threads start out as waiting
                ++this->_waitingThreads;
                ++this->_threadCount;

                this->_threads.emplace(thread.get_id(), std::move(thread));
            }

            void ensure_thread_count()
            {
                assert_locked();
                assert(this->_minThreads <= this->_maxThreads); // UB if not

                // We can be in one of three scenarios: either we have too few threads, too many threads, or an
                // acceptable number of threads executing.
                if (this->_threadCount < this->_minThreads)
                {
                    // This is the first case from above
                    while (this->_threadCount < this->_minThreads)
                    {
                        // We're eagerly creating threads, meaning we have no work for them to perform when they start
                        create_thread();
                    }
                }
                else
                {
                    // This is either the second or third case from above. This can mean that either we're good and
                    // don't need to shutdown any of our threads, we exceed the max number of allowed threads, or we
                    // have too many threads waiting to execute. Note that the second and third scenarios aren't
                    // necessarily mutually exclusive. For example, we may be one thread over our max number of allowed
                    // threads, but five over our max number of allowed waiting threads, in which case we'll want to
                    // shut down five threads. The opposite is of course also possible where we are five over the max
                    // total allowance, but only one over the max waiting allowance, in which case we'll still want to
                    // shut down five threads.
                    auto excessThreads = (this->_threadCount > this->_maxThreads) ?
                        (this->_threadCount - this->_maxThreads) : 0;
                    auto excessWaiting = (this->_waitingThreads > this->_maxWaitingThreads) ?
                        (this->_waitingThreads - this->_maxWaitingThreads) : 0;

                    // We can't shut down more than the min number of threads
                    excessWaiting = std::min(excessWaiting, this->_threadCount - this->_minThreads);

                    // We want to shut down whatever the maximum is. That said, we can't just shut down in-progress
                    // threads. Instead, we have to wait for them to stop processing their current task, at which point
                    // they will automatically shutdown when they notice that we are over one of our quotas. That said,
                    // we may have waiting threads which are immediately eiligible for termination, so notify them if
                    // possible.
                    auto notifyCount = std::min(std::max(excessThreads, excessWaiting), this->_waitingThreads);
                    for (size_t i = 0; i < notifyCount; ++i)
                    {
                        this->_taskAvailable.notify_one();
                    }
                }
            }

            thread_pool_task initial_task()
            {
                std::unique_lock<std::mutex> lock(this->_mutex);

                // Tasks start out as waiting, so we _don't_ want to increment the waiting count
                auto stopWaitingOnExit = make_scope_guard([&]()
                {
                    --this->_waitingThreads;
                });

                return get_task(lock);
            }

            thread_pool_task next_task()
            {
                std::unique_lock<std::mutex> lock(this->_mutex);

                ++this->_waitingThreads;
                auto stopWaitingOnExit = make_scope_guard([&]()
                {
                    --this->_waitingThreads;
                });

                return get_task(lock);
            }

            thread_pool_task get_task(std::unique_lock<std::mutex>& lock)
            {
                assert_locked();
                assert(this->_waitingThreads > 0); // Should have been incremented before calling

                while (true)
                {
                    if (shutdown_thread())
                    {
                        // Assume that the thread is going to shut down. We want to know this now rather than later so
                        // that we don't accidentally shut down a different thread, or worse, not create a new thread
                        // because we think that an existing thread will be able to service the task only for it to be
                        // in the process of shutting down
                        --this->_threadCount;
                        return thread_pool_task{ thread_pool_task_type::shutdown };
                    }
                    else if (!this->_taskList.empty())
                    {
                        return pop_task();
                    }

                    this->_taskAvailable.wait(lock);
                }
            }

            bool shutdown_thread() const
            {
                assert_locked();

                // Even if the thread pool has been shut down, we let all queued up tasks complete, so only shut down if
                // there are no more tasks to execute
                if (!this->_running && this->_taskList.empty())
                {
                    return true;
                }

                // If we're over our thread quota, then we need to shut the caller down
                if (this->_threadCount > this->_maxThreads)
                {
                    return true;
                }

                // Otherwise, we need to be careful. The only other condition that would cause us to shut down the
                // caller is if the number of waiting threads is over the allowed limit. However, we want to avoid doing
                // this if either (1) we are at our minimum allowed number of threads, or (2) have available tasks to
                // execute, since that will cause the thread to no longer be waiting
                if (this->_taskList.empty() &&
                    (this->_threadCount > this->_minThreads) &&
                    (this->_waitingThreads > this->_maxWaitingThreads))
                {
                    return true;
                }

                return false;
            }

            thread_pool_task pop_task()
            {
                assert_locked();
                assert(!this->_taskList.empty());

                // If there are no high or normal priority tasks in the queue, make sure that their end iterators remain
                // valid since we are about to remove the first element
                if (this->_highPriorityEnd == std::begin(this->_taskList))
                {
                    ++this->_highPriorityEnd;
                }
                if (this->_normalPriorityEnd == std::begin(this->_taskList))
                {
                    ++this->_normalPriorityEnd;
                }

                auto value = std::move(this->_taskList.front().second);
                this->_taskList.pop_front();
                return value;
            }

            void emplace_task(thread_pool_priority priority, std::function<void(void)>&& func)
            {
                assert_locked();
                assert(this->_running);

                // First do the insertion. This will be used to update end iterators if necessary
                task_list::iterator insertPos;
                switch (priority)
                {
                case thread_pool_priority::high:
                    insertPos = this->_highPriorityEnd;
                    break;

                case thread_pool_priority::normal:
                    insertPos = this->_normalPriorityEnd;
                    break;

                case thread_pool_priority::low:
                default:
                    insertPos = std::end(this->_taskList);
                    break;
                }

                auto pos = this->_taskList.emplace(
                    insertPos,
                    priority,
                    thread_pool_task{ thread_pool_task_type::execute, std::move(func) });

                switch (priority)
                {
                    // If there are no normal priority tasks in the queue, we need to make sure that our normal priority
                    // end iterator is updated to stay correct
                case thread_pool_priority::low:
                default:
                    if (this->_normalPriorityEnd == insertPos)
                    {
                        this->_normalPriorityEnd = pos;
                    }
                    [[fallthrough]];

                    // Similarly, if there are no high priority tasks in the queue, we need to make sure that our high
                    // priority end iterator is updated to stay correct
                case thread_pool_priority::normal:
                    if (this->_highPriorityEnd == insertPos)
                    {
                        this->_highPriorityEnd = pos;
                    }
                    [[fallthrough]];

                case thread_pool_priority::high:
                    break;
                }

                this->_taskAvailable.notify_one();
            }



            mutable std::mutex _mutex;
            std::condition_variable _taskAvailable;
            bool _running = true;

            std::unordered_map<std::thread::id, std::thread> _threads;
            size_t _threadCount = 0;

            // Min/max number of threads allowed
            size_t _minThreads;
            size_t _maxThreads;

            // Indicates the maximum number of threads that can be waiting for a task. If this value is reached, then
            // waiting threads are sent 'shutdown' events until the number of waiting threads is less than or equal to
            // this value
            size_t _maxWaitingThreads;
            size_t _waitingThreads = 0;

            // List of all tasks waiting to be claimed, sorted by priority. New tasks are added after all other tasks
            // with the same priority
            using task_list = std::list<std::pair<thread_pool_priority, details::thread_pool_task>>;
            task_list _taskList;

            // For quicker insertion/removal, we maintain knowledge of where priority changes occur in the task list
            task_list::iterator _highPriorityEnd = std::end(_taskList);
            task_list::iterator _normalPriorityEnd = std::end(_taskList);

            CreationBehavior _creationBehavior;
        };



        /*
         * apply_set_value
         *
         * We can't invoke `std::promise<Ty>::set_value` with the result of a void-returning function, so we use this
         * helper to handle such cases and either call the function first and then call the no-argument
         * `std::promise<Ty>set_value` function (for the void case), or call `std::promise<Ty>set_value` with the result
         * of the function (for all other cases).
         *
         * This variant of the function accepts a `std::tuple` and invokes the function via `std::apply`.
         */
        template <typename Ty, typename Func, typename TupleTy, std::enable_if_t<std::is_void_v<Ty>, int> = 0>
        void apply_set_value(std::promise<Ty>& promise, Func&& func, TupleTy&& tuple)
        {
            static_assert(std::is_void_v<decltype(std::apply(std::forward<Func>(func), std::forward<TupleTy>(tuple)))>,
                "Function return type expected to be void");

            std::apply(std::forward<Func>(func), std::forward<TupleTy>(tuple));
            promise.set_value();
        }

        template <typename Ty, typename Func, typename TupleTy, std::enable_if_t<!std::is_void_v<Ty>, int> = 0>
        void apply_set_value(std::promise<Ty>& promise, Func&& func, TupleTy&& tuple)
        {
            promise.set_value(std::apply(std::forward<Func>(func), std::forward<TupleTy>(tuple)));
        }
    }



    /*
     * basic_thread_pool
     */
    template <typename Traits = default_thread_pool_traits>
    class basic_thread_pool
    {
        using impl = details::thread_pool_impl<typename Traits::creation_behavior>;

    public:
        /*
         * Constructor(s)/Destructor
         */
        basic_thread_pool() :
            _impl(std::make_shared<impl>(
                Traits::initial_min_threads(),
                Traits::initial_max_threads(),
                Traits::initial_max_available_threads()))
        {
            this->_impl->start();
        }

        // Can't copy of course
        basic_thread_pool(const basic_thread_pool&) = delete;
        basic_thread_pool& operator=(const basic_thread_pool&) = delete;

        // Move is okay
        basic_thread_pool(basic_thread_pool&&) = default;
        basic_thread_pool& operator=(basic_thread_pool&&) = default;



        /*
         * Information
         */
        size_t count() const
        {
            return this->_impl->count();
        }



        /*
         * Shutdown
         */
        void join()
        {
            // Shuts down the thread pool, but allows all running and queued tasks to complete, blocking execution until
            // all tasks are complete and all threads termiante
            this->_impl->join();
        }

        void detach()
        {
            // Shuts down the thread pool, but allows all running and queued tasks to complete in the background. I.e.
            // does _not_ block execution.
            this->_impl->detach();
        }



        /*
         * Task Submission
         */
#pragma region Task Submission

        template <typename Func>
        void submit(Func&& func)
        {
            submit(thread_pool_priority::normal, std::forward<Func>(func));
        }

        template <typename Func, typename... Args>
        void submit(Func&& func, Args&&... args)
        {
            submit(thread_pool_priority::normal, std::forward<Func>(func), std::forward<Args>(args)...);
        }

        template <typename Func>
        void submit(thread_pool_priority priority, Func&& func)
        {
            this->_impl->submit(priority, std::forward<Func>(func));
        }

        template <typename Func, typename... Args>
        void submit(thread_pool_priority priority, Func&& func, Args&&... args)
        {
            submit(priority,
                [
                    func = std::forward<Func>(func),
                    args = std::make_tuple(std::forward<Args>(args)...)
                ]() mutable
            {
                std::apply(std::move(func), std::move(args));
            });
        }

        template <typename Func, typename... Args>
        auto submit_for_result(Func&& func, Args&&... args) ->
            std::future<std::result_of_t<std::decay_t<Func>(std::decay_t<Args>...)>>
        {
            return submit_for_result(
                thread_pool_priority::normal,
                std::forward<Func>(func),
                std::forward<Args>(args)...);
        }

        template <typename Func, typename... Args>
        auto submit_for_result(thread_pool_priority priority, Func&& func, Args&&... args) ->
            std::future<std::result_of_t<std::decay_t<Func>(std::decay_t<Args>...)>>
        {
            using result_type = std::result_of_t<std::decay_t<Func>(std::decay_t<Args>...)>;
            std::promise<result_type> promise;
            auto future = promise.get_future();

            submit(priority, make_lambda_shared(
                [
                    promise = std::move(promise),
                    func = std::forward<Func>(func),
                    //args = std::forward<Args>(args)
                    args = std::make_tuple(std::forward<Args>(args)...)
                ]() mutable
            {
                // Let the caller be the one to handle any exceptions
                try
                {
                    details::apply_set_value(promise, std::move(func), std::move(args));
                }
                catch (...)
                {
                    // If set_exception throws, let it propagate down
                    promise.set_exception(std::current_exception());
                }
            }));

            return future;
        }

#pragma endregion



        /*
         * Configuration
         */
#pragma region Thread Pool Configuration

        size_t max_available_threads() const
        {
            return this->_impl->get_max_waiting_threads();
        }

        void set_max_available_threads(size_t value)
        {
            this->_impl->change_max_waiting_threads(value);
        }

        size_t max_threads() const
        {
            return this->_impl->get_max_threads();
        }

        void set_max_threads(size_t value)
        {
            this->_impl->change_max_threads(value);
        }

        size_t min_threads() const
        {
            return this->_impl->get_min_threads();
        }

        void set_min_threads(size_t value)
        {
            this->_impl->change_min_threads(value);
        }

#pragma endregion



    private:

        std::shared_ptr<impl> _impl;
    };



#pragma region Thread Pool Aliases

    using thread_pool = basic_thread_pool<>;
    using single_thread_thread_pool = basic_thread_pool<single_thread_thread_pool_traits>;

#pragma endregion
}

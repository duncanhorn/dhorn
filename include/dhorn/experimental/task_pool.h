/*
 * Duncan Horn
 *
 * task_pool.h
 *
 *
 */
#pragma once

#include <future>
#include <thread>
#include <type_traits>
#include <vector>

#include "message_queue.h"

namespace dhorn
{
    namespace experimental
    {
        /*
         * task_pool
         */
#pragma region task_pool

        class task_pool
        {
        public:

            /*
             * Constructor(s)/Destructor
             */
            task_pool(size_t threadCount) :
                _running(true)
            {
                for (size_t i = 0; i < threadCount; ++i)
                {
                    // It's assumed that the threadpool only exist as long as this object exists
                    this->_threadPool.emplace_back([this]()
                    {
                        while (this->_running)
                        {
                            this->_messageQueue.pop_front()();
                        }
                    });
                }
            }



            /*
             * Task Submit Functions
             */
            template <typename Func>
            void submit(const Func &func)
            {
                this->_messageQueue.push_back(func);
            }

            template <typename Func, typename ResultType = decltype(std::declval<Func>()())>
            auto submit_for_result(const Func &func) -> std::future<ResultType>
            {
                auto promise = std::make_shared<std::promise<ResultType>>();
                auto future = promise->get_future();

                this->_messageQueue.push_back([promise = std::move(promise), func]()
                {
                    promise->set_value(func());
                });

                return future;
            }

            void barrier(void)
            {
                // Blocks all threads in the thread pool until all have reached the barrier.
                size_t reached_barrier = 0;
                size_t passed_barrier = 0;
                std::mutex mutex;
                std::condition_variable allReached;
                std::condition_variable allPassed;

                { // Acquire _barrierLock
                    // Only one thread can execute barrier() at a time. Otherwise, we can very easliy get deadlock
                    std::lock_guard<std::mutex> guard(this->_barrierLock);

                    for (size_t i = 0; i < this->_threadPool.size(); ++i)
                    {
                        this->submit([&]()
                        {
                            std::unique_lock<std::mutex> lock(mutex);
                            ++reached_barrier;
                            if (reached_barrier == this->_threadPool.size())
                            {
                                allReached.notify_all();
                            }
                            else
                            {
                                allReached.wait(lock);
                            }

                            ++passed_barrier;
                            if (passed_barrier == this->_threadPool.size())
                            {
                                allPassed.notify_all();
                            }
                        });
                    }

                    // Wait for completion
                    { // Acquire mutex
                        std::unique_lock<std::mutex> lock(mutex);
                        allPassed.wait(lock, [&]() -> bool
                        {
                            return passed_barrier == this->_threadPool.size();
                        });
                    } // Release mutex
                } // Release _barrierLock
            }



            /*
             * Shutdown/Clean-Up Functions
             */
            void shutdown(void)
            {
                start_shutdown();
                join();
            }

            void start_shutdown(void)
            {
                // Since we cannot guarantee that all threads in the tread pool will encounter another task to wake them
                // up, we need to go ahead and do so now. Each thread will then read the value of _running and exit
                for (size_t i = 0; i < this->_threadPool.size(); ++i)
                {
                    this->_messageQueue.push_back([this]()
                    {
                        // We delay setting _running to false until the message is actually processed. This will guarantee
                        // that any outstanding work in the queue has at least been addressed before this point
                        this->_running = false;
                    });
                }
            }

            void join(void)
            {
                for (auto& thread : this->_threadPool)
                {
                    thread.join();
                }
            }



        private:

            std::vector<std::thread> _threadPool;
            message_queue<void(void)> _messageQueue;
            bool _running;

            std::mutex _barrierLock;
        };

#pragma endregion
    }
}

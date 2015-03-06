/*
 * task_pool.h
 *
 * Duncan Horn
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
        task_pool(_In_ size_t threadCount) :
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
        void submit(_In_ Func func)
        {
            this->_messageQueue.push_back(func);
        }

        template <typename Func>
        auto submit_for_result(_In_ Func func) ->
            std::future<decltype(std::declval<Func>()())>
        {
            using ResultType = decltype(std::declval<Func>()());

            //std::packaged_task<ResultType(void)> task(func);
            //auto future = task.get_future();

            //this->_messageQueue.push_back(std::move(task));

            //return future;

            std::promise<ResultType> promise;
            auto future = promise.get_future();

            this->_messageQueue.push_back([promise, func]()
            {
                promise.set_value(func());
            });

            return future;
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
    };

#pragma endregion
}

/*
 * Duncan Horn
 *
 * message_queue.h
 *
 * 
 */
#pragma once

#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>

#include "../scope_guard.h"

namespace dhorn
{
    namespace experimental
    {
        namespace details
        {
            /*
             * message_queue_types
             */
            template <typename FuncType>
            struct message_queue_types
            {
                using function_type = std::function<FuncType>;
            };



            /*
             * message_queue_node
             */
            template <typename Types>
            struct message_queue_node
            {
                using function_type = typename Types::function_type;

                function_type func;
                message_queue_node *next;

                /*
                 * Constructor(s)/Destructor
                 */
                message_queue_node(void) :
                    next(nullptr)
                {
                }

                message_queue_node(function_type func) :
                    func(std::move(func)),
                    next(nullptr)
                {
                }

                ~message_queue_node(void)
                {
                    // Recursive delete; NOP if nullptr
                    delete this->next;
                }
            };
        }



        /*
         * message_queue
         */
        template <typename FuncType>
        class message_queue final
        {
            using MyTypes = details::message_queue_types<FuncType>;
            using FunctionType = typename MyTypes::function_type;

        public:

            /*
             * Constructor(s)/Destructor
             */
            message_queue(void) :
                _size(0),
                _back(&this->_front)
            {
            }

            // Cannot copy
            message_queue(const message_queue &other) = delete;
            message_queue &operator=(const message_queue &other) = delete;


            /*
             * Public Functions
             */
            bool empty(void) const
            {
                return this->_size == 0;
            }

            size_t size(void) const
            {
                return this->_size;
            }

            void push_back(const FunctionType &func)
            {
                // Do all initialization work that does not need to occur under lock first
                auto pNode = new details::message_queue_node<MyTypes>(func);

                { // Acquire _backMutex
                    std::lock_guard<std::mutex> lock(this->_backMutex);

                    assert(!this->_back->next);
                    this->_back->next = pNode;
                    this->_back = pNode;
                } // Release _backMutex

                { // Acquire _sizeMutex
                    std::lock_guard<std::mutex> lock(this->_sizeMutex);

                    ++this->_size;
                    assert(this->_size);

                    // Check to see if we need to notify any other threads that the queue is no longer empty
                    if (this->_size == 1)
                    {
                        this->_notEmpty.notify_all();
                    }
                } // Release _sizeMutex
            }

            FunctionType pop_front(void)
            {
                { // Acquire _sizeMutex
                    std::unique_lock<std::mutex> lock(this->_sizeMutex);

                    while (!this->_size)
                    {
                        this->_notEmpty.wait(lock);
                    }

                    // Decrementing the count is equivalent to "reserving" an entry in the queue. It does not necessarily
                    // mean that this thread will get the first value, but that's not really all that important
                    --this->_size;
                } // Release _sizeMutex

                return this->PopFront();
            }

            bool try_pop_front(FunctionType &func)
            {
                { // Acquire _sizeMutex
                    std::unique_lock<std::mutex> lock(this->_sizeMutex);

                    if (!this->_size)
                    {
                        return false;
                    }

                    // Decrementing the count is equivalent to "reserving" an entry in the queue. It does not necessarily
                    // mean that this thread will get the first value, but that's not really all that important
                    --this->_size;
                } // Release _sizeMutex

                func = this->PopFront();
                return true;
            }



        private:

            FunctionType PopFront(void)
            {
                // !!! NOTE: _size must have been previously decremented !!!
                details::message_queue_node<MyTypes> *pNode;
                { // Acquire _frontMutex
                    std::lock_guard<std::mutex> lock(this->_frontMutex);
                    pNode = this->_front.next;
                    assert(pNode);

                    { // Acquire _backMutex
                        std::lock_guard<std::mutex> guard(this->_backMutex);

                        this->_front.next = pNode->next;
                        if (pNode == this->_back)
                        {
                            this->_back = &this->_front;
                        }
                    }// Release _backMutex
                } // Release _frontMutex

                auto destroy = make_scope_guard([&]()
                {
                    pNode->next = nullptr;
                    delete pNode;
                });

                return std::move(pNode->func);
            }

            details::message_queue_node<MyTypes> _front;
            details::message_queue_node<MyTypes> *_back;
            size_t _size;

            std::mutex _frontMutex;
            std::mutex _backMutex;
            std::mutex _sizeMutex;
            std::condition_variable _notEmpty;
        };
    }
}

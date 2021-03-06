/*
 * Duncan Horn
 *
 * worker.h
 */
#pragma once

#include <atomic>
#include <complex>
#include <mutex>
#include <thread>
#include <vector>

class worker final
{
    using ComplexType = std::complex<long double>;
    struct StorageType
    {
        ComplexType point;
        ComplexType value;
        std::size_t iterations;
        COLORREF color;

        StorageType(ComplexType point) :
            point(point),
            value(point),
            iterations{},
            color(RGB(0, 0, 0))
        {
        }
    };

    using DataType = std::vector<std::vector<StorageType>>;



public:
    worker(void);
    ~worker(void);

    void start(void);
    void exit(void);



private:

    void thread_proc(void);
    void synchronize_update(void);
    void update_size(void);

    dhorn::experimental::win32::callback_handler::result_type on_paint(
        dhorn::experimental::win32::window *pWindow,
        std::uintptr_t wparam,
        std::intptr_t lparam);

    dhorn::experimental::win32::callback_handler::result_type on_resize(
        dhorn::experimental::win32::window *pWindow,
        std::uintptr_t wparam,
        std::intptr_t lparam);

    dhorn::experimental::win32::callback_handler::result_type on_erase_background(
        dhorn::experimental::win32::window *pWindow,
        std::uintptr_t wparam,
        std::intptr_t lparam);

    dhorn::experimental::win32::callback_handler::result_type on_scrollwheel(
        dhorn::experimental::win32::window *pWindow,
        std::uintptr_t wparam,
        std::intptr_t lparam);

    // Internal data that keeps track of each current value. We use a std::shared_ptr on our data since it is possible
    // for us to post a paint request, receive a size update, and clear our buffer information before - or worse,
    // during - the paint
    ComplexType _topLeft;
    ComplexType _bottomRight;
    std::shared_ptr<DataType> _data;
    std::atomic_int _nextRow;
    std::size_t _iterations;
    std::size_t _iterationsPerUpdate;

    // Thread pooling information
    std::uint32_t _threadCount;
    std::vector<std::thread> _threads;

    // Thread synchronization
    std::mutex _monitor;
    std::size_t _threadsExecuting;
    std::condition_variable _updateReady;
    std::condition_variable _updateCompleted;
    bool _sizeUpdatePending;
    bool _running;

    // Graphics information
    HDC dc;
    std::mutex _drawMutex;
};

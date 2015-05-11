/*
 * Duncan Horn
 *
 * worker.cpp
 */

#include "precomp.h"

#include "globals.h"

using namespace dhorn::win32;

worker::worker(void) :
    _topLeft(-2, 1),
    _bottomRight(1, -1),
    _nextRow(0),
    _iterations(0),
    _iterationsPerUpdate(1),
    _threadCount(0),
    _threadsExecuting(0),
    _sizeUpdatePending(false),
    _running(false)
{
    // For now, we will spawn one worker thread for each logical processor. The thought is that the UI thread shouldn't
    // have all that much work to do. This can be tweaked later
    this->_threadCount = dhorn::win32::get_maximum_processor_count();
}

worker::~worker(void)
{
}

void worker::start(void)
{
    // Register callback handlers
    globals::window.add_callback_handler(window_message::paint, dhorn::bind_member_function(&worker::on_paint, this));
    globals::window.add_callback_handler(window_message::size, dhorn::bind_member_function(&worker::on_resize, this));

    // Initialize the number of threads executing since the update function assums that the thread was already
    // contributing to the execution count
    this->_threadsExecuting = this->_threadCount;

    this->update_size();

    // Finally, begin execution!
    this->_running = true;
    for (uint32_t i = 0; i < this->_threadCount; ++i)
    {
        this->_threads.emplace_back(dhorn::bind_member_function(&worker::thread_proc, this));
    }
}

void worker::exit(void)
{
    // Must join all threads since it is an error to let them destroy without being detached if they have not yet
    // completed execution
    for (auto &thread : this->_threads)
    {
        thread.join();
    }
    this->_threads.clear();
}

void worker::thread_proc(void)
{
    while (this->_running)
    {
        using PointType = std::pair<size_t, size_t>;

        size_t row_index = ++this->_nextRow;
        for (; row_index < this->_data->size(); row_index = ++this->_nextRow)
        {
            auto &row = (*this->_data)[row_index];
            for (auto &entry : row)
            {
                // Skip points that are already complete
                if (entry.iterations)
                {
                    continue;
                }

                for (size_t i = 0; i < this->_iterationsPerUpdate; ++i)
                {
                    entry.value = (entry.value * entry.value) + entry.point;
                    if (dhorn::length_squared(entry.value.imag(), entry.value.real()) >= 4)
                    {
                        entry.iterations = this->_iterations + i;
                        break;
                    }
                }
            }

            // Allow response to size update to occur earlier
            if (this->_sizeUpdatePending)
            {
                break;
            }
        }

        this->synchronize_update();
    }
}

void worker::synchronize_update(void)
{
    std::unique_lock<std::mutex> lock(this->_monitor);
    --this->_threadsExecuting;

    // If we are the last thread to begin waiting, then it is our responsibility to notify the update thread that
    // it can proceed. We also use this opportunity to reset the next row
    if (this->_threadsExecuting == 0)
    {
        // Check to see if there was a size change
        if (this->_sizeUpdatePending)
        {
            this->update_size();
            this->_sizeUpdatePending = false;
        }

        // Reset state for the next iteration
        this->_nextRow = 0;
        this->_iterations += this->_iterationsPerUpdate;
        this->_updateReady.notify_all();
        this->_running = globals::window.running();
    }
    else
    {
        // Wait for the size update to complete
        this->_updateReady.wait(lock);
    }

    ++this->_threadsExecuting;

    // Barrier until we know that all threads have reached this point
    if (this->_threadsExecuting != this->_threadCount)
    {
        this->_updateCompleted.wait(lock);
    }
    else
    {
        this->_updateCompleted.notify_all();
    }
}

void worker::update_size(void)
{
    auto size = globals::window.size();

    // We treat the indices as (row, column), so (x, y) is flipped
    this->_data = std::make_shared<DataType>(size.height);
    for (size_t i = 0; i < size.height; ++i)
    {
        auto &row = (*this->_data)[i];
        long double deltaY = (this->_topLeft.imag() - this->_bottomRight.imag()) * i / size.height;
        for (size_t j = 0; j < size.width; ++j)
        {
            long double deltaX = (this->_bottomRight.real() - this->_topLeft.real()) * j / size.width;
            ComplexType pt(this->_topLeft.real() + deltaX, this->_topLeft.imag() - deltaY);
            row.push_back(pt);
        }
    }
}

callback_handler::result_type worker::on_paint(
    _In_ window *pWindow,
    _In_ uintptr_t /*wparam*/,
    _In_ intptr_t /*lparam*/)
{
    // TODO
    (void)pWindow;
    return std::make_pair(false, 0);
}

callback_handler::result_type worker::on_resize(
    _In_ dhorn::win32::window * /*pWindow*/,
    _In_ uintptr_t /*wparam*/,
    _In_ intptr_t /*lparam*/)
{
    this->_sizeUpdatePending = true;
    return std::make_pair(false, 0);
}

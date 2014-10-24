/*
 * Duncan Horn
 *
 * worker.h
 */
#pragma once

#include <complex>
#include <vector>

class worker
{
    using StorageType = long double;
    using ComplexType = std::complex<StorageType>;

public:

    void initialize(_In_ dhorn::win32::window &window);



private:

    dhorn::win32::callback_handler::result_type on_paint(
        _In_ dhorn::win32::window *pWindow,
        _In_ uint32_t /*wparam*/,
        _In_ int32_t /*lparam*/);

    // Internal data that keeps track of each current value
    ComplexType _topLeft;
    StorageType _delta;
    std::vector<std::vector<ComplexType>> _data;

    // Graphics information
};

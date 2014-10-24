/*
 * Duncan Horn
 *
 * worker.cpp
 */

#include "precomp.h"

using namespace dhorn::win32;

void worker::initialize(_In_ window &window)
{
    // Register callback handlers
    window.add_callback_handler(window_message::paint,
        std::bind(&worker::on_paint, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

callback_handler::result_type worker::on_paint(
    _In_ window *pWindow,
    _In_ uintptr_t /*wparam*/,
    _In_ intptr_t /*lparam*/)
{
    // TODO
    return std::make_pair(false, 0);
}

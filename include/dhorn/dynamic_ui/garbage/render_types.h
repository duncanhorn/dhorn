/*
 * Duncan Horn
 *
 * render_types.h
 *
 * Types common to multiple dynamic_ui constructs
 */
#pragma once

#include "../../d3d/d3d_utils.h"

namespace dhorn
{
    namespace dynamic_ui
    {
        namespace garbage
        {
            struct render_state
            {
                win32::com_ptr<ID3D11Device> device;
                win32::com_ptr<ID3D11DeviceContext> device_context;
                win32::com_ptr<IDXGISwapChain> swap_chain;
                win32::com_ptr<ID3D11RenderTargetView> render_target_view;
                win32::com_ptr<ID3D11Texture2D> depth_stencil_buffer;
                win32::com_ptr<ID3D11DepthStencilView> depth_stencil_view;
            };
        }
    }
}

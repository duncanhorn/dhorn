/*
 * Duncan Horn
 *
 * render_types.h
 *
 * Types common to multiple dynamic_ui constructs
 */
#pragma once

#include <d3d11.h>

#include "../../com_ptr.h"
#include "../../d3d/d3d_utils.h"

namespace dhorn
{
    namespace dynamic_ui
    {
        namespace garbage
        {
            struct render_state
            {
                com_ptr<ID3D11Device> device;
                com_ptr<ID3D11DeviceContext> device_context;
                com_ptr<IDXGISwapChain> swap_chain;
                com_ptr<ID3D11RenderTargetView> render_target_view;
                com_ptr<ID3D11Texture2D> depth_stencil_buffer;
                com_ptr<ID3D11DepthStencilView> depth_stencil_view;
            };
        }
    }
}

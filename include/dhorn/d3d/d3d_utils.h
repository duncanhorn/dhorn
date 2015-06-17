/*
 * Duncan Horn
 *
 * d3d_utils.h
 *
 * Simple D3D utilities that don't take dependencies past what DirectX defines
 */
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

namespace dhorn
{
    namespace d3d
    {
        /*
         * Descriptor "constructors"
         */
#pragma region Descriptors

        inline DXGI_SWAP_CHAIN_DESC swap_chain_desc(
            _In_ UINT width,
            _In_ UINT height,
            _In_ HWND windowHandle,
            _In_ UINT sampleCount = 4,
            _In_ UINT sampleQuality = 1,
            _In_ bool windowed = true,
            _In_ DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
            _In_ UINT backBufferCount = 1)
        {
            DXGI_SWAP_CHAIN_DESC desc = {};
            desc.BufferDesc.Width = width;
            desc.BufferDesc.Height = height;
            desc.BufferDesc.RefreshRate.Numerator = 60;
            desc.BufferDesc.RefreshRate.Denominator = 1;
            desc.BufferDesc.Format = format;
            desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
            desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
            desc.SampleDesc.Count = sampleCount;
            desc.SampleDesc.Quality = sampleQuality;
            desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.BufferCount = backBufferCount;
            desc.OutputWindow = windowHandle;
            desc.Windowed = windowed;
            desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            return desc;
        }

        inline D3D11_TEXTURE2D_DESC texture2d_desc(
            _In_ UINT width,
            _In_ UINT height,
            _In_ DXGI_FORMAT format,
            _In_ UINT bindFlags,
            _In_ UINT sampleCount = 4,
            _In_ UINT sampleQuality = 1)
        {
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = format;
            desc.SampleDesc.Count = sampleCount;
            desc.SampleDesc.Quality = sampleQuality;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = bindFlags;

            return desc;
        }

        inline D3D11_TEXTURE2D_DESC depth_stencil_desc(
            _In_ UINT width,
            _In_ UINT height,
            _In_ UINT sampleCount = 4,
            _In_ UINT sampleQuality = 1,
            _In_ DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT)
        {
            return texture2d_desc(width, height, format, D3D11_BIND_DEPTH_STENCIL, sampleCount, sampleQuality);
        }

        inline D3D11_VIEWPORT view_port(
            _In_ float width,
            _In_ float height,
            _In_ float x = 0.0f,
            _In_ float y = 0.0f,
            _In_ float minDepth = 0.0f,
            _In_ float maxDepth = 1.0f)
        {
            D3D11_VIEWPORT viewPort = {};
            viewPort.Width = width;
            viewPort.Height = height;
            viewPort.TopLeftX = x;
            viewPort.TopLeftY = y;
            viewPort.MinDepth = minDepth;
            viewPort.MaxDepth = maxDepth;

            return viewPort;
        }

#pragma endregion



        /*
         * View/Projection
         */
#pragma region ViewProjection

        // NOTE: Assumes that forward, right, and up are all normal vectors orthogonal to each other
        inline DirectX::XMMATRIX XM_CALLCONV create_view_matrix(
            _In_ DirectX::FXMVECTOR right,
            _In_ DirectX::FXMVECTOR up,
            _In_ DirectX::FXMVECTOR forward,
            _In_ DirectX::GXMVECTOR position)
        {
            using namespace DirectX;

            // Calculate the dot products first
            float x = -XMVectorGetX(XMVector3Dot(right, position));
            float y = -XMVectorGetX(XMVector3Dot(up, position));
            float z = -XMVectorGetX(XMVector3Dot(forward, position));

            return XMMatrixSet(
                XMVectorGetX(right), XMVectorGetX(up), XMVectorGetX(forward), 0.0f,
                XMVectorGetY(right), XMVectorGetY(up), XMVectorGetY(forward), 0.0f,
                XMVectorGetZ(right), XMVectorGetZ(up), XMVectorGetZ(forward), 0.0f,
                x, y, z, 1.0f);
        }

#pragma endregion
    }
}

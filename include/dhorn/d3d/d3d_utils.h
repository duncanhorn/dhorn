/*
 * Duncan Horn
 *
 * d3d11_utils.h
 *
 * Simple D3D utilities that don't take dependencies past what DirectX defines
 */
#pragma once

#include <cstdint>
#include <DirectXMath.h>
#include <dxgi.h>
#include <fstream>

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
            _In_ UINT backBufferCount = 1,
            _In_ DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_DISCARD,
            _In_ DXGI_USAGE bufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT)
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
            desc.BufferUsage = bufferUsage;
            desc.BufferCount = backBufferCount;
            desc.OutputWindow = windowHandle;
            desc.Windowed = windowed;
            desc.SwapEffect = swapEffect;

            return desc;
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



        /*
         * Shaders
         */
#pragma region Shaders

        template <typename CharT>
        std::vector<uint8_t> read_shader_file(_In_ const CharT *path)
        {
            std::fstream fileStream(path, std::ios::in | std::ios::binary);
            if (fileStream.fail() || fileStream.bad())
            {
                throw win32::hresult_exception(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND));
            }

            // Get the size of the file
            fileStream.seekg(0, std::ios_base::end);
            size_t bufferSize = static_cast<size_t>(fileStream.tellg());
            fileStream.seekg(0, std::ios_base::beg);

            // Read the data
            std::vector<uint8_t> buffer(bufferSize);
            fileStream.read(reinterpret_cast<char *>(buffer.data()), bufferSize);
            fileStream.close();

            return buffer;
        }

#pragma endregion
    }
}

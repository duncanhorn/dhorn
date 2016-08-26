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
#include <dxgi1_2.h>
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
            UINT width,
            UINT height,
            HWND windowHandle,
            UINT sampleCount = 4,
            UINT sampleQuality = 1,
            bool windowed = true,
            DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
            UINT backBufferCount = 1,
            DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_DISCARD,
            DXGI_USAGE bufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT) noexcept
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

        inline DXGI_SWAP_CHAIN_DESC1 swap_chain_desc_1(
            UINT width,
            UINT height,
            DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
            UINT sampleCount = 1,
            UINT sampleQuality = 0,
            UINT bufferCount = 2,
            bool stereo = false,
            UINT flags = 0,
            DXGI_SCALING scaling = DXGI_SCALING_NONE,
            DXGI_ALPHA_MODE alphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
            DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_USAGE bufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT) noexcept
        {
            // If DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL is specified, then MSAA is not allowed
            assert(((sampleCount == 1) && (sampleQuality == 0)) || (swapEffect != DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL));

            // If DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL is specified, then BufferCount must be between 2 and 16
            assert(((bufferCount >= 2) && (bufferCount <= 16)) || (swapEffect != DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL));

            // If DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL is specified, then the format must be one of the values below
            assert((swapEffect != DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) ||
                (format == DXGI_FORMAT_R8G8B8A8_UNORM) ||
                (format == DXGI_FORMAT_B8G8R8A8_UNORM) ||
                (format == DXGI_FORMAT_R16G16B16A16_FLOAT));

            // Stereo is only supported for use with DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
            assert(!stereo || (swapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL));

            DXGI_SWAP_CHAIN_DESC1 desc = {};
            desc.Width = width;
            desc.Height = height;
            desc.Format = format;
            desc.Stereo = stereo;
            desc.SampleDesc.Count = sampleCount;
            desc.SampleDesc.Quality = sampleQuality;
            desc.BufferUsage = bufferUsage;
            desc.BufferCount = bufferCount;
            desc.Scaling = scaling;
            desc.SwapEffect = swapEffect;
            desc.AlphaMode = alphaMode;
            desc.Flags = flags;

            return desc;
        }

#pragma endregion



        /*
         * View/Projection
         */
#pragma region ViewProjection

        // NOTE: Assumes that forward, right, and up are all normal vectors orthogonal to each other
        inline DirectX::XMMATRIX XM_CALLCONV create_view_matrix(
            DirectX::FXMVECTOR right,
            DirectX::FXMVECTOR up,
            DirectX::FXMVECTOR forward,
            DirectX::GXMVECTOR position) noexcept
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
        std::vector<uint8_t> read_shader_file(const CharT *path)
        {
            std::fstream fileStream(path, std::ios::in | std::ios::binary);
            if (fileStream.fail() || fileStream.bad())
            {
                throw hresult_exception(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND));
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

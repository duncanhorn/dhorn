/*
 * Duncan Horn
 *
 * d3d_utils.h
 *
 * Simple D3D utilities that don't take dependencies past what DirectX defines
 */
#pragma once

#include <cstdint>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <vector>

#include "../type_traits.h"
#include "../windows/com_ptr.h"
#include "../windows/windows_exception.h"

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

        inline D3D11_INPUT_ELEMENT_DESC input_element_desc(
            _In_ DXGI_FORMAT format,
            _In_ UINT byteOffset,
            _In_ LPCSTR semanticName,
            _In_ UINT semanticIndex = 0,
            _In_ D3D11_INPUT_CLASSIFICATION inputClassification = D3D11_INPUT_PER_VERTEX_DATA)
        {
            D3D11_INPUT_ELEMENT_DESC desc = {};
            desc.SemanticName = semanticName;
            desc.SemanticIndex = semanticIndex;
            desc.Format = format;
            desc.AlignedByteOffset = byteOffset;
            desc.InputSlotClass = inputClassification;

            return desc;
        }

        template <typename Ty, typename Struct>
        inline D3D11_INPUT_ELEMENT_DESC input_element_desc(
            _In_ Ty Struct::*member,
            _In_ DXGI_FORMAT format,
            _In_ LPCSTR semanticName,
            _In_ UINT semanticIndex = 0,
            _In_ D3D11_INPUT_CLASSIFICATION inputClassification = D3D11_INPUT_PER_VERTEX_DATA)
        {
            return input_element_desc(
                format,
                byte_offset(member),
                semanticName,
                semanticIndex,
                inputClassification);
        }

        inline D3D11_RASTERIZER_DESC rasterizer_desc(
            _In_ D3D11_FILL_MODE fillMode,
            _In_ D3D11_CULL_MODE cullMode)
        {
            D3D11_RASTERIZER_DESC desc = {};
            desc.DepthClipEnable = true;
            desc.FillMode = fillMode;
            desc.CullMode = cullMode;

            return desc;
        }

        inline D3D11_BUFFER_DESC buffer_desc(
            _In_ UINT size,
            _In_ UINT bindFlags)
        {
            D3D11_BUFFER_DESC desc = {};
            desc.ByteWidth = size;
            desc.Usage = D3D11_USAGE_IMMUTABLE;
            desc.BindFlags = bindFlags;

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

        template <typename CharT>
        win32::com_ptr<ID3D11VertexShader> load_vertex_shader(
            _In_ ID3D11Device *device,
            _In_ const CharT *path,
            _Inout_ std::vector<uint8_t> &bytecode)
        {
            bytecode = read_shader_file(path);

            win32::com_ptr<ID3D11VertexShader> vertexShader;
            win32::throw_if_failed(device->CreateVertexShader(
                bytecode.data(),
                bytecode.size(),
                nullptr, // Class linkage
                &vertexShader));

            return vertexShader;
        }

        template <typename CharT>
        win32::com_ptr<ID3D11VertexShader> load_vertex_shader(_In_ ID3D11Device *device, _In_ const CharT *path)
        {
            std::vector<uint8_t> bytecode;
            return load_vertex_shader(device, path, bytecode);
        }

        template <typename CharT>
        static win32::com_ptr<ID3D11PixelShader> load_pixel_shader(_In_ ID3D11Device *device, _In_ const CharT *path)
        {
            auto bytecode = read_shader_file(path);

            win32::com_ptr<ID3D11PixelShader> pixelShader;
            win32::throw_if_failed(device->CreatePixelShader(
                bytecode.data(),
                bytecode.size(),
                nullptr, // Class linkage
                &pixelShader));

            return pixelShader;
        }

#pragma endregion



        /*
         * Buffers/Geometry
         */
#pragma region Buffers/Geometry

        template <typename Ty>
        inline win32::com_ptr<ID3D11Buffer> create_buffer(
            _In_ ID3D11Device *device,
            _In_ const Ty *bufferData,
            _In_ size_t length,
            _In_ UINT bindFlags)
        {
            D3D11_BUFFER_DESC desc = buffer_desc(length * sizeof(Ty), bindFlags);
            D3D11_SUBRESOURCE_DATA data = { bufferData };

            win32::com_ptr<ID3D11Buffer> buffer;
            win32::throw_if_failed(device->CreateBuffer(&desc, &data, &buffer));

            return buffer;
        }

        template <typename Ty>
        inline win32::com_ptr<ID3D11Buffer> create_buffer(
            _In_ ID3D11Device *device,
            _In_ const std::vector<Ty> &data,
            _In_ UINT bindFlags)
        {
            return create_buffer(device, data.data(), data.size(), bindFlags);
        }

        template <typename Ty, size_t Size>
        inline win32::com_ptr<ID3D11Buffer> create_buffer(
            _In_ ID3D11Device *device,
            _In_ const Ty(&data)[Size],
            _In_ UINT bindFlags)
        {
            return create_buffer(device, data, Size, bindFlags);
        }

        template <typename Ty>
        inline win32::com_ptr<ID3D11Buffer> create_constant_buffer(_In_ ID3D11Device *device)
        {
            auto desc = buffer_desc(sizeof(Ty), D3D11_BIND_CONSTANT_BUFFER);
            desc.Usage = D3D11_USAGE_DEFAULT;

            win32::com_ptr<ID3D11Buffer> buffer;
            win32::throw_if_failed(device->CreateBuffer(&desc, nullptr, &buffer));

            return buffer;
        }

#pragma endregion
    }
}

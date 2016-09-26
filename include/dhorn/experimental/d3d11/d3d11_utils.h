/*
 * Duncan Horn
 *
 * d3d11_utils.h
 *
 * Simple D3D11 utilities that don't take dependencies past what DirectX defines
 */
#pragma once

#include <cstdint>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <vector>

#include "../com_ptr.h"
#include "../d3d/d3d_utils.h"
#include "../../type_traits.h"
#include "../windows_exception.h"

namespace dhorn
{
    namespace experimental
    {
        namespace d3d11
        {
            /*
             * Descriptor "constructors"
             */
#pragma region Descriptors

            inline D3D11_TEXTURE2D_DESC texture2d_desc(
                UINT width,
                UINT height,
                DXGI_FORMAT format,
                UINT bindFlags,
                UINT sampleCount = 4,
                UINT sampleQuality = 1)
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
                UINT width,
                UINT height,
                UINT sampleCount = 4,
                UINT sampleQuality = 1,
                DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT)
            {
                return texture2d_desc(width, height, format, D3D11_BIND_DEPTH_STENCIL, sampleCount, sampleQuality);
            }

            inline D3D11_VIEWPORT view_port(
                float width,
                float height,
                float x = 0.0f,
                float y = 0.0f,
                float minDepth = 0.0f,
                float maxDepth = 1.0f)
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
                DXGI_FORMAT format,
                UINT byteOffset,
                LPCSTR semanticName,
                UINT semanticIndex = 0,
                D3D11_INPUT_CLASSIFICATION inputClassification = D3D11_INPUT_PER_VERTEX_DATA)
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
                Ty Struct::*member,
                DXGI_FORMAT format,
                LPCSTR semanticName,
                UINT semanticIndex = 0,
                D3D11_INPUT_CLASSIFICATION inputClassification = D3D11_INPUT_PER_VERTEX_DATA)
            {
                return input_element_desc(
                    format,
                    byte_offset(member),
                    semanticName,
                    semanticIndex,
                    inputClassification);
            }

            inline D3D11_RASTERIZER_DESC rasterizer_desc(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode)
            {
                D3D11_RASTERIZER_DESC desc = {};
                desc.DepthClipEnable = true;
                desc.FillMode = fillMode;
                desc.CullMode = cullMode;

                return desc;
            }

            inline D3D11_BUFFER_DESC buffer_desc(UINT size, UINT bindFlags)
            {
                D3D11_BUFFER_DESC desc = {};
                desc.ByteWidth = size;
                desc.Usage = D3D11_USAGE_IMMUTABLE;
                desc.BindFlags = bindFlags;

                return desc;
            }

#pragma endregion



            /*
             * Shaders
             */
#pragma region Shaders

            template <typename CharT>
            com_ptr<ID3D11VertexShader> load_vertex_shader(
                ID3D11Device *device,
                const CharT *path,
                std::vector<uint8_t> &bytecode)
            {
                bytecode = d3d::read_shader_file(path);

                com_ptr<ID3D11VertexShader> vertexShader;
                throw_if_failed(device->CreateVertexShader(
                    bytecode.data(),
                    bytecode.size(),
                    nullptr, // Class linkage
                    &vertexShader));

                return vertexShader;
            }

            template <typename CharT>
            com_ptr<ID3D11VertexShader> load_vertex_shader(ID3D11Device *device, const CharT *path)
            {
                std::vector<uint8_t> bytecode;
                return load_vertex_shader(device, path, bytecode);
            }

            template <typename CharT>
            static com_ptr<ID3D11PixelShader> load_pixel_shader(ID3D11Device *device, const CharT *path)
            {
                auto bytecode = d3d::read_shader_file(path);

                com_ptr<ID3D11PixelShader> pixelShader;
                throw_if_failed(device->CreatePixelShader(
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
            inline com_ptr<ID3D11Buffer> create_buffer(
                ID3D11Device *device,
                const Ty *bufferData,
                size_t length,
                UINT bindFlags)
            {
                D3D11_BUFFER_DESC desc = buffer_desc(length * sizeof(Ty), bindFlags);
                D3D11_SUBRESOURCE_DATA data = { bufferData };

                com_ptr<ID3D11Buffer> buffer;
                throw_if_failed(device->CreateBuffer(&desc, &data, &buffer));

                return buffer;
            }

            template <typename Ty>
            inline com_ptr<ID3D11Buffer> create_buffer(ID3D11Device *device, const std::vector<Ty> &data, UINT bindFlags)
            {
                return create_buffer(device, data.data(), data.size(), bindFlags);
            }

            template <typename Ty, size_t Size>
            inline com_ptr<ID3D11Buffer> create_buffer(ID3D11Device *device, const Ty(&data)[Size], UINT bindFlags)
            {
                return create_buffer(device, data, Size, bindFlags);
            }

            template <typename Ty>
            inline com_ptr<ID3D11Buffer> create_constant_buffer(ID3D11Device *device)
            {
                auto desc = buffer_desc(sizeof(Ty), D3D11_BIND_CONSTANT_BUFFER);
                desc.Usage = D3D11_USAGE_DEFAULT;

                com_ptr<ID3D11Buffer> buffer;
                throw_if_failed(device->CreateBuffer(&desc, nullptr, &buffer));

                return buffer;
            }

#pragma endregion
        }
    }
}

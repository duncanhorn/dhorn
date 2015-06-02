/*
 * Duncan Horn
 *
 * d3d_window.h
 *
 * 
 */
#pragma once

#include <d3d11.h>

#include "..\windows\com_ptr.h"
#include "..\windows\window.h"

namespace dhorn
{
    namespace d3d
    {
        /*
         * d3d_window
         */
        class d3d_window :
            public dhorn::win32::window
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            d3d_window(
                _In_opt_ IDXGIAdapter *adapter = nullptr,
                _In_ D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE,
                _In_opt_ HMODULE software = nullptr,
                _In_ UINT deviceFlags = 0,
                _In_reads_opt_(featureLevelSize) const D3D_FEATURE_LEVEL *featureLevels = nullptr,
                _In_ UINT featureLevelSize = 0,
                _In_ UINT sdkVersion = D3D11_SDK_VERSION)
            {
                // Various other functions rely on the presence of a device
                win32::throw_if_failed(::D3D11CreateDevice(
                    adapter,
                    driverType,
                    software,
                    deviceFlags,
                    featureLevels,
                    featureLevelSize,
                    sdkVersion,
                    &this->_device,
                    &this->_featureLevel,
                    &this->_deviceContext));
            }



            /*
             * Public Function(s)
             */
#pragma region Public Functions

            D3D_FEATURE_LEVEL feature_level(void) const
            {
                return this->_featureLevel;
            }

            ID3D11Device *device(void) const
            {
                // Note to callers: this does not AddRef. Therefore, if you call this function, you must have a strong
                // reference to this d3d_window instance to ensure that the object is not destroyed as the result of
                // a call to the destructor.
                return this->_device;
            }

            ID3D11DeviceContext *context(void) const
            {
                // Note to callers: this does not AddRef. Therefore, if you call this function, you must have a strong
                // reference to this d3d_window instance to ensure that the object is not destroyed as the result of
                // a call to the destructor.
                return this->_deviceContext;
            }

            UINT query_msaa_quality_levels(_In_ DXGI_FORMAT format, _In_ UINT sampleCount) const
            {
                // Treat failures as non-fatal; just report that the queried sample count is not supported
                UINT result;
                if (FAILED(this->_device->CheckMultisampleQualityLevels(format, sampleCount, &result)))
                {
                    result = 0;
                }

                return result;
            }

#pragma endregion



        private:

            void CreateSwapChain(_In_ DXGI_SWAP_CHAIN_DESC &desc)
            {
                assert(this->_device);
                assert(!this->_swapChain);

                win32::com_ptr<IDXGIDevice> device = this->_device;
                win32::com_ptr<IDXGIAdapter> adapter;
                win32::com_ptr<IDXGIFactory> factory;
                win32::throw_if_failed(device->GetParent(IID_PPV_ARGS(&adapter)));
                win32::throw_if_failed(adapter->GetParent(IID_PPV_ARGS(&factory)));
                win32::throw_if_failed(factory->CreateSwapChain(this->_device, &desc, &this->_swapChain));
            }

            void CreateRenderTargetView(_In_opt_ const D3D11_RENDER_TARGET_VIEW_DESC *desc)
            {
                assert(!this->_renderTargetView);

                win32::com_ptr<ID3D11Texture2D> backBuffer;
                win32::throw_if_failed(this->_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
                win32::throw_if_failed(this->_device->CreateRenderTargetView(
                    backBuffer,
                    desc,
                    &this->_renderTargetView));
            }

            void CreateDepthStencil(_In_ const D3D11_TEXTURE2D_DESC &desc, _In_opt_ const D3D11_SUBRESOURCE_DATA *data)
            {
                assert(!this->_depthStencilBuffer);
                win32::throw_if_failed(this->_device->CreateTexture2D(&desc, data, &this->_depthStencilBuffer));
            }

            void CreateDepthStencilView(_In_opt_ const D3D11_DEPTH_STENCIL_VIEW_DESC *desc)
            {
                assert(this->_depthStencilBuffer);
                assert(!this->_depthStencilView);
                win32::throw_if_failed(this->_device->CreateDepthStencilView(
                    this->_depthStencilBuffer,
                    desc,
                    &this->_depthStencilView));
            }

            void OnResize(void)
            {
                // TODO
            }



            // DirectX Interface Pointers
            D3D_FEATURE_LEVEL _featureLevel;
            win32::com_ptr<ID3D11Device> _device;
            win32::com_ptr<ID3D11DeviceContext> _deviceContext;
            win32::com_ptr<IDXGISwapChain> _swapChain;
            win32::com_ptr<ID3D11RenderTargetView> _renderTargetView;
            win32::com_ptr<ID3D11Texture2D> _depthStencilBuffer;
            win32::com_ptr<ID3D11DepthStencilView> _depthStencilView;
        };
    }
}

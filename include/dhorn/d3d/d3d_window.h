/*
 * Duncan Horn
 *
 * d3d_window.h
 *
 * 
 */
#pragma once

#include "d3d_utils.h"
#include "../functional.h"
#include "../unique_any.h"
#include "../windows/com_ptr.h"
#include "../windows/window.h"

namespace dhorn
{
    namespace d3d
    {
        /*
         * d3d_window_traits - the default traits type for the basic_d3d_window class
         */
        struct d3d_window_traits
        {
            static const DXGI_FORMAT swap_chain_format = DXGI_FORMAT_R8G8B8A8_UNORM;
            static const DXGI_FORMAT depth_stencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            static const UINT back_buffer_count = 1;
        };



        /*
         * basic_d3d_window
         */
        template <typename Traits>
        class basic_d3d_window :
            public dhorn::win32::window
        {
            using QualityFunc = std::function<UINT(_In_ ID3D11Device *, _Inout_ UINT *)>;

        public:
            /*
             * Constructor(s)/Destructor
             */
            basic_d3d_window() :
                _sampleCount(4)
            {
                this->add_callback_handler(
                    win32::window_message::enter_size_move,
                    bind_member_function(&basic_d3d_window::on_enter_size_move, this));
                this->add_callback_handler(
                    win32::window_message::exit_size_move,
                    bind_member_function(&basic_d3d_window::on_exit_size_move, this));
                this->add_callback_handler(
                    win32::window_message::size,
                    bind_member_function(&basic_d3d_window::on_resize, this));
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



            /*
             * Custom initialization values
             */
            void set_msaa_quality_callback(_In_ QualityFunc func)
            {
                this->_qualityFunc = std::move(func);
            }

#pragma endregion



        protected:

            virtual void initialize(void)
            {
                // Let our base class do any initialization it needs first
                window::initialize();

                // All other initialization functions rely on the existence of the device and device context
                this->CreateDevice();

                // By default, use the size of the window client area
                auto size = this->size();

                // Check MSAA support
                if (this->_qualityFunc)
                {
                    this->_sampleQuality = this->_qualityFunc(this->_device, &this->_sampleCount);
                }
                else
                {
                    win32::throw_if_failed(this->_device->CheckMultisampleQualityLevels(
                        Traits::swap_chain_format,
                        this->_sampleCount,
                        &this->_sampleQuality));
                    win32::throw_hr_if_false(this->_sampleQuality > 0, E_INVALIDARG);

                    --this->_sampleQuality;
                }

                // Perform each initialization operation, giving callers a chance to overload each one
                this->CreateSwapChain(size);
                this->CreateRenderTargetView();
                this->CreateDepthStencil(size);
                this->CreateDepthStencilView();

                // Bind the views
                this->_deviceContext->OMSetRenderTargets(
                    1, // Number of render target views
                    this->_renderTargetView.get_address_of(),
                    this->_depthStencilView);

                this->SetViewports(size); // Note that this is meaningless if we set it inside of Render()
            }

            virtual void CreateDevice(void)
            {
                UINT deviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
                deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

                win32::throw_if_failed(::D3D11CreateDevice(
                    nullptr,                    // IDXGIAdapter
                    D3D_DRIVER_TYPE_HARDWARE,
                    nullptr,                    // HMODULE
                    deviceFlags,
                    nullptr, 0,                 // D3D_FEATURE_LEVEL array
                    D3D11_SDK_VERSION,
                    &this->_device,
                    &this->_featureLevel,
                    &this->_deviceContext));

                // D3D11 not supported
                win32::throw_hr_if_false(this->_featureLevel == D3D_FEATURE_LEVEL_11_0, E_NOTIMPL);
            }

            virtual void CreateSwapChain(_In_ const dhorn::rect<size_t> &size)
            {
                assert(this->_device);
                assert(!this->_swapChain);

                DXGI_SWAP_CHAIN_DESC desc = swap_chain_desc(
                    static_cast<UINT>(size.width),
                    static_cast<UINT>(size.height),
                    this->handle(),
                    this->_sampleCount,
                    this->_sampleQuality,
                    true, // Windowed
                    Traits::swap_chain_format,
                    Traits::back_buffer_count);

                win32::com_ptr<IDXGIDevice> device = this->_device;
                win32::com_ptr<IDXGIAdapter> adapter;
                win32::com_ptr<IDXGIFactory> factory;
                win32::throw_if_failed(device->GetParent(IID_PPV_ARGS(&adapter)));
                win32::throw_if_failed(adapter->GetParent(IID_PPV_ARGS(&factory)));
                win32::throw_if_failed(factory->CreateSwapChain(this->_device, &desc, &this->_swapChain));
            }

            virtual void CreateRenderTargetView()
            {
                assert(!this->_renderTargetView);

                win32::com_ptr<ID3D11Texture2D> backBuffer;
                win32::throw_if_failed(this->_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
                win32::throw_if_failed(this->_device->CreateRenderTargetView(
                    backBuffer,
                    nullptr,                    // D3D11_RENDER_TARGET_VIEW_DESC
                    &this->_renderTargetView));
            }

            virtual void CreateDepthStencil(_In_ const dhorn::rect<size_t> &size)
            {
                assert(!this->_depthStencilBuffer);

                D3D11_TEXTURE2D_DESC desc = depth_stencil_desc(
                    static_cast<UINT>(size.width),
                    static_cast<UINT>(size.height),
                    this->_sampleCount,
                    this->_sampleQuality,
                    Traits::depth_stencil_format);

                win32::throw_if_failed(this->_device->CreateTexture2D(
                    &desc,
                    nullptr,                    // D3D11_SUBRESOURCE_DATA
                    &this->_depthStencilBuffer));
            }

            virtual void CreateDepthStencilView()
            {
                assert(this->_depthStencilBuffer);
                assert(!this->_depthStencilView);
                win32::throw_if_failed(this->_device->CreateDepthStencilView(
                    this->_depthStencilBuffer,
                    nullptr,                    // D3D11_DEPTH_STENCIL_VIEW_DESC
                    &this->_depthStencilView));
            }

            virtual void SetViewports(_In_ const dhorn::rect<size_t> &size)
            {
                // By default, create one view port that is the size of the window
                D3D11_VIEWPORT viewPort = view_port(static_cast<UINT>(size.width), static_cast<UINT>(height));
                this->_deviceContext->RSSetViewports(1, &viewPort);
            }



            /*
             * Custom Message Pump
             */
            virtual uintptr_t message_pump()
            {
                // We use peek_message so that we can update/draw the world when not handling messages
                MSG msg{};
                while (static_cast<win32::window_message>(msg.message) != win32::window_message::quit)
                {
                    if (win32::peek_message(nullptr, 0, 0, true, &msg))
                    {
                        win32::translate_message(msg);
                        win32::dispatch_message(msg);
                    }
                    else
                    {
                        // TODO: Update/draw
                    }
                }

                return msg.wParam;
            }



            /*
             * Custom message handling
             */
            virtual win32::callback_handler::result_type on_enter_size_move(
                _In_ win32::window * /*pWindow*/,
                _In_ uintptr_t wparam,
                _In_ intptr_t lparam)
            {
                // TODO
                wparam;lparam;
                return std::make_pair(false, 0);
            }

            virtual win32::callback_handler::result_type on_exit_size_move(
                _In_ win32::window * /*pWindow*/,
                _In_ uintptr_t wparam,
                _In_ intptr_t lparam)
            {
                // TODO
                wparam;lparam;
                return std::make_pair(false, 0);
            }

            virtual win32::callback_handler::result_type on_resize(
                _In_ win32::window * /*pWindow*/,
                _In_ uintptr_t wparam,
                _In_ intptr_t lparam)
            {
                // TODO
                wparam;lparam;
                return std::make_pair(false, 0);
            }



        private:

            // DirectX Interface Pointers
            D3D_FEATURE_LEVEL _featureLevel;
            win32::com_ptr<ID3D11Device> _device;
            win32::com_ptr<ID3D11DeviceContext> _deviceContext;
            win32::com_ptr<IDXGISwapChain> _swapChain;
            win32::com_ptr<ID3D11RenderTargetView> _renderTargetView;
            win32::com_ptr<ID3D11Texture2D> _depthStencilBuffer;
            win32::com_ptr<ID3D11DepthStencilView> _depthStencilView;

            // Data that can be set so that clients need not derive from this class unless actually needed
            QualityFunc _qualityFunc;
            UINT _sampleCount;
            UINT _sampleQuality;
        };



        /*
         * basic_d3d_window type definitions
         */
        using d3d_window = basic_d3d_window<d3d_window_traits>;
    }
}

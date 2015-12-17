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
            static const UINT sample_count = 4;
#if defined(DEBUG) || defined(_DEBUG)
            static const UINT device_flags = D3D11_CREATE_DEVICE_DEBUG;
#else
            static const UINT device_flags = 0;
#endif
        };



        /*
         * basic_d3d_window
         */
        template <typename Traits>
        class basic_d3d_window :
            public dhorn::win32::window
        {
            using QualityFunc = std::function<UINT(_In_ ID3D11Device *, _Inout_ UINT *)>;
            using UpdateFunc = std::function<void(void)>;
            using DrawFunc = std::function<void(ID3D11Device *, ID3D11DeviceContext *)>;
            using SizeChangeFunc = std::function<void(const rect<size_t> &)>;

        public:
            /*
             * Constructor(s)/Destructor
             */
            basic_d3d_window() :
                _sampleCount(Traits::sample_count),
                _previousClientArea{},
                _resizing(false)
            {
                this->add_callback_handler(
                    win32::window_message::enter_size_move,
                    bind_member_function(&basic_d3d_window::on_enter_size_move, this));
                this->add_callback_handler(
                    win32::window_message::exit_size_move,
                    bind_member_function(&basic_d3d_window::on_exit_size_move, this));
                this->add_callback_handler(
                    win32::window_message::size,
                    bind_member_function(&basic_d3d_window::on_size_change, this));

                DirectX::XMStoreFloat4(&this->_backgroundColor, colors::cornflower_blue);
            }



            /*
             * Public Function(s)
             */
#pragma region Public Functions

            D3D_FEATURE_LEVEL feature_level(void) const
            {
                return this->_featureLevel;
            }

            void XM_CALLCONV set_background(_In_ DirectX::FXMVECTOR color)
            {
                DirectX::XMStoreFloat4(&this->_backgroundColor, color);
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
            void msaa_quality_callback(_In_ const QualityFunc &func)
            {
                this->_qualityFunc = func;
            }

            void on_update(_In_ const UpdateFunc &func)
            {
                this->_updateFunc = func;
            }

            void on_draw(_In_ const DrawFunc &func)
            {
                this->_drawFunc = func;
            }

            void on_resize(_In_ const SizeChangeFunc &func)
            {
                this->_sizeChangeFunc = func;
            }

#pragma endregion



        protected:

            /*
             * Initialization
             */
            virtual void initialize(void)
            {
                // Let our base class do any initialization it needs first
                window::initialize();

                // All other initialization functions rely on the existence of the device and device context
                this->create_device();

                // By default, use the size of the window client area
                auto clientArea = this->client_rect();

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
                this->create_swap_chain(clientArea);
                this->resize(clientArea);
            }

            virtual void create_device(void)
            {
                win32::throw_if_failed(::D3D11CreateDevice(
                    nullptr,                    // IDXGIAdapter
                    D3D_DRIVER_TYPE_HARDWARE,
                    nullptr,                    // HMODULE
                    Traits::device_flags,
                    nullptr, 0,                 // D3D_FEATURE_LEVEL array
                    D3D11_SDK_VERSION,
                    &this->_device,
                    &this->_featureLevel,
                    &this->_deviceContext));

                // D3D11 not supported
                win32::throw_hr_if_false(this->_featureLevel == D3D_FEATURE_LEVEL_11_0, E_NOTIMPL);
            }

            virtual void create_swap_chain(_In_ const dhorn::rect<size_t> &size)
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

            virtual void create_render_target_view()
            {
                assert(!this->_renderTargetView);

                win32::com_ptr<ID3D11Texture2D> backBuffer;
                win32::throw_if_failed(this->_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
                win32::throw_if_failed(this->_device->CreateRenderTargetView(
                    backBuffer,
                    nullptr,                    // D3D11_RENDER_TARGET_VIEW_DESC
                    &this->_renderTargetView));
            }

            virtual void create_depth_stencil(_In_ const dhorn::rect<size_t> &size)
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

            virtual void create_depth_stencil_view()
            {
                assert(this->_depthStencilBuffer);
                assert(!this->_depthStencilView);
                win32::throw_if_failed(this->_device->CreateDepthStencilView(
                    this->_depthStencilBuffer,
                    nullptr,                    // D3D11_DEPTH_STENCIL_VIEW_DESC
                    &this->_depthStencilView));
            }

            virtual void set_viewports(_In_ const dhorn::rect<size_t> &size)
            {
                // By default, create one view port that is the size of the window
                D3D11_VIEWPORT viewPort = view_port(static_cast<float>(size.width), static_cast<float>(size.height));
                this->_deviceContext->RSSetViewports(1, &viewPort);
            }

            virtual void resize(_In_ const rect<size_t> &clientArea)
            {
                // We don't care if only the coordinates change
                if ((this->_previousClientArea.width == clientArea.width) &&
                    (this->_previousClientArea.height == clientArea.height))
                {
                    return;
                }

                // Reset our state
                this->_renderTargetView = nullptr;
                this->_depthStencilBuffer = nullptr;
                this->_depthStencilView = nullptr;

                // Resize the swap chain
                this->_swapChain->ResizeBuffers(
                    1,
                    static_cast<UINT>(clientArea.width),
                    static_cast<UINT>(clientArea.height),
                    Traits::swap_chain_format,
                    0);

                this->create_render_target_view();
                this->create_depth_stencil(clientArea);
                this->create_depth_stencil_view();

                // Bind the views
                this->_deviceContext->OMSetRenderTargets(
                    1, // Number of render target views
                    this->_renderTargetView.get_address_of(),
                    this->_depthStencilView);

                this->set_viewports(clientArea);

                this->_previousClientArea = clientArea;
                if (this->_sizeChangeFunc)
                {
                    this->_sizeChangeFunc(clientArea);
                }
            }



            /*
             * Update/render loop
             */
            virtual void update(void)
            {
                if (this->_updateFunc)
                {
                    this->_updateFunc();
                }
            }

            virtual void render(void)
            {
                this->_deviceContext->ClearRenderTargetView(this->_renderTargetView, &this->_backgroundColor.x);
                this->_deviceContext->ClearDepthStencilView(
                    this->_depthStencilView,
                    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                    1.0f,
                    0);

                if (this->_drawFunc)
                {
                    this->_drawFunc(this->_device, this->_deviceContext);
                }

                win32::throw_if_failed(this->_swapChain->Present(0, 0));
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
                        this->update();
                        this->render();
                    }
                }

                return msg.wParam;
            }



            /*
             * Custom message handling
             */
            virtual win32::callback_handler::result_type on_enter_size_move(
                _In_ win32::window * /*sender*/,
                _In_ uintptr_t /*wparam*/,
                _In_ intptr_t /*lparam*/)
            {
                this->_resizing = true;
                return std::make_pair(true, 0);
            }

            virtual win32::callback_handler::result_type on_exit_size_move(
                _In_ win32::window * /*sender*/,
                _In_ uintptr_t /*wparam*/,
                _In_ intptr_t /*lparam*/)
            {
                this->_resizing = false;
                this->resize(this->client_rect());

                return std::make_pair(true, 0);
            }

            virtual win32::callback_handler::result_type on_size_change(
                _In_ win32::window * /*sender*/,
                _In_ uintptr_t wparam,
                _In_ intptr_t lparam)
            {
                rect<size_t> clientArea = { 0, 0, LOWORD(lparam), HIWORD(lparam) };

                if (this->_device)
                {
                    switch (wparam)
                    {
                    case SIZE_MAXIMIZED:
                    case SIZE_RESTORED:
                        if (!this->_resizing)
                        {
                            this->resize(clientArea);
                        }
                        break;
                    }
                }

                return std::make_pair(true, 0);
            }



            /*
             * Protected members
             */

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
            UpdateFunc _updateFunc;
            DrawFunc _drawFunc;
            SizeChangeFunc _sizeChangeFunc;
            UINT _sampleCount;
            UINT _sampleQuality;

            DirectX::XMFLOAT4 _backgroundColor;

            // Window state information
            rect<size_t> _previousClientArea;
            bool _resizing;
        };



        /*
         * basic_d3d_window type definitions
         */
        using d3d_window = basic_d3d_window<d3d_window_traits>;
    }
}

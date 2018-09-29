/*
 * Duncan Horn
 *
 * d3d11_window.h
 *
 *
 */
#pragma once

#pragma comment(lib, "d3d11.lib")

#include "../../com/com_ptr.h"

#include "../d3d/colors.h"
#include "../functional.h"
#include "../unique_any.h"
#include "../windows/window.h"
#include "d3d11_utils.h"

namespace dhorn
{
    namespace experimental
    {
        namespace d3d11
        {
            /*
             * d3d11_window_traits - the default traits type for the basic_d3d11_window class
             */
            struct d3d11_window_traits
            {
                static const DXGI_FORMAT swap_chain_format = DXGI_FORMAT_R8G8B8A8_UNORM;
                static const DXGI_FORMAT depth_stencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
                static const UINT back_buffer_count = 1;
                static const UINT sample_count = 4;
#if defined(DEBUG) || defined(_DEBUG)
                static const UINT device_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;
#else
                static const UINT device_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif
            };



            /*
             * basic_d3d11_window
             */
            template <typename Traits>
            class basic_d3d11_window :
                public win32::window
            {
                using QualityFunc = std::function<UINT(ID3D11Device *, UINT *)>;
                using UpdateFunc = std::function<void(void)>;
                using DrawFunc = std::function<void(ID3D11Device *, ID3D11DeviceContext *)>;
                using SizeChangeFunc = std::function<void(const rect<std::size_t> &)>;

            public:
                /*
                 * Constructor(s)/Destructor
                 */
                basic_d3d11_window() :
                    _sampleCount(Traits::sample_count),
                    _previousClientArea{},
                    _resizing(false)
                {
                    this->add_callback_handler(
                        win32::window_message::enter_size_move,
                        bind_member_function(&basic_d3d11_window::on_enter_size_move, this));
                    this->add_callback_handler(
                        win32::window_message::exit_size_move,
                        bind_member_function(&basic_d3d11_window::on_exit_size_move, this));
                    this->add_callback_handler(
                        win32::window_message::size,
                        bind_member_function(&basic_d3d11_window::on_size_change, this));

                    DirectX::XMStoreFloat4(&this->_backgroundColor, d3d::colors::cornflower_blue);
                }



                /*
                 * Public Function(s)
                 */
#pragma region Public Functions

                D3D_FEATURE_LEVEL feature_level(void) const
                {
                    return this->_featureLevel;
                }

                void XM_CALLCONV set_background(DirectX::FXMVECTOR color)
                {
                    DirectX::XMStoreFloat4(&this->_backgroundColor, color);
                }

                ID3D11Device *device(void) const
                {
                    // Note to callers: this does not AddRef. Therefore, if you call this function, you must have a strong
                    // reference to this d3d11_window instance to ensure that the object is not destroyed as the result of
                    // a call to the destructor.
                    return this->_device.get();
                }

                ID3D11DeviceContext *context(void) const
                {
                    // Note to callers: this does not AddRef. Therefore, if you call this function, you must have a strong
                    // reference to this d3d11_window instance to ensure that the object is not destroyed as the result of
                    // a call to the destructor.
                    return this->_deviceContext.get();
                }



                /*
                 * Custom initialization values
                 */
                void msaa_quality_callback(const QualityFunc &func)
                {
                    this->_qualityFunc = func;
                }

                void on_update(const UpdateFunc &func)
                {
                    this->_updateFunc = func;
                }

                void on_draw(const DrawFunc &func)
                {
                    this->_drawFunc = func;
                }

                void on_resize(const SizeChangeFunc &func)
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
                        this->_sampleQuality = this->_qualityFunc(this->_device.get(), &this->_sampleCount);
                    }
                    else
                    {
                        com::check_hresult(this->_device->CheckMultisampleQualityLevels(
                            Traits::swap_chain_format,
                            this->_sampleCount,
                            &this->_sampleQuality));
                        if (this->_sampleQuality <= 0)
                        {
                            com::throw_hresult(E_INVALIDARG);
                        }

                        --this->_sampleQuality;
                    }

                    // Perform each initialization operation, giving callers a chance to overload each one
                    this->create_swap_chain(clientArea);
                    this->resize(clientArea);
                }

                virtual void create_device(void)
                {
                    com::check_hresult(::D3D11CreateDevice(
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
                    if (this->_featureLevel != D3D_FEATURE_LEVEL_11_0)
                    {
                        com::throw_hresult(E_NOTIMPL);
                    }
                }

                virtual void create_swap_chain(const rect<std::size_t> &size)
                {
                    assert(this->_device);
                    assert(!this->_swapChain);

                    DXGI_SWAP_CHAIN_DESC desc = d3d::swap_chain_desc(
                        static_cast<UINT>(size.width),
                        static_cast<UINT>(size.height),
                        this->handle(),
                        this->_sampleCount,
                        this->_sampleQuality,
                        true, // Windowed
                        Traits::swap_chain_format,
                        Traits::back_buffer_count);

                    // !TEMPORARY! 'template as' should be unnecessary, but needed here thanks to bug in Clang
                    auto device = this->_device.template as<IDXGIDevice>();
                    com::com_ptr<IDXGIAdapter> adapter;
                    com::com_ptr<IDXGIFactory> factory;
                    com::check_hresult(device->GetParent(IID_PPV_ARGS(&adapter)));
                    com::check_hresult(adapter->GetParent(IID_PPV_ARGS(&factory)));
                    com::check_hresult(factory->CreateSwapChain(this->_device.get(), &desc, &this->_swapChain));
                }

                virtual void create_render_target_view()
                {
                    assert(!this->_renderTargetView);

                    com::com_ptr<ID3D11Texture2D> backBuffer;
                    com::check_hresult(this->_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
                    com::check_hresult(this->_device->CreateRenderTargetView(
                        backBuffer.get(),
                        nullptr,                    // D3D11_RENDER_TARGET_VIEW_DESC
                        &this->_renderTargetView));
                }

                virtual void create_depth_stencil(const rect<std::size_t> &size)
                {
                    assert(!this->_depthStencilBuffer);

                    D3D11_TEXTURE2D_DESC desc = depth_stencil_desc(
                        static_cast<UINT>(size.width),
                        static_cast<UINT>(size.height),
                        this->_sampleCount,
                        this->_sampleQuality,
                        Traits::depth_stencil_format);

                    com::check_hresult(this->_device->CreateTexture2D(
                        &desc,
                        nullptr,                    // D3D11_SUBRESOURCE_DATA
                        &this->_depthStencilBuffer));
                }

                virtual void create_depth_stencil_view()
                {
                    assert(this->_depthStencilBuffer);
                    assert(!this->_depthStencilView);
                    com::check_hresult(this->_device->CreateDepthStencilView(
                        this->_depthStencilBuffer.get(),
                        nullptr,                    // D3D11_DEPTH_STENCIL_VIEW_DESC
                        &this->_depthStencilView));
                }

                virtual void set_viewports(const rect<std::size_t> &size)
                {
                    // By default, create one view port that is the size of the window
                    D3D11_VIEWPORT viewPort = view_port(static_cast<float>(size.width), static_cast<float>(size.height));
                    this->_deviceContext->RSSetViewports(1, &viewPort);
                }

                virtual void resize(const rect<std::size_t> &clientArea)
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
                        this->_renderTargetView.address_of(),
                        this->_depthStencilView.get());

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
                    this->_deviceContext->ClearRenderTargetView(
                        this->_renderTargetView.get(),
                        &this->_backgroundColor.x);
                    this->_deviceContext->ClearDepthStencilView(
                        this->_depthStencilView.get(),
                        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                        1.0f,
                        0);

                    if (this->_drawFunc)
                    {
                        this->_drawFunc(this->_device.get(), this->_deviceContext.get());
                    }

                    com::check_hresult(this->_swapChain->Present(0, 0));
                }



                /*
                 * Custom Message Pump
                 */
                virtual std::uintptr_t message_pump()
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
                    win32::window * /*sender*/,
                    std::uintptr_t /*wparam*/,
                    std::intptr_t /*lparam*/)
                {
                    this->_resizing = true;
                    return std::make_pair(true, 0);
                }

                virtual win32::callback_handler::result_type on_exit_size_move(
                    win32::window * /*sender*/,
                    std::uintptr_t /*wparam*/,
                    std::intptr_t /*lparam*/)
                {
                    this->_resizing = false;
                    this->resize(this->client_rect());

                    return std::make_pair(true, 0);
                }

                virtual win32::callback_handler::result_type on_size_change(
                    win32::window * /*sender*/,
                    std::uintptr_t wparam,
                    std::intptr_t lparam)
                {
                    rect<std::size_t> clientArea = { 0, 0, LOWORD(lparam), HIWORD(lparam) };

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
                com::com_ptr<ID3D11Device> _device;
                com::com_ptr<ID3D11DeviceContext> _deviceContext;
                com::com_ptr<IDXGISwapChain> _swapChain;
                com::com_ptr<ID3D11RenderTargetView> _renderTargetView;
                com::com_ptr<ID3D11Texture2D> _depthStencilBuffer;
                com::com_ptr<ID3D11DepthStencilView> _depthStencilView;

                // Data that can be set so that clients need not derive from this class unless actually needed
                QualityFunc _qualityFunc;
                UpdateFunc _updateFunc;
                DrawFunc _drawFunc;
                SizeChangeFunc _sizeChangeFunc;
                UINT _sampleCount;
                UINT _sampleQuality;

                DirectX::XMFLOAT4 _backgroundColor;

                // Window state information
                rect<std::size_t> _previousClientArea;
                bool _resizing;
            };



            /*
             * basic_d3d11_window type definitions
             */
            using d3d11_window = basic_d3d11_window<d3d11_window_traits>;
        }
    }
}

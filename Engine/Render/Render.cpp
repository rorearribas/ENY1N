#include "Render.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Scenes/Scene.h"
#include "Libs/ImGui/imgui_impl_win32.h"
#include "Libs/ImGui/imgui_impl_dx11.h"
#include "Engine/Base/Engine.h"

namespace render
{
  namespace internal_render
  {
    static const float s_fMinDepth(0.0f);
    static const float s_fMaxDepth(1.0f);
  }
  // ------------------------------------
  CRender::CRender(UINT32 _uX, UINT32 _uY)
  {
    // Create render window
    m_pRenderWindow = new render::CRenderWindow(_uX, _uY);
    // Init render
    HRESULT hr = Init(_uX, _uY);
    assert(!FAILED(hr));
  }
  // ------------------------------------
  CRender::~CRender()
  {
    // Delete global resources
    global::dx11::SafeRelease(global::dx11::s_pDevice);
    global::dx11::SafeRelease(global::dx11::s_pDeviceContext);

    // Delete internal resources
    global::dx11::SafeRelease(m_oRenderingResources.m_pSwapChain);
    global::dx11::SafeRelease(m_oRenderingResources.m_pRenderTargetView);
    global::dx11::SafeRelease(m_oRenderingResources.m_pDepthStencilTexture);
    global::dx11::SafeRelease(m_oRenderingResources.m_pDepthStencilState);
    global::dx11::SafeRelease(m_oRenderingResources.m_pDepthStencilView);
    global::dx11::SafeRelease(m_oRenderingResources.m_pRasterizerState);
  }
// ------------------------------------
  HRESULT CRender::Init(UINT32 _uX, UINT32 _uY)
  {
    // Create deviec
    HRESULT hr = CreateDevice(_uX, _uY);
    if (FAILED(hr)) return hr;

    // Init ImGui
    if (!InitImGui()) return -1;

    // Setup basic pipeline
    hr = InitBasicPipeline(_uX, _uY);
    if (FAILED(hr)) return hr;

    // Set delegate
    utils::CDelegate<void(UINT32, UINT32)> oResizeDelegate(&CRender::OnWindowResizeEvent, this);
    global::delegates::s_vctWindowsResizeDelegates.push_back(oResizeDelegate);

    return S_OK;
  }
  // ------------------------------------
  HRESULT CRender::InitBasicPipeline(UINT32 _uX, UINT32 _uY)
  {
    // Configure viewport
    ConfigureViewport(_uX, _uY);

    // Create new render target
    HRESULT hr = CreateRenderTargetView();
    assert(!FAILED(hr));

    // Create a new depth stencil
    hr = CreateDepthStencilView(_uX, _uY);
    assert(!FAILED(hr));

    // Create rasterizer
    hr = CreateRasterizerState();
    assert(!FAILED(hr));

    // Update scissor
    SetScissorRect(_uX, _uY);
    
    return hr;
  }
// ------------------------------------
  void CRender::SetScissorRect(UINT32 _uX, UINT32 _uY)
  {
    D3D11_RECT oScissorRect = {};
    oScissorRect.left = 0;
    oScissorRect.top = 0;
    oScissorRect.right = static_cast<LONG>(_uX);
    oScissorRect.bottom = static_cast<LONG>(_uY);
    global::dx11::s_pDeviceContext->RSSetScissorRects(1, &oScissorRect);
  }
  // ------------------------------------
  bool CRender::InitImGui()
  {
    if (!IMGUI_CHECKVERSION()) 
      return false;

    if (!ImGui::CreateContext()) 
      return false;

    if (!ImGui_ImplWin32_Init(m_pRenderWindow->GetHwnd())) 
      return false;

    if (!ImGui_ImplDX11_Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext)) 
      return false;

    ImGui::StyleColorsDark();
    return true;
  }
  // ------------------------------------
  HRESULT CRender::CreateDevice(UINT32 _uX, UINT32 _uY)
  {
    DXGI_SWAP_CHAIN_DESC oSwapChainDescriptor = {};
    oSwapChainDescriptor.BufferCount = 1;
    oSwapChainDescriptor.BufferDesc.Width = _uX;
    oSwapChainDescriptor.BufferDesc.Height = _uY;
    oSwapChainDescriptor.OutputWindow = global::window::s_oHwnd;
    oSwapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    oSwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    oSwapChainDescriptor.SampleDesc.Count = 1;
    oSwapChainDescriptor.SampleDesc.Quality = 0;
    oSwapChainDescriptor.Windowed = TRUE;

    D3D_FEATURE_LEVEL vctFeatureLevels[] =
    {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3,
      D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1
    };
    D3D_FEATURE_LEVEL oFeatureLevel;
    UINT uNumFeatureLevels = ARRAYSIZE(vctFeatureLevels);

    // Device and swapchain
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, vctFeatureLevels,
    uNumFeatureLevels, D3D11_SDK_VERSION, &oSwapChainDescriptor, &m_oRenderingResources.m_pSwapChain,
    &global::dx11::s_pDevice, &oFeatureLevel, &global::dx11::s_pDeviceContext);
    if (FAILED(hr)) return hr;

    return S_OK;
  }
  // ------------------------------------
  void CRender::OnWindowResizeEvent(UINT32 _uX, UINT32 _uY)
  {
    // Remove current target view
    global::dx11::SafeRelease(m_oRenderingResources.m_pRenderTargetView);

    // Resize buffers
    HRESULT hr = m_oRenderingResources.m_pSwapChain->ResizeBuffers(0, _uX, _uY, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    assert(!FAILED(hr));

    // Init basic pipeline
    hr = InitBasicPipeline(_uX, _uY);
    assert(!FAILED(hr));
  }
  // ------------------------------------
  HRESULT CRender::CreateRenderTargetView()
  {
    ID3D11Texture2D* pBackBuffer = nullptr;
    m_oRenderingResources.m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&pBackBuffer));
    if (!pBackBuffer) return -1;

    HRESULT hr = global::dx11::s_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_oRenderingResources.m_pRenderTargetView);
    if (FAILED(hr)) return hr;

    pBackBuffer->Release();
    return hr;
  }
  // ------------------------------------
  HRESULT CRender::CreateDepthStencilView(UINT32 _uX, UINT32 _uY)
  {
    // Release resources
    global::dx11::SafeRelease(m_oRenderingResources.m_pDepthStencilTexture);
    global::dx11::SafeRelease(m_oRenderingResources.m_pDepthStencilState);
    global::dx11::SafeRelease(m_oRenderingResources.m_pDepthStencilView);

    // Create texture
    D3D11_TEXTURE2D_DESC oTextureDesc = {};
    oTextureDesc.Width = _uX;
    oTextureDesc.Height = _uY;
    oTextureDesc.MipLevels = 1;
    oTextureDesc.ArraySize = 1;
    oTextureDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    oTextureDesc.SampleDesc.Count = 1;
    oTextureDesc.SampleDesc.Quality = 0;
    oTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    oTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    oTextureDesc.CPUAccessFlags = 0;
    oTextureDesc.MiscFlags = 0;
    HRESULT hr = global::dx11::s_pDevice->CreateTexture2D(&oTextureDesc, nullptr, &m_oRenderingResources.m_pDepthStencilTexture);
    assert(!FAILED(hr));

    // Depth test parameters
    D3D11_DEPTH_STENCIL_DESC oDepthStencilDesc = {};
    oDepthStencilDesc.DepthEnable = true;
    oDepthStencilDesc.StencilEnable = true;
    oDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    oDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    oDepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    oDepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    // Front-face
    oDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    oDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    oDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    oDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Back-face
    oDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    oDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    oDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    oDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    hr = global::dx11::s_pDevice->CreateDepthStencilState(&oDepthStencilDesc, &m_oRenderingResources.m_pDepthStencilState);

    D3D11_DEPTH_STENCIL_VIEW_DESC oDepthStencilViewDesc = {};
    oDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    oDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    oDepthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view
    return global::dx11::s_pDevice->CreateDepthStencilView
    (
      m_oRenderingResources.m_pDepthStencilTexture,
      &oDepthStencilViewDesc,
      &m_oRenderingResources.m_pDepthStencilView
    );
  }
  // ------------------------------------
  HRESULT CRender::CreateRasterizerState(D3D11_FILL_MODE _eFillMode)
  {
    global::dx11::SafeRelease(m_oRenderingResources.m_pRasterizerState);

    D3D11_RASTERIZER_DESC oRasterizerConfig = {};
    oRasterizerConfig.FillMode = _eFillMode;
    oRasterizerConfig.CullMode = D3D11_CULL_BACK;
    oRasterizerConfig.FrontCounterClockwise = false;
    oRasterizerConfig.DepthBias = false;
    oRasterizerConfig.DepthBiasClamp = 0;
    oRasterizerConfig.SlopeScaledDepthBias = 0;
    oRasterizerConfig.DepthClipEnable = true;
    oRasterizerConfig.ScissorEnable = true;
    oRasterizerConfig.MultisampleEnable = false;
    oRasterizerConfig.AntialiasedLineEnable = false;

    return global::dx11::s_pDevice->CreateRasterizerState(&oRasterizerConfig, &m_oRenderingResources.m_pRasterizerState);
  }
  // ------------------------------------
  void CRender::ConfigureViewport(UINT32 _uX, UINT32 _uY)
  {
    if (global::dx11::s_pDeviceContext && m_pRenderWindow)
    {
      D3D11_VIEWPORT oD3D11Viewport = {};
      oD3D11Viewport.Width = (FLOAT)(_uX);
      oD3D11Viewport.Height = (FLOAT)(_uY);
      oD3D11Viewport.MinDepth = internal_render::s_fMinDepth;
      oD3D11Viewport.MaxDepth = internal_render::s_fMaxDepth;

      global::dx11::s_pDeviceContext->RSSetViewports(1, &oD3D11Viewport);
    }
  }
  // ------------------------------------
  void CRender::BeginDraw()
  {
    // Clear resources
    float background_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    global::dx11::s_pDeviceContext->ClearRenderTargetView(m_oRenderingResources.m_pRenderTargetView, background_color);
    global::dx11::s_pDeviceContext->ClearDepthStencilView(m_oRenderingResources.m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Prepare imgui new frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
  }
  // ------------------------------------
  void CRender::EndDraw()
  {
    if (ImGui::Button("Show wireframe"))
    {
      CreateRasterizerState(D3D11_FILL_WIREFRAME);
    }
    if (ImGui::Button("Show lit"))
    {
      CreateRasterizerState(D3D11_FILL_SOLID);
    }

    // Update resources
    global::dx11::s_pDeviceContext->OMSetRenderTargets(1, &m_oRenderingResources.m_pRenderTargetView, m_oRenderingResources.m_pDepthStencilView);
    global::dx11::s_pDeviceContext->OMSetDepthStencilState(m_oRenderingResources.m_pDepthStencilState, 1);
    global::dx11::s_pDeviceContext->RSSetState(m_oRenderingResources.m_pRasterizerState);

    // End imgui draw
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Draw the current frame
    m_oRenderingResources.m_pSwapChain->Present(m_bVerticalSync, 0);
  }
}
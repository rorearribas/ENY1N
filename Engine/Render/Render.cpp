#include "Render.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Scenes/Scene.h"
#include "Libs/ImGui/imgui_impl_win32.h"
#include "Libs/ImGui/imgui_impl_dx11.h"
#include "Engine/Base/Engine.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/ImGui/ImGuizmo.h"

// Shaders
#include "Engine/Shaders/Standard/ForwardPixelShader.h"
#include "Engine/Shaders/Standard/ForwardVertexShader.h"

namespace render
{
  namespace internal_render
  {
    static const wchar_t* s_sPrepareFrameMrk(L"Begin Frame");
    static const wchar_t* s_sDrawModelsMrk(L"Models");
    static const wchar_t* s_sDrawPrimitivesMrk(L"Primitives");
    static const wchar_t* s_sImGuiMarker(L"ImGui");
    
    static const float s_v4ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static const float s_fMinDepth(0.0f);
    static const float s_fMaxDepth(1.0f);

    struct SRenderPipeline
    {
      // Swap chain + render target
      IDXGISwapChain* pSwapChain = nullptr;
      ID3D11RenderTargetView* pRenderTarget = nullptr;

      // Depth
      ID3D11Texture2D* pDepthTexture = nullptr;
      ID3D11DepthStencilState* pDepthStencilState = nullptr;
      ID3D11DepthStencilView* pDepthStencilView = nullptr;

      // Rasterizer
      ID3D11RasterizerState* pRasterizer = nullptr;

      // Blending
      ID3D11BlendState* pBlendState = nullptr;

      // Debug
      ID3DUserDefinedAnnotation* pUserMarker = nullptr;

      // Shaders for 3D Pipeline
      shader::CShader<shader::EShaderType::PIXEL_SHADER>* pPixelShader;
      shader::CShader<shader::EShaderType::VERTEX_SHADER>* pVertexShader;
    };

    static SRenderPipeline s_oPipeline;
  }
  // ------------------------------------
  CRender::CRender(uint32_t _uX, uint32_t _uY)
  {
    // Create render window
    m_pRenderWindow = new render::CRenderWindow(_uX, _uY);

    // Init render
    HRESULT hResult = Init(_uX, _uY);
    UNUSED_VAR(hResult);
    assert(!FAILED(hResult));
  }
  // ------------------------------------
  CRender::~CRender()
  {
    // Delete global resources
    global::dx11::SafeRelease(global::dx11::s_pDevice);
    global::dx11::SafeRelease(global::dx11::s_pDeviceContext);

    // Delete internal resources
    global::dx11::SafeRelease(internal_render::s_oPipeline.pSwapChain);
    global::dx11::SafeRelease(internal_render::s_oPipeline.pRenderTarget);
    global::dx11::SafeRelease(internal_render::s_oPipeline.pDepthTexture);
    global::dx11::SafeRelease(internal_render::s_oPipeline.pDepthStencilState);
    global::dx11::SafeRelease(internal_render::s_oPipeline.pDepthStencilView);
    global::dx11::SafeRelease(internal_render::s_oPipeline.pRasterizer);
    global::dx11::SafeRelease(internal_render::s_oPipeline.pBlendState);
    global::dx11::SafeRelease(internal_render::s_oPipeline.pUserMarker);

    // Release objetcs
    global::ReleaseObject(internal_render::s_oPipeline.pPixelShader);
    global::ReleaseObject(internal_render::s_oPipeline.pVertexShader);
  }
  // ------------------------------------
  HRESULT CRender::Init(uint32_t _uX, uint32_t _uY)
  {
    // Create device
    HRESULT hResult = CreateDevice(_uX, _uY);
    if (FAILED(hResult)) 
    {
      return hResult;
    }

    // Init ImGui
    if (!InitImGui()) 
    {
      return -1;
    }

    // Setup basic pipeline
    hResult = InitBasicPipeline(_uX, _uY);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Create shaders for 3D pipeline
    using namespace shader;
    internal_render::s_oPipeline.pPixelShader = new CShader<PIXEL_SHADER>(g_ForwardPixelShader, ARRAYSIZE(g_ForwardPixelShader));
    internal_render::s_oPipeline.pVertexShader = new CShader<VERTEX_SHADER>(g_ForwardVertexShader, ARRAYSIZE(g_ForwardVertexShader));

    // Set delegate
    utils::CDelegate<void(uint32_t, uint32_t)> oResizeDelegate(&CRender::OnWindowResizeEvent, this);
    global::delegates::s_vctOnWindowResizeDelegates.emplace_back(oResizeDelegate);

    // Push shaders
    internal_render::s_oPipeline.pPixelShader->PushShader();
    internal_render::s_oPipeline.pVertexShader->PushShader();

    // Get user def
    global::dx11::s_pDeviceContext->QueryInterface
    (
      __uuidof(ID3DUserDefinedAnnotation),
      reinterpret_cast<void**>(&internal_render::s_oPipeline.pUserMarker)
    );

    return hResult;
  }
  // ------------------------------------
  HRESULT CRender::InitBasicPipeline(uint32_t _uX, uint32_t _uY)
  {
    // Configure viewport
    ConfigureViewport(_uX, _uY);

    // Create new render target
    HRESULT hResult = CreateRenderTargetView();
    assert(!FAILED(hResult));

    // Create a new depth stencil
    hResult = CreateDepthStencilView(_uX, _uY);
    assert(!FAILED(hResult));

    // Create rasterizer
    hResult = CreateRasterizerState();
    assert(!FAILED(hResult));

    hResult = CreateBlendState();
    assert(!FAILED(hResult));

    // Update scissor
    SetScissorRect(_uX, _uY);

    return hResult;
  }
  // ------------------------------------
  void CRender::SetScissorRect(uint32_t _uX, uint32_t _uY)
  {
    D3D11_RECT oRect = D3D11_RECT();
    oRect.left = 0;
    oRect.top = 0;
    oRect.right = static_cast<LONG>(_uX);
    oRect.bottom = static_cast<LONG>(_uY);
    global::dx11::s_pDeviceContext->RSSetScissorRects(1, &oRect);
  }
  // ------------------------------------
  bool CRender::InitImGui()
  {
    if (!IMGUI_CHECKVERSION())
    {
      return false;
    }
    if (!ImGui::CreateContext())
    {
      return false;
    }
    if (!ImGui_ImplWin32_Init(m_pRenderWindow->GetHwnd()))
    {
      return false;
    }
    if (!ImGui_ImplDX11_Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext))
    {
      return false;
    }

    ImGui::StyleColorsDark();
    return true;
  }
  // ------------------------------------
  HRESULT CRender::CreateDevice(uint32_t _uX, uint32_t _uY)
  {
    // Create descriptor
    DXGI_SWAP_CHAIN_DESC oSwapChainDescriptor = DXGI_SWAP_CHAIN_DESC();
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
    uint32_t uNumFeatureLevels = ARRAYSIZE(vctFeatureLevels);

    // Create device and swap chain
    uint32_t uFlags = 0;
    HRESULT hResult = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, uFlags, vctFeatureLevels,
      uNumFeatureLevels, D3D11_SDK_VERSION, &oSwapChainDescriptor, &internal_render::s_oPipeline.pSwapChain,
      &global::dx11::s_pDevice, &oFeatureLevel, &global::dx11::s_pDeviceContext);

    return hResult;
  }
  // ------------------------------------
  void CRender::OnWindowResizeEvent(uint32_t _uX, uint32_t _uY)
  {
    // Remove current target view
    global::dx11::SafeRelease(internal_render::s_oPipeline.pRenderTarget);

    // Resize buffers
    HRESULT hResult = internal_render::s_oPipeline.pSwapChain->ResizeBuffers(0, _uX, _uY, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    assert(!FAILED(hResult));

    // Init basic pipeline
    hResult = InitBasicPipeline(_uX, _uY);
    assert(!FAILED(hResult));
  }
  // ------------------------------------
  HRESULT CRender::CreateRenderTargetView()
  {
    ID3D11Texture2D* pBackBuffer = nullptr;
    internal_render::s_oPipeline.pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (!pBackBuffer) 
    {
      return E_FAIL;
    }

    HRESULT hResult = global::dx11::s_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &internal_render::s_oPipeline.pRenderTarget);
    if (FAILED(hResult))
    {
      return hResult;
    }
    pBackBuffer->Release();
    return hResult;
  }
  // ------------------------------------
  HRESULT CRender::CreateDepthStencilView(uint32_t _uX, uint32_t _uY)
  {
    // Release resources
    global::dx11::SafeRelease(internal_render::s_oPipeline.pDepthTexture);
    global::dx11::SafeRelease(internal_render::s_oPipeline.pDepthStencilState);
    global::dx11::SafeRelease(internal_render::s_oPipeline.pDepthStencilView);

    // Create texture
    D3D11_TEXTURE2D_DESC oTextureDesc = D3D11_TEXTURE2D_DESC();
    oTextureDesc.Width = _uX;
    oTextureDesc.Height = _uY;
    oTextureDesc.MipLevels = 1;
    oTextureDesc.ArraySize = 1;
    oTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    oTextureDesc.SampleDesc.Count = 1;
    oTextureDesc.SampleDesc.Quality = 0;
    oTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    oTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    oTextureDesc.CPUAccessFlags = 0;
    oTextureDesc.MiscFlags = 0;

    HRESULT hResult = global::dx11::s_pDevice->CreateTexture2D
    (
      &oTextureDesc, 
      nullptr, 
      &internal_render::s_oPipeline.pDepthTexture
    );
    assert(!FAILED(hResult));

    // Depth test parameters
    D3D11_DEPTH_STENCIL_DESC oDepthStencilDesc = D3D11_DEPTH_STENCIL_DESC();
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
    hResult = global::dx11::s_pDevice->CreateDepthStencilState
    (
      &oDepthStencilDesc, 
      &internal_render::s_oPipeline.pDepthStencilState
    );

    D3D11_DEPTH_STENCIL_VIEW_DESC oDepthStencilViewDesc = D3D11_DEPTH_STENCIL_VIEW_DESC();
    oDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    oDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    oDepthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view
    return global::dx11::s_pDevice->CreateDepthStencilView
    (
      internal_render::s_oPipeline.pDepthTexture,
      &oDepthStencilViewDesc,
      &internal_render::s_oPipeline.pDepthStencilView
    );
  }
  // ------------------------------------
  HRESULT CRender::CreateRasterizerState(D3D11_FILL_MODE _eFillMode)
  {
    global::dx11::SafeRelease(internal_render::s_oPipeline.pRasterizer);

    D3D11_RASTERIZER_DESC oRasterizerDesc = D3D11_RASTERIZER_DESC();
    oRasterizerDesc.FillMode = _eFillMode;
    oRasterizerDesc.CullMode = D3D11_CULL_BACK;
    oRasterizerDesc.FrontCounterClockwise = false;
    oRasterizerDesc.DepthBias = false;
    oRasterizerDesc.DepthBiasClamp = 0;
    oRasterizerDesc.SlopeScaledDepthBias = 0;
    oRasterizerDesc.DepthClipEnable = true;
    oRasterizerDesc.ScissorEnable = true;
    oRasterizerDesc.MultisampleEnable = false;
    oRasterizerDesc.AntialiasedLineEnable = false;

    return global::dx11::s_pDevice->CreateRasterizerState(&oRasterizerDesc, &internal_render::s_oPipeline.pRasterizer);
  }
  // ------------------------------------
  HRESULT CRender::CreateBlendState()
  {
    global::dx11::SafeRelease(internal_render::s_oPipeline.pBlendState);

    D3D11_RENDER_TARGET_BLEND_DESC oRenderTargetBlendDesc = D3D11_RENDER_TARGET_BLEND_DESC();
    oRenderTargetBlendDesc.BlendEnable = false;
    oRenderTargetBlendDesc.SrcBlend = D3D11_BLEND_ONE;
    oRenderTargetBlendDesc.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    oRenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
    oRenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
    oRenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
    oRenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    oRenderTargetBlendDesc.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    D3D11_BLEND_DESC oBlendDesc = D3D11_BLEND_DESC();
    oBlendDesc.AlphaToCoverageEnable = false;
    oBlendDesc.RenderTarget[0] = oRenderTargetBlendDesc;

    return global::dx11::s_pDevice->CreateBlendState(&oBlendDesc, &internal_render::s_oPipeline.pBlendState);
  }
  // ------------------------------------
  void CRender::ConfigureViewport(uint32_t _uX, uint32_t _uY)
  {
    if (global::dx11::s_pDeviceContext && m_pRenderWindow)
    {
      D3D11_VIEWPORT oViewport = D3D11_VIEWPORT();
      oViewport.Width = static_cast<float>(_uX);
      oViewport.Height = static_cast<float>(_uY);
      oViewport.MinDepth = internal_render::s_fMinDepth;
      oViewport.MaxDepth = internal_render::s_fMaxDepth;

      global::dx11::s_pDeviceContext->RSSetViewports(1, &oViewport);
    }
  }
  // ------------------------------------
  void CRender::BeginDraw() const
  {
    // Clear resources
    BeginMarker(internal_render::s_sPrepareFrameMrk);
    {
      // Clear render target
      ::global::dx11::s_pDeviceContext->ClearRenderTargetView(internal_render::s_oPipeline.pRenderTarget, internal_render::s_v4ClearColor);
      // Clear depth stencil view
      ::global::dx11::s_pDeviceContext->ClearDepthStencilView(internal_render::s_oPipeline.pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

      // Prepare ImGu
      ::ImGui_ImplDX11_NewFrame();
      ::ImGui_ImplWin32_NewFrame();
      ::ImGui::NewFrame();

      // ImGuizmo
      ::ImGuizmo::BeginFrame();
    }
    EndMarker();
  }
  // ------------------------------------
  void CRender::Draw(scene::CScene* _pScene) const
  {
    // Update lights
    _pScene->UpdateLighting();

    // Draw models
    BeginMarker(internal_render::s_sDrawModelsMrk);
    {
      _pScene->DrawModels();
    }
    EndMarker();

    // Draw primitives
    BeginMarker(internal_render::s_sDrawPrimitivesMrk);
    {
      _pScene->DrawPrimitives();
    }
    EndMarker();
  }
  // ------------------------------------
  void CRender::EndDraw() const
  {
    // Update resources
    global::dx11::s_pDeviceContext->OMSetRenderTargets(1, &internal_render::s_oPipeline.pRenderTarget, internal_render::s_oPipeline.pDepthStencilView);
    global::dx11::s_pDeviceContext->OMSetDepthStencilState(internal_render::s_oPipeline.pDepthStencilState, 1);
    global::dx11::s_pDeviceContext->OMSetBlendState(internal_render::s_oPipeline.pBlendState, nullptr, 0xFFFFFFFF);
    global::dx11::s_pDeviceContext->RSSetState(internal_render::s_oPipeline.pRasterizer);

    // Render ImGui
    BeginMarker(internal_render::s_sImGuiMarker);
    {
      ::ImGui::Render();
      ::ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
    EndMarker();

    // Present
    const uint32_t uFlags = 0;
    internal_render::s_oPipeline.pSwapChain->Present(m_bVerticalSync, uFlags);
  }
  // ------------------------------------
  void CRender::BeginMarker(const wchar_t* _sMarker) const
  {
    if (internal_render::s_oPipeline.pUserMarker)
    {
      internal_render::s_oPipeline.pUserMarker->BeginEvent(_sMarker);
    }
  }
  // ------------------------------------
  void CRender::EndMarker() const
  {
    if (internal_render::s_oPipeline.pUserMarker)
    {
      internal_render::s_oPipeline.pUserMarker->EndEvent();
    }
  }
}
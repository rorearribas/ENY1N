#include "Render.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Scenes/Scene.h"
#include "Libs/ImGui/imgui_impl_win32.h"
#include "Libs/ImGui/imgui_impl_dx11.h"
#include "Engine/Engine.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/ImGui/ImGuizmo.h"

// Shaders
#include "Engine/Shaders/Standard/ForwardPixelShader.h"
#include "Engine/Shaders/Standard/ForwardVertexShader.h"

namespace render
{
  const math::CVector3 CRender::s_v3WorldUp(0.0f, 1.0f, 0.0f);

  namespace internal
  {
    static const wchar_t* s_sPrepareFrameMrk(L"Clear");
    static const wchar_t* s_sZPrepassMrk(L"Z-Prepass");
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

      // Deferred shading
      ID3D11RenderTargetView* pGBufferRTV = nullptr;
      ID3D11RenderTargetView* pPositionRTV = nullptr;
      ID3D11RenderTargetView* pAlbedoRTV = nullptr;
      ID3D11RenderTargetView* pNormalRTV = nullptr;
      ID3D11RenderTargetView* pSpecularRTV;

      // Depth
      ID3D11Texture2D* pDepthTexture = nullptr;
      ID3D11DepthStencilView* pDepthStencilView = nullptr;

      // Depth state
      ID3D11DepthStencilState* pDepthStencilState = nullptr;
      D3D11_DEPTH_STENCIL_DESC oDepthStencilCfg = D3D11_DEPTH_STENCIL_DESC();

      // Rasterizer
      ID3D11RasterizerState* pRasterizer = nullptr;
      D3D11_RASTERIZER_DESC oRasterizerCfg = D3D11_RASTERIZER_DESC();

      // Blend
      ID3D11BlendState* pBlendState = nullptr;
      D3D11_RENDER_TARGET_BLEND_DESC oBlendStateCfg = D3D11_RENDER_TARGET_BLEND_DESC();

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
    LOG("Creating render window...");
    m_pRenderWindow = new render::CRenderWindow(_uX, _uY);
    SUCCESS_LOG("The window has been created successfully!");

    // Init render
    LOG("Initializing render...");
    HRESULT hResult = Init(_uX, _uY);
    UNUSED_VAR(hResult);
    assert(!FAILED(hResult));
    SUCCESS_LOG("Render initialized correctly!");
  }
  // ------------------------------------
  CRender::~CRender()
  {
    // Delete global resources
    global::dx11::SafeRelease(global::dx11::s_pDevice);
    global::dx11::SafeRelease(global::dx11::s_pDeviceContext);

    // Delete internal resources
    global::dx11::SafeRelease(internal::s_oPipeline.pSwapChain);
    global::dx11::SafeRelease(internal::s_oPipeline.pAlbedoRTV);
    global::dx11::SafeRelease(internal::s_oPipeline.pDepthTexture);
    global::dx11::SafeRelease(internal::s_oPipeline.pDepthStencilState);
    global::dx11::SafeRelease(internal::s_oPipeline.pDepthStencilView);
    global::dx11::SafeRelease(internal::s_oPipeline.pRasterizer);
    global::dx11::SafeRelease(internal::s_oPipeline.pBlendState);
    global::dx11::SafeRelease(internal::s_oPipeline.pUserMarker);

    // Release objetcs
    global::ReleaseObject(internal::s_oPipeline.pPixelShader);
    global::ReleaseObject(internal::s_oPipeline.pVertexShader);
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
    internal::s_oPipeline.pPixelShader = new CShader<PIXEL_SHADER>(g_ForwardPixelShader, ARRAYSIZE(g_ForwardPixelShader));
    internal::s_oPipeline.pVertexShader = new CShader<VERTEX_SHADER>(g_ForwardVertexShader, ARRAYSIZE(g_ForwardVertexShader));

    // Set delegate
    utils::CDelegate<void(uint32_t, uint32_t)> oResizeDelegate(&CRender::OnWindowResizeEvent, this);
    global::delegates::s_vctOnWindowResizeDelegates.emplace_back(oResizeDelegate);

    // Get user def
    global::dx11::s_pDeviceContext->QueryInterface
    (
      __uuidof(ID3DUserDefinedAnnotation),
      reinterpret_cast<void**>(&internal::s_oPipeline.pUserMarker)
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

    // Set standard rasterizer config
    internal::s_oPipeline.oRasterizerCfg.FillMode = D3D11_FILL_SOLID;
    internal::s_oPipeline.oRasterizerCfg.CullMode = D3D11_CULL_BACK;
    internal::s_oPipeline.oRasterizerCfg.FrontCounterClockwise = false;
    internal::s_oPipeline.oRasterizerCfg.DepthBias = 10; // Z-Fighting
    internal::s_oPipeline.oRasterizerCfg.DepthBiasClamp = 0.0f;
    internal::s_oPipeline.oRasterizerCfg.SlopeScaledDepthBias = 1.5f;
    internal::s_oPipeline.oRasterizerCfg.DepthClipEnable = true;
    internal::s_oPipeline.oRasterizerCfg.ScissorEnable = true;
    internal::s_oPipeline.oRasterizerCfg.MultisampleEnable = false;
    internal::s_oPipeline.oRasterizerCfg.AntialiasedLineEnable = false;

    // Create rasterizer
    hResult = CreateRasterizerState(internal::s_oPipeline.oRasterizerCfg);
    assert(!FAILED(hResult));

    // Set standard blend state config
    internal::s_oPipeline.oBlendStateCfg.BlendEnable = false;
    internal::s_oPipeline.oBlendStateCfg.SrcBlend = D3D11_BLEND_ONE;
    internal::s_oPipeline.oBlendStateCfg.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    internal::s_oPipeline.oBlendStateCfg.BlendOp = D3D11_BLEND_OP_ADD;
    internal::s_oPipeline.oBlendStateCfg.SrcBlendAlpha = D3D11_BLEND_ONE;
    internal::s_oPipeline.oBlendStateCfg.DestBlendAlpha = D3D11_BLEND_ZERO;
    internal::s_oPipeline.oBlendStateCfg.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    internal::s_oPipeline.oBlendStateCfg.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    hResult = CreateBlendState(internal::s_oPipeline.oBlendStateCfg);
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
    oSwapChainDescriptor.OutputWindow = m_pRenderWindow->GetHwnd();
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
      uNumFeatureLevels, D3D11_SDK_VERSION, &oSwapChainDescriptor, &internal::s_oPipeline.pSwapChain,
      &global::dx11::s_pDevice, &oFeatureLevel, &global::dx11::s_pDeviceContext);

    return hResult;
  }
  // ------------------------------------
  void CRender::OnWindowResizeEvent(uint32_t _uX, uint32_t _uY)
  {
    // Remove current target view
    global::dx11::SafeRelease(internal::s_oPipeline.pAlbedoRTV);

    // Resize buffers
    HRESULT hResult = internal::s_oPipeline.pSwapChain->ResizeBuffers(0, _uX, _uY, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    assert(!FAILED(hResult));

    // Init basic pipeline
    hResult = InitBasicPipeline(_uX, _uY);
    assert(!FAILED(hResult));
  }
  // ------------------------------------
  HRESULT CRender::CreateRenderTargetView()
  {
    ID3D11Texture2D* pBackBuffer = nullptr;
    internal::s_oPipeline.pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (!pBackBuffer)
    {
      return E_FAIL;
    }

    HRESULT hResult = global::dx11::s_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &internal::s_oPipeline.pAlbedoRTV);
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
    global::dx11::SafeRelease(internal::s_oPipeline.pDepthTexture);
    global::dx11::SafeRelease(internal::s_oPipeline.pDepthStencilState);
    global::dx11::SafeRelease(internal::s_oPipeline.pDepthStencilView);

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
      &internal::s_oPipeline.pDepthTexture
    );
    assert(!FAILED(hResult));

    // Create standard depth stencil state
    internal::s_oPipeline.oDepthStencilCfg.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    internal::s_oPipeline.oDepthStencilCfg.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    internal::s_oPipeline.oDepthStencilCfg.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    internal::s_oPipeline.oDepthStencilCfg.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    internal::s_oPipeline.oDepthStencilCfg.DepthEnable = true;
    internal::s_oPipeline.oDepthStencilCfg.StencilEnable = true;

    // Front-face
    internal::s_oPipeline.oDepthStencilCfg.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    internal::s_oPipeline.oDepthStencilCfg.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    internal::s_oPipeline.oDepthStencilCfg.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    internal::s_oPipeline.oDepthStencilCfg.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Back-face
    internal::s_oPipeline.oDepthStencilCfg.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    internal::s_oPipeline.oDepthStencilCfg.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    internal::s_oPipeline.oDepthStencilCfg.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    internal::s_oPipeline.oDepthStencilCfg.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    hResult = SetDepthStencilState(internal::s_oPipeline.oDepthStencilCfg);
    assert(!FAILED(hResult));

    D3D11_DEPTH_STENCIL_VIEW_DESC oDepthStencilViewDesc = D3D11_DEPTH_STENCIL_VIEW_DESC();
    oDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    oDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    oDepthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view
    return global::dx11::s_pDevice->CreateDepthStencilView
    (
      internal::s_oPipeline.pDepthTexture, 
      &oDepthStencilViewDesc, 
      &internal::s_oPipeline.pDepthStencilView
    );
  }
  // ------------------------------------
  HRESULT CRender::SetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& _oDepthStencilState)
  {
    // Create depth stencil state
    global::dx11::SafeRelease(internal::s_oPipeline.pDepthStencilState);
    return global::dx11::s_pDevice->CreateDepthStencilState(&_oDepthStencilState, &internal::s_oPipeline.pDepthStencilState);
  }
  // ------------------------------------
  HRESULT CRender::CreateRasterizerState(const D3D11_RASTERIZER_DESC& _oRasterizerState)
  {
    // Create rasterizer state
    global::dx11::SafeRelease(internal::s_oPipeline.pRasterizer);
    return global::dx11::s_pDevice->CreateRasterizerState(&_oRasterizerState, &internal::s_oPipeline.pRasterizer);
  }
  // ------------------------------------
  HRESULT CRender::CreateBlendState(const D3D11_RENDER_TARGET_BLEND_DESC& _oBlendState)
  {
    // Create blend desc
    D3D11_BLEND_DESC oBlendDesc = D3D11_BLEND_DESC();
    oBlendDesc.AlphaToCoverageEnable = false;
    oBlendDesc.RenderTarget[0] = _oBlendState;

    // Create blend state
    global::dx11::SafeRelease(internal::s_oPipeline.pBlendState);
    return global::dx11::s_pDevice->CreateBlendState(&oBlendDesc, &internal::s_oPipeline.pBlendState);
  }
  // ------------------------------------
  void CRender::ConfigureViewport(uint32_t _uX, uint32_t _uY)
  {
    if (global::dx11::s_pDeviceContext && m_pRenderWindow)
    {
      D3D11_VIEWPORT oViewport = D3D11_VIEWPORT();
      oViewport.Width = static_cast<float>(_uX);
      oViewport.Height = static_cast<float>(_uY);
      oViewport.MinDepth = internal::s_fMinDepth;
      oViewport.MaxDepth = internal::s_fMaxDepth;

      global::dx11::s_pDeviceContext->RSSetViewports(1, &oViewport);
    }
  }
  // ------------------------------------
  void CRender::BeginDraw()
  {
    // Clear resources
    BeginMarker(internal::s_sPrepareFrameMrk);
    {
      // Clear render target
      ::global::dx11::s_pDeviceContext->ClearRenderTargetView(internal::s_oPipeline.pAlbedoRTV, internal::s_v4ClearColor);
      // Clear depth stencil view
      ::global::dx11::s_pDeviceContext->ClearDepthStencilView(internal::s_oPipeline.pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

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
  void CRender::Draw(scene::CScene* _pScene)
  {
    BeginMarker(internal::s_sZPrepassMrk);
    {
      // Set depth stencil config
      internal::s_oPipeline.oDepthStencilCfg.DepthFunc = D3D11_COMPARISON_LESS;
      internal::s_oPipeline.oDepthStencilCfg.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
      HRESULT hResult = SetDepthStencilState(internal::s_oPipeline.oDepthStencilCfg);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));
      // Push invalid RT
      global::dx11::s_pDeviceContext->OMSetRenderTargets(0, nullptr, internal::s_oPipeline.pDepthStencilView);
      // Set depth stencil state
      global::dx11::s_pDeviceContext->OMSetDepthStencilState(internal::s_oPipeline.pDepthStencilState, 1);

      internal::s_oPipeline.pVertexShader->PushShader(); // Push
      internal::s_oPipeline.pPixelShader->DetachShader(); // Detach
      _pScene->DrawModels();
    }
    EndMarker();

    // Draw models
    BeginMarker(internal::s_sDrawModelsMrk);
    {
      internal::s_oPipeline.oDepthStencilCfg.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
      internal::s_oPipeline.oDepthStencilCfg.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
      HRESULT hResult = SetDepthStencilState(internal::s_oPipeline.oDepthStencilCfg);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));
      // Set render target      
      global::dx11::s_pDeviceContext->OMSetRenderTargets(1, &internal::s_oPipeline.pAlbedoRTV, internal::s_oPipeline.pDepthStencilView);
      // Set depth stencil state
      global::dx11::s_pDeviceContext->OMSetDepthStencilState(internal::s_oPipeline.pDepthStencilState, 1);

      // Push shaders
      internal::s_oPipeline.pVertexShader->PushShader(); // Push
      internal::s_oPipeline.pPixelShader->PushShader(); // Push

      // Update lights
      _pScene->UpdateLighting();
      // Draw models
      _pScene->DrawModels();
    }
    EndMarker();

    // Draw primitives
    BeginMarker(internal::s_sDrawPrimitivesMrk);
    {
      internal::s_oPipeline.oDepthStencilCfg.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
      internal::s_oPipeline.oDepthStencilCfg.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
      HRESULT hResult = SetDepthStencilState(internal::s_oPipeline.oDepthStencilCfg);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));
      // Set depth stencil state
      global::dx11::s_pDeviceContext->OMSetDepthStencilState(internal::s_oPipeline.pDepthStencilState, 1);

      _pScene->DrawPrimitives();
    }
    EndMarker();
  }
  // ------------------------------------
  void CRender::EndDraw()
  {
    // Update resources
    global::dx11::s_pDeviceContext->OMSetBlendState(internal::s_oPipeline.pBlendState, nullptr, 0xFFFFFFFF);
    global::dx11::s_pDeviceContext->RSSetState(internal::s_oPipeline.pRasterizer);

    // Render ImGui
    BeginMarker(internal::s_sImGuiMarker);
    {
      ::ImGui::Render();
      ::ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
    EndMarker();

    // Present
    const uint32_t uFlags = 0;
    internal::s_oPipeline.pSwapChain->Present(m_bVerticalSync, uFlags);
  }
  // ------------------------------------
  void CRender::SetFillMode(D3D11_FILL_MODE _eFillMode)
  {
    // Update rasterizer
    internal::s_oPipeline.oRasterizerCfg.FillMode = _eFillMode;
    CreateRasterizerState(internal::s_oPipeline.oRasterizerCfg);
  }
  // ------------------------------------
  void CRender::BeginMarker(const wchar_t* _sMarker) const
  {
    if (internal::s_oPipeline.pUserMarker)
    {
      internal::s_oPipeline.pUserMarker->BeginEvent(_sMarker);
    }
  }
  // ------------------------------------
  void CRender::EndMarker() const
  {
    if (internal::s_oPipeline.pUserMarker)
    {
      internal::s_oPipeline.pUserMarker->EndEvent();
    }
  }
}
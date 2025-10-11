#include "Render.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Scenes/Scene.h"
#include "Libs/ImGui/imgui_impl_win32.h"
#include "Libs/ImGui/imgui_impl_dx11.h"
#include "Engine/Engine.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/ImGui/ImGuizmo.h"
#include "Engine/Render/RenderTarget.h"

// Shaders
#include "Engine/Shaders/Pipeline/StandardVS.h"
#include "Engine/Shaders/Pipeline/SimplePS.h"
#include "Engine/Shaders/Pipeline/ForwardPS.h"
#include "Engine/Shaders/Pipeline/DeferredPixelShader.h"
#include "RenderTypes.h"

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
      ID3D11RenderTargetView* pBackBuffer = nullptr;

      // Deferred shading
      render::CRenderTarget* pPositionRT;
      render::CRenderTarget* pDiffuseRT;
      render::CRenderTarget* pNormalRT;
      render::CRenderTarget* pSpecularRT;

      // Depth
      texture::CTexture2D<DEPTH_STENCIL>* pDepthStencilTexture = nullptr;
      texture::CTexture2D<SHADER_RESOURCE>* pDepthTexture = nullptr;
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

      // Standard shaders
      shader::CShader<E_VERTEX>* pStandardVS;
      shader::CShader<E_PIXEL>* pSimplePS;

      // Deferred
      shader::CShader<E_PIXEL>* pDeferredPS;
      shader::CShader<E_PIXEL>* pCalculateLightsShader;
    };

    static SRenderPipeline s_oRender;
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
    // Shutdown ImGui
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // Release depth
    global::dx::SafeRelease(internal::s_oRender.pDepthStencilState);
    global::ReleaseObject(internal::s_oRender.pDepthStencilTexture);
    global::ReleaseObject(internal::s_oRender.pDepthTexture);

    // Release render targets
    global::dx::SafeRelease(internal::s_oRender.pRasterizer);

    global::ReleaseObject(internal::s_oRender.pDiffuseRT);
    global::ReleaseObject(internal::s_oRender.pNormalRT);
    global::ReleaseObject(internal::s_oRender.pSpecularRT);

    global::dx::SafeRelease(internal::s_oRender.pBlendState);
    global::dx::SafeRelease(internal::s_oRender.pUserMarker);

    // Release shaders
    global::ReleaseObject(internal::s_oRender.pStandardVS);
    global::ReleaseObject(internal::s_oRender.pSimplePS);
    global::ReleaseObject(internal::s_oRender.pDeferredPS);
    global::ReleaseObject(internal::s_oRender.pCalculateLightsShader);

    // Release swap chain
    global::dx::SafeRelease(internal::s_oRender.pSwapChain);
    global::dx::SafeRelease(internal::s_oRender.pBackBuffer);

    // Release device context then device
    global::dx::SafeRelease(global::dx::s_pDeviceContext);
    global::dx::SafeRelease(global::dx::s_pDevice);

    // Release render window
    global::ReleaseObject(m_pRenderWindow);
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
      return E_FAIL;
    }

    // Setup basic pipeline
    hResult = InitBasicPipeline(_uX, _uY);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Create shaders for 3D pipeline
    internal::s_oRender.pStandardVS = new shader::CShader<E_VERTEX>(g_StandardVS, ARRAYSIZE(g_StandardVS));
    internal::s_oRender.pSimplePS = new shader::CShader<E_PIXEL>(g_SimplePS, ARRAYSIZE(g_SimplePS));
    internal::s_oRender.pDeferredPS = new shader::CShader<E_PIXEL>(g_DeferredPixelShader, ARRAYSIZE(g_DeferredPixelShader));
    internal::s_oRender.pCalculateLightsShader = new shader::CShader<E_PIXEL>(g_ForwardPS, ARRAYSIZE(g_ForwardPS));

    // Set delegate
    utils::CDelegate<void(uint32_t, uint32_t)> oResizeDelegate(&CRender::OnWindowResizeEvent, this);
    global::delegates::s_vctOnWindowResizeDelegates.emplace_back(oResizeDelegate);

    // Get user def
    global::dx::s_pDeviceContext->QueryInterface
    (
      __uuidof(ID3DUserDefinedAnnotation),
      reinterpret_cast<void**>(&internal::s_oRender.pUserMarker)
    );

    return hResult;
  }
  // ------------------------------------
  HRESULT CRender::InitBasicPipeline(uint32_t _uX, uint32_t _uY)
  {
    // Configure viewport
    ConfigureViewport(_uX, _uY);

    // Create a new depth stencil
    HRESULT hResult = CreateDepthStencilView(_uX, _uY);
    assert(!FAILED(hResult));

    // Setup deferred shading
    hResult = SetupDeferredShading(_uX, _uY);
    assert(!FAILED(hResult));

    // Create new render target
    hResult = CreateBackBuffer();
    assert(!FAILED(hResult));

    // Set standard rasterizer config
    internal::s_oRender.oRasterizerCfg.FillMode = D3D11_FILL_SOLID;
    internal::s_oRender.oRasterizerCfg.CullMode = D3D11_CULL_BACK;
    internal::s_oRender.oRasterizerCfg.FrontCounterClockwise = false;
    internal::s_oRender.oRasterizerCfg.DepthBias = 10; // Z-Fighting
    internal::s_oRender.oRasterizerCfg.DepthBiasClamp = 0.0f;
    internal::s_oRender.oRasterizerCfg.SlopeScaledDepthBias = 1.5f;
    internal::s_oRender.oRasterizerCfg.DepthClipEnable = true;
    internal::s_oRender.oRasterizerCfg.ScissorEnable = true;
    internal::s_oRender.oRasterizerCfg.MultisampleEnable = false;
    internal::s_oRender.oRasterizerCfg.AntialiasedLineEnable = false;

    // Create rasterizer
    hResult = CreateRasterizerState(internal::s_oRender.oRasterizerCfg);
    assert(!FAILED(hResult));

    // Set standard blend state config
    internal::s_oRender.oBlendStateCfg.BlendEnable = false;
    internal::s_oRender.oBlendStateCfg.SrcBlend = D3D11_BLEND_ONE;
    internal::s_oRender.oBlendStateCfg.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    internal::s_oRender.oBlendStateCfg.BlendOp = D3D11_BLEND_OP_ADD;
    internal::s_oRender.oBlendStateCfg.SrcBlendAlpha = D3D11_BLEND_ONE;
    internal::s_oRender.oBlendStateCfg.DestBlendAlpha = D3D11_BLEND_ZERO;
    internal::s_oRender.oBlendStateCfg.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    internal::s_oRender.oBlendStateCfg.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    hResult = CreateBlendState(internal::s_oRender.oBlendStateCfg);
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
    global::dx::s_pDeviceContext->RSSetScissorRects(1, &oRect);
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
    if (!ImGui_ImplDX11_Init(global::dx::s_pDevice, global::dx::s_pDeviceContext))
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
      uNumFeatureLevels, D3D11_SDK_VERSION, &oSwapChainDescriptor, &internal::s_oRender.pSwapChain,
      &global::dx::s_pDevice, &oFeatureLevel, &global::dx::s_pDeviceContext);

    return hResult;
  }
  // ------------------------------------
  void CRender::OnWindowResizeEvent(uint32_t _uX, uint32_t _uY)
  {
    // Remove current target view
    global::dx::SafeRelease(internal::s_oRender.pBackBuffer);

    // Resize buffers
    HRESULT hResult = internal::s_oRender.pSwapChain->ResizeBuffers(0, _uX, _uY, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    assert(!FAILED(hResult));

    // Init basic pipeline
    hResult = InitBasicPipeline(_uX, _uY);
    assert(!FAILED(hResult));
  }
  // ------------------------------------
  HRESULT CRender::CreateBackBuffer()
  {
    ID3D11Texture2D* pTexture = nullptr;
    internal::s_oRender.pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture));
    if (!pTexture)
    {
      return E_FAIL;
    }

    HRESULT hResult = global::dx::s_pDevice->CreateRenderTargetView(pTexture, nullptr, &internal::s_oRender.pBackBuffer);
    if (FAILED(hResult))
    {
      return hResult;
    }
    pTexture->Release();
    return hResult;
  }
  // ------------------------------------
  HRESULT CRender::CreateDepthStencilView(uint32_t _uX, uint32_t _uY)
  {
    // Release resources
    global::ReleaseObject(internal::s_oRender.pDepthStencilTexture);
    global::dx::SafeRelease(internal::s_oRender.pDepthStencilState);

    // Create depth stencil texture
    internal::s_oRender.pDepthStencilTexture = new texture::CTexture2D<DEPTH_STENCIL>("DepthStencilTexture");

    // Set desc
    D3D11_TEXTURE2D_DESC oTextureDesc = D3D11_TEXTURE2D_DESC();
    oTextureDesc.Width = _uX;
    oTextureDesc.Height = _uY;
    oTextureDesc.MipLevels = 1;
    oTextureDesc.ArraySize = 1;
    oTextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    oTextureDesc.SampleDesc.Count = 1;
    oTextureDesc.SampleDesc.Quality = 0;
    oTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    oTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    oTextureDesc.CPUAccessFlags = 0;
    oTextureDesc.MiscFlags = 0;

    HRESULT hResult = internal::s_oRender.pDepthStencilTexture->CreateTexture(oTextureDesc);
    assert(!FAILED(hResult));

    // Set depth stencil view desc
    D3D11_DEPTH_STENCIL_VIEW_DESC oDepthStencilViewDesc = D3D11_DEPTH_STENCIL_VIEW_DESC();
    oDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    oDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    // Create the depth stencil view
    hResult = internal::s_oRender.pDepthStencilTexture->CreateView(oDepthStencilViewDesc);
    assert(!FAILED(hResult));

    // Create depth texture
    ID3D11Texture2D* pDepthTexture = *(internal::s_oRender.pDepthStencilTexture);
    internal::s_oRender.pDepthTexture = new texture::CTexture2D<SHADER_RESOURCE>("DepthTexture");
    internal::s_oRender.pDepthTexture->SetTexture(pDepthTexture);

    // Set shader resource view desc
    D3D11_SHADER_RESOURCE_VIEW_DESC oShaderResourceViewDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
    oShaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    oShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    oShaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view
    hResult = internal::s_oRender.pDepthTexture->CreateView(oShaderResourceViewDesc);
    assert(!FAILED(hResult));

    // Create standard depth stencil state
    internal::s_oRender.oDepthStencilCfg.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    internal::s_oRender.oDepthStencilCfg.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    internal::s_oRender.oDepthStencilCfg.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    internal::s_oRender.oDepthStencilCfg.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    internal::s_oRender.oDepthStencilCfg.DepthEnable = true;
    internal::s_oRender.oDepthStencilCfg.StencilEnable = true;

    // Front-face
    internal::s_oRender.oDepthStencilCfg.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    internal::s_oRender.oDepthStencilCfg.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    internal::s_oRender.oDepthStencilCfg.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    internal::s_oRender.oDepthStencilCfg.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Back-face
    internal::s_oRender.oDepthStencilCfg.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    internal::s_oRender.oDepthStencilCfg.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    internal::s_oRender.oDepthStencilCfg.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    internal::s_oRender.oDepthStencilCfg.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    return SetDepthStencilState(internal::s_oRender.oDepthStencilCfg);
  }
  // ------------------------------------
  HRESULT CRender::SetupDeferredShading(uint32_t _uX, uint32_t _uY)
  {
    global::ReleaseObject(internal::s_oRender.pPositionRT);
    internal::s_oRender.pPositionRT = new CRenderTarget("Position");
    HRESULT hResult = internal::s_oRender.pPositionRT->CreateRT(_uX, _uY, DXGI_FORMAT_B8G8R8A8_UNORM);
    if (FAILED(hResult))
    {
      return hResult;
    }

    global::ReleaseObject(internal::s_oRender.pDiffuseRT);
    internal::s_oRender.pDiffuseRT = new CRenderTarget("Diffuse");
    hResult = internal::s_oRender.pDiffuseRT->CreateRT(_uX, _uY, DXGI_FORMAT_B8G8R8A8_UNORM);
    if (FAILED(hResult))
    {
      return hResult;
    }

    global::ReleaseObject(internal::s_oRender.pNormalRT);
    internal::s_oRender.pNormalRT = new CRenderTarget("Normals");
    hResult = internal::s_oRender.pNormalRT->CreateRT(_uX, _uY, DXGI_FORMAT_R16G16B16A16_FLOAT);
    if (FAILED(hResult))
    {
      return hResult;
    }

    global::ReleaseObject(internal::s_oRender.pSpecularRT);
    internal::s_oRender.pSpecularRT = new CRenderTarget("Specular");
    return internal::s_oRender.pSpecularRT->CreateRT(_uX, _uY, DXGI_FORMAT_B8G8R8A8_UNORM);
  }
  // ------------------------------------
  HRESULT CRender::SetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& _oDepthStencilState)
  {
    // Create depth stencil state
    global::dx::SafeRelease(internal::s_oRender.pDepthStencilState);
    return global::dx::s_pDevice->CreateDepthStencilState(&_oDepthStencilState, &internal::s_oRender.pDepthStencilState);
  }
  // ------------------------------------
  HRESULT CRender::CreateRasterizerState(const D3D11_RASTERIZER_DESC& _oRasterizerState)
  {
    // Create rasterizer state
    global::dx::SafeRelease(internal::s_oRender.pRasterizer);
    return global::dx::s_pDevice->CreateRasterizerState(&_oRasterizerState, &internal::s_oRender.pRasterizer);
  }
  // ------------------------------------
  HRESULT CRender::CreateBlendState(const D3D11_RENDER_TARGET_BLEND_DESC& _oBlendState)
  {
    // Create blend desc
    D3D11_BLEND_DESC oBlendDesc = D3D11_BLEND_DESC();
    oBlendDesc.AlphaToCoverageEnable = false;
    oBlendDesc.RenderTarget[0] = _oBlendState;

    // Create blend state
    global::dx::SafeRelease(internal::s_oRender.pBlendState);
    return global::dx::s_pDevice->CreateBlendState(&oBlendDesc, &internal::s_oRender.pBlendState);
  }
  // ------------------------------------
  void CRender::ConfigureViewport(uint32_t _uX, uint32_t _uY)
  {
    if (global::dx::s_pDeviceContext && m_pRenderWindow)
    {
      D3D11_VIEWPORT oViewport = D3D11_VIEWPORT();
      oViewport.Width = static_cast<float>(_uX);
      oViewport.Height = static_cast<float>(_uY);
      oViewport.MinDepth = internal::s_fMinDepth;
      oViewport.MaxDepth = internal::s_fMaxDepth;

      global::dx::s_pDeviceContext->RSSetViewports(1, &oViewport);
    }
  }
  // ------------------------------------
  void CRender::BeginDraw()
  {
    // Clear resources
    BeginMarker(internal::s_sPrepareFrameMrk);
    {
      // Clear back buffer
      ::global::dx::s_pDeviceContext->ClearRenderTargetView(internal::s_oRender.pBackBuffer, internal::s_v4ClearColor);
      // Clear depth stencil view
      ID3D11DepthStencilView* pDepthStencilView = internal::s_oRender.pDepthStencilTexture->GetView();
      ::global::dx::s_pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

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
      internal::s_oRender.oDepthStencilCfg.DepthFunc = D3D11_COMPARISON_LESS;
      internal::s_oRender.oDepthStencilCfg.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
      HRESULT hResult = SetDepthStencilState(internal::s_oRender.oDepthStencilCfg);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));
      // Push invalid RT
      ID3D11DepthStencilView* pDepthStencilView = internal::s_oRender.pDepthStencilTexture->GetView();
      global::dx::s_pDeviceContext->OMSetRenderTargets(0, nullptr, pDepthStencilView);
      // Set depth stencil state
      global::dx::s_pDeviceContext->OMSetDepthStencilState(internal::s_oRender.pDepthStencilState, 1);

      internal::s_oRender.pStandardVS->AttachShader(); // Attach
      internal::s_oRender.pCalculateLightsShader->DetachShader(); // Detach
      internal::s_oRender.pDeferredPS->DetachShader(); // Detach
      _pScene->DrawModels();
    }
    EndMarker();

    // Draw models
    BeginMarker(internal::s_sDrawModelsMrk);
    {
      internal::s_oRender.oDepthStencilCfg.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
      internal::s_oRender.oDepthStencilCfg.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
      HRESULT hResult = SetDepthStencilState(internal::s_oRender.oDepthStencilCfg);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));

      // RT list
      ID3D11RenderTargetView* lstRenderViews[4];
      lstRenderViews[0] = *(internal::s_oRender.pPositionRT);
      lstRenderViews[1] = *(internal::s_oRender.pDiffuseRT);
      lstRenderViews[2] = *(internal::s_oRender.pNormalRT);
      lstRenderViews[3] = *(internal::s_oRender.pSpecularRT);

      // Set render targets
      ID3D11DepthStencilView* pDepthStencilView = internal::s_oRender.pDepthStencilTexture->GetView();
      global::dx::s_pDeviceContext->OMSetRenderTargets(ARRAYSIZE(lstRenderViews), &lstRenderViews[0], pDepthStencilView);
      // Set depth stencil state
      global::dx::s_pDeviceContext->OMSetDepthStencilState(internal::s_oRender.pDepthStencilState, 1);

      // Push shaders
      internal::s_oRender.pStandardVS->AttachShader(); // Push
      internal::s_oRender.pDeferredPS->AttachShader(); // Detach

      // Draw models
      _pScene->DrawModels();

      // Push shader
      internal::s_oRender.pCalculateLightsShader->AttachShader(); // Attach
      _pScene->UpdateLighting();
    }
    EndMarker();

    // Draw primitives
    BeginMarker(internal::s_sDrawPrimitivesMrk);
    {
      ID3D11DepthStencilView* pDepthStencilView = internal::s_oRender.pDepthStencilTexture->GetView();
      global::dx::s_pDeviceContext->OMSetRenderTargets(1, &internal::s_oRender.pBackBuffer, pDepthStencilView);

      internal::s_oRender.oDepthStencilCfg.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
      internal::s_oRender.oDepthStencilCfg.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
      HRESULT hResult = SetDepthStencilState(internal::s_oRender.oDepthStencilCfg);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));
      // Push depth stencil state
      global::dx::s_pDeviceContext->OMSetDepthStencilState(internal::s_oRender.pDepthStencilState, 1);

      // Attach shader
      internal::s_oRender.pSimplePS->AttachShader();

      // Draw primitives
      _pScene->DrawPrimitives();
    }
    EndMarker();
  }
  // ------------------------------------
  void CRender::EndDraw()
  {
    // Update resources
    global::dx::s_pDeviceContext->OMSetBlendState(internal::s_oRender.pBlendState, nullptr, 0xFFFFFFFF);
    global::dx::s_pDeviceContext->RSSetState(internal::s_oRender.pRasterizer);

    // Render ImGui
    BeginMarker(internal::s_sImGuiMarker);
    {
      ::ImGui::Render();
      ::ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
    EndMarker();

    // Present
    const uint32_t uFlags = 0;
    internal::s_oRender.pSwapChain->Present(m_bVerticalSync, uFlags);
  }
  // ------------------------------------
  void CRender::SetFillMode(D3D11_FILL_MODE _eFillMode)
  {
    // Update rasterizer
    internal::s_oRender.oRasterizerCfg.FillMode = _eFillMode;
    CreateRasterizerState(internal::s_oRender.oRasterizerCfg);
  }
  // ------------------------------------
  void CRender::BeginMarker(const wchar_t* _sMarker) const
  {
    if (internal::s_oRender.pUserMarker)
    {
      internal::s_oRender.pUserMarker->BeginEvent(_sMarker);
    }
  }
  // ------------------------------------
  void CRender::EndMarker() const
  {
    if (internal::s_oRender.pUserMarker)
    {
      internal::s_oRender.pUserMarker->EndEvent();
    }
  }
}
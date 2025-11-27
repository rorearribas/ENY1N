#include "Render.h"

#include "Engine/Render/ConstantBuffer/BufferTypes.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Scenes/Scene.h"
#include "Engine/Render/RenderTarget.h"
#include "RenderTypes.h"

// Debug
#include "Engine/Shaders/Forward/SimpleVS.h"
#include "Engine/Shaders/Forward/SimplePS.h"

// Deferred
#include "Engine/Shaders/Deferred/StandardVS.h"
#include "Engine/Shaders/Deferred/LightsPS.h"
#include "Engine/Shaders/Deferred/GBufferPS.h"
#include "Engine/Shaders/Deferred/DrawTriangleVS.h"

// ImGui
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/ImGui/imgui_impl_win32.h"
#include "Libs/ImGui/imgui_impl_dx11.h"
#include "Libs/ImGui/ImGuizmo.h"

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

    // Standard Input
    static const int s_iStandardLayoutSize(7);
    static const D3D11_INPUT_ELEMENT_DESC s_tStandardLayout[s_iStandardLayoutSize] =
    {
      // Vertex layout
      { "VERTEXPOS",          0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 12
      { "NORMAL",             0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 24
      { "UV",                 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 36
      // Instancing
      { "INSTANCE_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 16
      { "INSTANCE_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 32
      { "INSTANCE_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 48
      { "INSTANCE_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 64
    };

    // Debug input
    static const int s_iDebugLayoutSize(6);
    static const D3D11_INPUT_ELEMENT_DESC s_tPrimitivesLayout[s_iDebugLayoutSize] =
    {
      // Vertex layout
      { "VERTEXPOS",          0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 12
      { "COLOR",              0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 24
      // Instancing
      { "INSTANCE_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 16
      { "INSTANCE_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 32
      { "INSTANCE_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 48
      { "INSTANCE_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 64
    };

    struct TRenderPipeline
    {
      // Swap chain + RT
      IDXGISwapChain* pSwapChain = nullptr;
      ID3D11RenderTargetView* pBackBuffer = nullptr;

      // Deferred shading
      render::CRenderTarget* pDiffuseRT = nullptr;
      render::CRenderTarget* pNormalRT = nullptr;
      render::CRenderTarget* pSpecularRT = nullptr;
      ID3D11SamplerState* pLinearSampler = nullptr;

      // Constant buffers
      CConstantBuffer<TTransforms> tTransformsBuffer;
      CConstantBuffer<TTextureInfo> tTexturesInfoBuffer;
      CConstantBuffer<TInstancingMode> tInstancingModeBuffer;

      // Depth
      texture::CTexture2D<EView::DEPTH_STENCIL>* pDepthStencil = nullptr;
      texture::CTexture2D<EView::SHADER_RESOURCE>* pDepthTexture = nullptr;

      // Stencils
      ID3D11DepthStencilState* pZPrepassStencilState = nullptr;
      ID3D11DepthStencilState* pDeferredStencilState = nullptr;
      ID3D11DepthStencilState* pDebugStencilState = nullptr;

      // Rasterizer
      ID3D11RasterizerState* pRasterizer = nullptr;
      D3D11_RASTERIZER_DESC tRasterizerCfg = D3D11_RASTERIZER_DESC();

      // Blend
      ID3D11BlendState* pBlendState = nullptr;
      D3D11_RENDER_TARGET_BLEND_DESC tBlendStateCfg = D3D11_RENDER_TARGET_BLEND_DESC();

      // Layouts
      ID3D11InputLayout* pStandardLayout = nullptr;
      ID3D11InputLayout* pDebugLayout = nullptr;

      // Debug
      ID3DUserDefinedAnnotation* pUserMarker = nullptr;

      // Forward
      shader::CShader<EShader::E_VERTEX>* pSimpleVS = nullptr;
      shader::CShader<EShader::E_PIXEL>*  pSimplePS = nullptr;

      // Deferred
      shader::CShader<EShader::E_VERTEX>* pStandardVS = nullptr;
      shader::CShader<EShader::E_VERTEX>* pDrawTriangle = nullptr;
      shader::CShader<EShader::E_PIXEL>*  pGBufferDeferred = nullptr;
      shader::CShader<EShader::E_PIXEL>*  pDeferredLights = nullptr;
    };

    static TRenderPipeline s_oPipeline;
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
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG
    SUCCESS_LOG("Render initialized correctly!");
  }
  // ------------------------------------
  CRender::~CRender()
  {
    // Shutdown ImGui
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // Clear constant buffer
    internal::s_oPipeline.tTransformsBuffer.Clear();
    internal::s_oPipeline.tTexturesInfoBuffer.Clear();
    internal::s_oPipeline.tInstancingModeBuffer.Clear();

    // Release depth
    global::ReleaseObject(internal::s_oPipeline.pDepthStencil);
    global::ReleaseObject(internal::s_oPipeline.pDepthTexture);

    // Release render targets
    global::ReleaseObject(internal::s_oPipeline.pDiffuseRT);
    global::ReleaseObject(internal::s_oPipeline.pNormalRT);
    global::ReleaseObject(internal::s_oPipeline.pSpecularRT);

    // Release stencils
    global::dx::SafeRelease(internal::s_oPipeline.pZPrepassStencilState);
    global::dx::SafeRelease(internal::s_oPipeline.pDeferredStencilState);
    global::dx::SafeRelease(internal::s_oPipeline.pDebugStencilState);

    // Release layouts
    global::dx::SafeRelease(internal::s_oPipeline.pStandardLayout);
    global::dx::SafeRelease(internal::s_oPipeline.pDebugLayout);

    // Release rasterizer, blending..
    global::dx::SafeRelease(internal::s_oPipeline.pRasterizer);
    global::dx::SafeRelease(internal::s_oPipeline.pBlendState);
    global::dx::SafeRelease(internal::s_oPipeline.pUserMarker);

    // Release shaders (forward)
    global::ReleaseObject(internal::s_oPipeline.pSimpleVS);
    global::ReleaseObject(internal::s_oPipeline.pSimplePS);

    // Release shaders (deferred)
    global::ReleaseObject(internal::s_oPipeline.pStandardVS);
    global::ReleaseObject(internal::s_oPipeline.pDrawTriangle);
    global::ReleaseObject(internal::s_oPipeline.pGBufferDeferred);
    global::ReleaseObject(internal::s_oPipeline.pDeferredLights);

    // Release swap chain
    global::dx::SafeRelease(internal::s_oPipeline.pSwapChain);
    global::dx::SafeRelease(internal::s_oPipeline.pBackBuffer);

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

    // Forward shaders
    internal::s_oPipeline.pSimpleVS = new shader::CShader<EShader::E_VERTEX>(g_SimpleVS, ARRAYSIZE(g_SimpleVS));
    internal::s_oPipeline.pSimplePS = new shader::CShader<EShader::E_PIXEL>(g_SimplePS, ARRAYSIZE(g_SimplePS));

    // Deferred shaders
    internal::s_oPipeline.pStandardVS = new shader::CShader<EShader::E_VERTEX>(g_StandardVS, ARRAYSIZE(g_StandardVS));
    internal::s_oPipeline.pDrawTriangle = new shader::CShader<EShader::E_VERTEX>(g_DrawTriangleVS, ARRAYSIZE(g_DrawTriangleVS));
    internal::s_oPipeline.pGBufferDeferred = new shader::CShader<EShader::E_PIXEL>(g_GBufferPS, ARRAYSIZE(g_GBufferPS));
    internal::s_oPipeline.pDeferredLights = new shader::CShader<EShader::E_PIXEL>(g_LightsPS, ARRAYSIZE(g_LightsPS));

    // Create standard layout
    hResult = global::dx::s_pDevice->CreateInputLayout
    (
      internal::s_tStandardLayout, 
      internal::s_iStandardLayoutSize,
      g_StandardVS, 
      sizeof(g_StandardVS),
      &internal::s_oPipeline.pStandardLayout
    );
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Create debug layout
    hResult = global::dx::s_pDevice->CreateInputLayout
    (
      internal::s_tPrimitivesLayout,
      internal::s_iDebugLayoutSize,
      g_SimpleVS,
      sizeof(g_SimpleVS),
      &internal::s_oPipeline.pDebugLayout
    );
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Set delegate
    utils::CDelegate<void(uint32_t, uint32_t)> oResizeDelegate(&CRender::OnWindowResizeEvent, this);
    global::delegates::s_lstOnWindowResizeDelegates.emplace_back(oResizeDelegate);

    // Init constant buffers
    hResult = InitConstantBuffers();
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Get user def
    return global::dx::s_pDeviceContext->QueryInterface
    (
      __uuidof(ID3DUserDefinedAnnotation),
      reinterpret_cast<void**>(&internal::s_oPipeline.pUserMarker)
    );
  }
  // ------------------------------------
  HRESULT CRender::InitConstantBuffers()
  {
    // Transforms buffer
    {
      HRESULT hResult = internal::s_oPipeline.tTransformsBuffer.Init();
      if (FAILED(hResult))
      {
        return hResult;
      }
      internal::s_oPipeline.tTransformsBuffer.SetSlot(0); // hlsl
    }
    // Textures info buffer
    {
      HRESULT hResult = internal::s_oPipeline.tTexturesInfoBuffer.Init();
      if (FAILED(hResult))
      {
        return hResult;
      }
      internal::s_oPipeline.tTexturesInfoBuffer.SetSlot(0); // hlsl
    }
    // Instancing buffer
    {
      HRESULT hResult = internal::s_oPipeline.tInstancingModeBuffer.Init();
      if (FAILED(hResult))
      {
        return hResult;
      }
      internal::s_oPipeline.tInstancingModeBuffer.SetSlot(1); // hlsl
    }
    return S_OK;
  }
  // ------------------------------------
  HRESULT CRender::InitBasicPipeline(uint32_t _uX, uint32_t _uY)
  {
    // Configure viewport
    ConfigureViewport(_uX, _uY);

    // Create a new depth stencil
    HRESULT hResult = CreateDepthStencilView(_uX, _uY);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Setup deferred shading
    hResult = SetupDeferredShading(_uX, _uY);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Create back buffer
    hResult = CreateBackBuffer();
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Set standard rasterizer config
    internal::s_oPipeline.tRasterizerCfg.FillMode = D3D11_FILL_SOLID;
    internal::s_oPipeline.tRasterizerCfg.CullMode = D3D11_CULL_BACK;
    internal::s_oPipeline.tRasterizerCfg.FrontCounterClockwise = false;
    internal::s_oPipeline.tRasterizerCfg.DepthBias = 0; // decals -> (10)
    internal::s_oPipeline.tRasterizerCfg.DepthBiasClamp = 0.0f;
    internal::s_oPipeline.tRasterizerCfg.SlopeScaledDepthBias = 0.0f; // decals -> (1.5f)
    internal::s_oPipeline.tRasterizerCfg.DepthClipEnable = true;
    internal::s_oPipeline.tRasterizerCfg.ScissorEnable = true;
    internal::s_oPipeline.tRasterizerCfg.MultisampleEnable = false;
    internal::s_oPipeline.tRasterizerCfg.AntialiasedLineEnable = false;

    // Create rasterizer
    hResult = CreateRasterizerState(internal::s_oPipeline.tRasterizerCfg);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Set standard blend state config
    internal::s_oPipeline.tBlendStateCfg.BlendEnable = false;
    internal::s_oPipeline.tBlendStateCfg.SrcBlend = D3D11_BLEND_ONE;
    internal::s_oPipeline.tBlendStateCfg.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    internal::s_oPipeline.tBlendStateCfg.BlendOp = D3D11_BLEND_OP_ADD;
    internal::s_oPipeline.tBlendStateCfg.SrcBlendAlpha = D3D11_BLEND_ONE;
    internal::s_oPipeline.tBlendStateCfg.DestBlendAlpha = D3D11_BLEND_ZERO;
    internal::s_oPipeline.tBlendStateCfg.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    internal::s_oPipeline.tBlendStateCfg.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    hResult = CreateBlendState(internal::s_oPipeline.tBlendStateCfg);
    if (FAILED(hResult))
    {
      return hResult;
    }

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

    D3D_FEATURE_LEVEL lstFeatureLevels[] =
    {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3,
      D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1
    };
    uint32_t uNumFeatureLevels = ARRAYSIZE(lstFeatureLevels);
    D3D_FEATURE_LEVEL oFeatureLevel = D3D_FEATURE_LEVEL();
    uint32_t uFlags = 0;

    // Create device and swap chain
    HRESULT hResult = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, uFlags, lstFeatureLevels,
      uNumFeatureLevels, D3D11_SDK_VERSION, &oSwapChainDescriptor, &internal::s_oPipeline.pSwapChain,
      &global::dx::s_pDevice, &oFeatureLevel, &global::dx::s_pDeviceContext);

    return hResult;
  }
  // ------------------------------------
  void CRender::OnWindowResizeEvent(uint32_t _uX, uint32_t _uY)
  {
    // Remove current target view
    global::dx::SafeRelease(internal::s_oPipeline.pBackBuffer);

    // Resize buffers
    HRESULT hResult = internal::s_oPipeline.pSwapChain->ResizeBuffers(0, _uX, _uY, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG

    // Init basic pipeline
    hResult = InitBasicPipeline(_uX, _uY);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG
  }
  // ------------------------------------
  HRESULT CRender::CreateBackBuffer()
  {
    ID3D11Texture2D* pTexture = nullptr;
    internal::s_oPipeline.pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture));
    if (!pTexture)
    {
      return E_FAIL;
    }

    HRESULT hResult = global::dx::s_pDevice->CreateRenderTargetView(pTexture, nullptr, &internal::s_oPipeline.pBackBuffer);
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
    global::ReleaseObject(internal::s_oPipeline.pDepthStencil);

    // Create depth stencil texture
    internal::s_oPipeline.pDepthStencil = new texture::CTexture2D<EView::DEPTH_STENCIL>();

    // Set desc
    D3D11_TEXTURE2D_DESC oTextureDesc = D3D11_TEXTURE2D_DESC();
    oTextureDesc.Width = _uX;
    oTextureDesc.Height = _uY;
    oTextureDesc.MipLevels = 1;
    oTextureDesc.ArraySize = 1;
    oTextureDesc.SampleDesc.Count = 1;
    oTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // Format
    oTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; // Depth stencil

    HRESULT hResult = internal::s_oPipeline.pDepthStencil->CreateTexture(oTextureDesc);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG

    // Set depth stencil view desc
    D3D11_DEPTH_STENCIL_VIEW_DESC oDepthStencilViewDesc = D3D11_DEPTH_STENCIL_VIEW_DESC();
    oDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    oDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    // Create the depth stencil view
    hResult = internal::s_oPipeline.pDepthStencil->CreateView(oDepthStencilViewDesc);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG

    // Create depth texture
    internal::s_oPipeline.pDepthTexture = new texture::CTexture2D<EView::SHADER_RESOURCE>();

    // Set texture config
    oTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Texture shader
    hResult = internal::s_oPipeline.pDepthTexture->CreateTexture(oTextureDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating texture!");
      return hResult;
    }

    // Creating view
    D3D11_SHADER_RESOURCE_VIEW_DESC oSRVDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
    oSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
    oSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    oSRVDesc.Texture2D.MipLevels = 1;
    hResult = internal::s_oPipeline.pDepthTexture->CreateView(oSRVDesc);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG

    // Create standard depth stencil state for zprepass
    D3D11_DEPTH_STENCIL_DESC oDepthStencilDesc = D3D11_DEPTH_STENCIL_DESC();
    oDepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    oDepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    oDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    oDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    oDepthStencilDesc.DepthEnable = true;
    oDepthStencilDesc.StencilEnable = true;

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

    // Z-Prepass
    global::dx::SafeRelease(internal::s_oPipeline.pZPrepassStencilState);
    hResult = global::dx::s_pDevice->CreateDepthStencilState(&oDepthStencilDesc, &internal::s_oPipeline.pZPrepassStencilState);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG

    // Deferred
    oDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    oDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    global::dx::SafeRelease(internal::s_oPipeline.pDeferredStencilState);
    hResult = global::dx::s_pDevice->CreateDepthStencilState(&oDepthStencilDesc, &internal::s_oPipeline.pDeferredStencilState);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG

    // Primitives (debug)
    oDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    global::dx::SafeRelease(internal::s_oPipeline.pDebugStencilState);
    return global::dx::s_pDevice->CreateDepthStencilState(&oDepthStencilDesc, &internal::s_oPipeline.pDebugStencilState);
  }
  // ------------------------------------
  HRESULT CRender::SetupDeferredShading(uint32_t _uX, uint32_t _uY)
  {
    global::ReleaseObject(internal::s_oPipeline.pDiffuseRT);
    internal::s_oPipeline.pDiffuseRT = new CRenderTarget();
    HRESULT hResult = internal::s_oPipeline.pDiffuseRT->CreateRT(_uX, _uY, DXGI_FORMAT_R8G8B8A8_UNORM);
    if (FAILED(hResult))
    {
      return hResult;
    }

    global::ReleaseObject(internal::s_oPipeline.pNormalRT);
    internal::s_oPipeline.pNormalRT = new CRenderTarget();
    hResult = internal::s_oPipeline.pNormalRT->CreateRT(_uX, _uY, DXGI_FORMAT_R16G16B16A16_FLOAT);
    if (FAILED(hResult))
    {
      return hResult;
    }

    global::ReleaseObject(internal::s_oPipeline.pSpecularRT);
    internal::s_oPipeline.pSpecularRT = new CRenderTarget();
    hResult = internal::s_oPipeline.pSpecularRT->CreateRT(_uX, _uY, DXGI_FORMAT_R8G8B8A8_UNORM);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Linear sampler
    D3D11_SAMPLER_DESC oSamplerDesc = D3D11_SAMPLER_DESC();
    oSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    oSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    oSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    oSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    oSamplerDesc.MipLODBias = 0.0f;
    oSamplerDesc.MaxAnisotropy = 1u;
    oSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    oSamplerDesc.BorderColor[0] = 0.0f;
    oSamplerDesc.BorderColor[1] = 0.0f;
    oSamplerDesc.BorderColor[2] = 0.0f;
    oSamplerDesc.BorderColor[3] = 0.0f;
    oSamplerDesc.MinLOD = 0.0f;
    oSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create sampler
    global::dx::SafeRelease(internal::s_oPipeline.pLinearSampler);
    return global::dx::s_pDevice->CreateSamplerState(&oSamplerDesc, &internal::s_oPipeline.pLinearSampler);
  }
  // ------------------------------------
  HRESULT CRender::CreateRasterizerState(const D3D11_RASTERIZER_DESC& _oRasterizerState)
  {
    // Create rasterizer state
    global::dx::SafeRelease(internal::s_oPipeline.pRasterizer);
    return global::dx::s_pDevice->CreateRasterizerState(&_oRasterizerState, &internal::s_oPipeline.pRasterizer);
  }
  // ------------------------------------
  HRESULT CRender::CreateBlendState(const D3D11_RENDER_TARGET_BLEND_DESC& _oBlendState)
  {
    // Create blend desc
    D3D11_BLEND_DESC oBlendDesc = D3D11_BLEND_DESC();
    oBlendDesc.AlphaToCoverageEnable = false;
    oBlendDesc.RenderTarget[0] = _oBlendState;

    // Create blend state
    global::dx::SafeRelease(internal::s_oPipeline.pBlendState);
    return global::dx::s_pDevice->CreateBlendState(&oBlendDesc, &internal::s_oPipeline.pBlendState);
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
  void CRender::PrepareFrame()
  {
    // Clear resources
    BeginMarker(internal::s_sPrepareFrameMrk);
    {
      // Clear back buffer
      ::global::dx::s_pDeviceContext->ClearRenderTargetView(internal::s_oPipeline.pBackBuffer, internal::s_v4ClearColor);

      // Clear RTs
      internal::s_oPipeline.pDiffuseRT->ClearRT(internal::s_v4ClearColor);
      internal::s_oPipeline.pNormalRT->ClearRT(internal::s_v4ClearColor);
      internal::s_oPipeline.pSpecularRT->ClearRT(internal::s_v4ClearColor);

      // Clear depth stencil view
      ::global::dx::s_pDeviceContext->ClearDepthStencilView(internal::s_oPipeline.pDepthStencil->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

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
      // Set constant transform
      if (m_pCamera)
      {
        // Calculate projection and invert projection
        TTransforms& rTransform = internal::s_oPipeline.tTransformsBuffer.GetData();
        math::CMatrix4x4 mViewProjection = m_pCamera->GetViewProjection();
        rTransform.ViewProjection = mViewProjection;
        rTransform.InvViewProjection = math::CMatrix4x4::Invert(mViewProjection);
        // Set near + far
        rTransform.FarPlane = m_pCamera->GetFar();
        rTransform.NearPlane = m_pCamera->GetNear();

        // Write
        bool bOk = internal::s_oPipeline.tTransformsBuffer.WriteBuffer();
        UNUSED_VAR(bOk);
#ifdef _DEBUG
        assert(bOk);
#endif
        // Bind buffer
        internal::s_oPipeline.tTransformsBuffer.Bind<render::EShader::E_VERTEX>();
      }

      // Push invalid RT
      ID3D11DepthStencilView* pDepthStencilView = internal::s_oPipeline.pDepthStencil->GetView();
      global::dx::s_pDeviceContext->OMSetRenderTargets(0, nullptr, pDepthStencilView);
      // Set input layout
      global::dx::s_pDeviceContext->IASetInputLayout(internal::s_oPipeline.pStandardLayout);
      // Set depth stencil state for zprepass
      global::dx::s_pDeviceContext->OMSetDepthStencilState(internal::s_oPipeline.pZPrepassStencilState, 1);

      // Detach simple pixel shader
      internal::s_oPipeline.pSimplePS->DetachShader();
      // Attach simple vertex shader
      internal::s_oPipeline.pStandardVS->AttachShader();
      // Set constant buffer (instancing info)
      internal::s_oPipeline.tInstancingModeBuffer.Bind<render::EShader::E_VERTEX>();

      // Cache models
      _pScene->CacheModels(m_pCamera);
      // Draw z-prepass
      _pScene->DrawModels();

      // Copy depth texture!
      ID3D11Texture2D* pTexture = internal::s_oPipeline.pDepthStencil->GetData();
      internal::s_oPipeline.pDepthTexture->CopyTexture(pTexture);
    }
    EndMarker();

    // Draw models
    BeginMarker(internal::s_sDrawModelsMrk);
    {
      // Set GBuffer RTVs
      static constexpr uint32_t uRenderTargets(3);
      ID3D11RenderTargetView* lstGBufferRTV[uRenderTargets] =
      {
        *(internal::s_oPipeline.pDiffuseRT),
        *(internal::s_oPipeline.pNormalRT),
        *(internal::s_oPipeline.pSpecularRT)
      };
      // Set render targets
      ID3D11DepthStencilView* pDepthStencilView = internal::s_oPipeline.pDepthStencil->GetView();
      global::dx::s_pDeviceContext->OMSetRenderTargets(uRenderTargets, lstGBufferRTV, pDepthStencilView);
      // Set depth stencil state
      global::dx::s_pDeviceContext->OMSetDepthStencilState(internal::s_oPipeline.pDeferredStencilState, 1);

      // Set linear sampler(read textures)
      global::dx::s_pDeviceContext->PSSetSamplers(0, 1, &internal::s_oPipeline.pLinearSampler);
      // Attach g-buffer(pixel shader)
      internal::s_oPipeline.pGBufferDeferred->AttachShader();

      // Set constant buffer (texture info)
      internal::s_oPipeline.tTexturesInfoBuffer.Bind<render::EShader::E_PIXEL>();
      // Set constant buffer (instancing info)
      internal::s_oPipeline.tInstancingModeBuffer.Bind<render::EShader::E_VERTEX>();

      // Draw models (i should separate this into 2 stages: models and instances)
      _pScene->DrawModels();

      // Remove render targets
      ID3D11RenderTargetView* lstEmptyRTs[uRenderTargets] = { nullptr, nullptr, nullptr };
      global::dx::s_pDeviceContext->OMSetRenderTargets(uRenderTargets, lstEmptyRTs, nullptr);
      // Attach triangle shader (vertex shader)
      internal::s_oPipeline.pDrawTriangle->AttachShader();
      // Attach calculate lights shader(pixel shader)
      internal::s_oPipeline.pDeferredLights->AttachShader();

      // Set transform constant
      internal::s_oPipeline.tTransformsBuffer.Bind<render::EShader::E_PIXEL>();

      // Apply lighting
      _pScene->ApplyLighting();

      // GBuffer list
      static constexpr uint32_t uTexturesSize(4);
      ID3D11ShaderResourceView* lstGBufferSRV[uTexturesSize] =
      {
        internal::s_oPipeline.pDepthTexture->GetView(),
        internal::s_oPipeline.pDiffuseRT->GetSRV(),
        internal::s_oPipeline.pNormalRT->GetSRV(),
        internal::s_oPipeline.pSpecularRT->GetSRV()
      };
      // Bind buffers
      global::dx::s_pDeviceContext->PSSetShaderResources(0, uTexturesSize, &lstGBufferSRV[0]);

      // Attach back buffer
      global::dx::s_pDeviceContext->OMSetRenderTargets(1, &internal::s_oPipeline.pBackBuffer, pDepthStencilView);
      // Draw triangle as fake quad!
      global::dx::s_pDeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
      global::dx::s_pDeviceContext->IASetInputLayout(nullptr);
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      global::dx::s_pDeviceContext->Draw(3, 0);

      // Set invalid shaders
      ID3D11ShaderResourceView* lstEmptyTextures[uTexturesSize] = { nullptr, nullptr, nullptr, nullptr };
      global::dx::s_pDeviceContext->PSSetShaderResources(0, uTexturesSize, lstEmptyTextures);
    }
    EndMarker();

    // Draw primitives (forward rendering)
    BeginMarker(internal::s_sDrawPrimitivesMrk);
    {
      // Set input layout
      global::dx::s_pDeviceContext->IASetInputLayout(internal::s_oPipeline.pDebugLayout);
      // Set depth stencil state
      global::dx::s_pDeviceContext->OMSetDepthStencilState(internal::s_oPipeline.pDebugStencilState, 1);

      // Attach shaders
      internal::s_oPipeline.pSimpleVS->AttachShader();
      internal::s_oPipeline.pSimplePS->AttachShader();

      // Draw primitives
      _pScene->DrawPrimitives(m_pCamera);
    }
    EndMarker();

    // Update resources
    global::dx::s_pDeviceContext->OMSetBlendState(internal::s_oPipeline.pBlendState, nullptr, 0xFFFFFFFF);
    global::dx::s_pDeviceContext->RSSetState(internal::s_oPipeline.pRasterizer);

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
  void CRender::SetTexturesInfo(bool _bDiffuse, bool _bNormal, bool _bSpecular)
  {
    // Set instancing mode
    TTextureInfo& rTexturesData = internal::s_oPipeline.tTexturesInfoBuffer.GetData();
    rTexturesData.HasDiffuse = static_cast<int>(_bDiffuse);
    rTexturesData.HasNormal = static_cast<int>(_bNormal);
    rTexturesData.HasSpecular = static_cast<int>(_bSpecular);
    bool bOk = internal::s_oPipeline.tTexturesInfoBuffer.WriteBuffer();
    UNUSED_VAR(bOk);
#ifdef _DEBUG
    assert(bOk);
#endif // DEBUG
  }
  // ------------------------------------
  void CRender::SetModelMatrix(const math::CMatrix4x4& _mModel)
  {
    // Set model matrix
    TTransforms& rTransform = internal::s_oPipeline.tTransformsBuffer.GetData();
    rTransform.Model = _mModel;
    bool bOk = internal::s_oPipeline.tTransformsBuffer.WriteBuffer();
    UNUSED_VAR(bOk);
#ifdef _DEBUG
    assert(bOk);
#endif // DEBUG
  }
  // ------------------------------------
  void CRender::SetInstancingMode(bool _bEnabled)
  {
    // Set instancing mode
    TInstancingMode& rData = internal::s_oPipeline.tInstancingModeBuffer.GetData();
    rData.IsInstanceMode = static_cast<int>(_bEnabled);
    bool bOk = internal::s_oPipeline.tInstancingModeBuffer.WriteBuffer();
    UNUSED_VAR(bOk);
#ifdef _DEBUG
    assert(bOk);
#endif // DEBUG
  }
  // ------------------------------------
  void CRender::SetFillMode(D3D11_FILL_MODE _eFillMode)
  {
    // Update rasterizer
    internal::s_oPipeline.tRasterizerCfg.FillMode = _eFillMode;
    CreateRasterizerState(internal::s_oPipeline.tRasterizerCfg);
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
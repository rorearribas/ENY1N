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
#include "Engine/Shaders/Deferred/ShadowsVS.h"
#include "Lights/DirectionalLight.h"

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
    static const wchar_t* s_sZPrepassMrk(L"ZPrepass");
    static const wchar_t* s_sDrawGBufferMrk(L"GBuffer");
    static const wchar_t* s_sComputeShadowsMrk(L"ShadowMapping");
    static const wchar_t* s_sDrawPrimitivesMrk(L"Primitives");
    static const wchar_t* s_sImGuiMarker(L"ImGui");

    static const float s_v4ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static const float s_fMinDepth(0.0f);
    static const float s_fMaxDepth(1.0f);

    // Standard Input
    static constexpr int s_iStandardLayoutSize(7);
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
    static constexpr int s_iPrimitiveLayoutSize(6);
    static const D3D11_INPUT_ELEMENT_DESC s_tPrimitivesLayout[s_iPrimitiveLayoutSize] =
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
      ID3D11SamplerState* pLinearSampler = nullptr;
      render::CRenderTarget rDiffuseRT;
      render::CRenderTarget rNormalRT;
      render::CRenderTarget rSpecularRT;

      // Constant buffers
      CConstantBuffer<TTransforms> tTransformsBuffer;
      static constexpr uint32_t uTransformSlot = 0;

      CConstantBuffer<TLightView> tLightViewBuffer;
      static constexpr uint32_t uLightViewSlot = 0;

      CConstantBuffer<TMaterialInfo> tMaterialInfoBuffer;
      static constexpr uint32_t uMaterialInfoSlot = 0;

      // Depth
      texture::TDepthStencil rDepthStencil;
      texture::TShaderResource rDepthTexture;

      // Stencils
      ID3D11DepthStencilState* pZPrepassStencilState = nullptr;
      ID3D11DepthStencilState* pDeferredStencilState = nullptr;
      ID3D11DepthStencilState* pDebugStencilState = nullptr;

      // Rasterizer
      ID3D11RasterizerState* pRasterizer = nullptr;
      D3D11_RASTERIZER_DESC rRasterizerCfg = D3D11_RASTERIZER_DESC();

      // Blend
      ID3D11BlendState* pBlendState = nullptr;
      D3D11_RENDER_TARGET_BLEND_DESC rBlendStateCfg = D3D11_RENDER_TARGET_BLEND_DESC();

      // Layouts
      ID3D11InputLayout* pStandardLayout = nullptr;
      ID3D11InputLayout* pDebugLayout = nullptr;

      // Debug
      ID3DUserDefinedAnnotation* pUserMarker = nullptr;

      // Forward
      shader::CShader<EShader::E_VERTEX> rForwardVS;
      shader::CShader<EShader::E_PIXEL> rForwardPS;

      // Deferred
      shader::CShader<EShader::E_VERTEX> rDrawTriangleVS;
      shader::CShader<EShader::E_VERTEX> rDeferredShadowsVS;
      shader::CShader<EShader::E_VERTEX> rDeferredVS;

      shader::CShader<EShader::E_PIXEL> rDeferredGBuffer;
      shader::CShader<EShader::E_PIXEL> rDeferredLights;
    };

    static TRenderPipeline s_oPipeline;
    static TTransforms s_oTransforms;
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
    internal::s_oPipeline.tTransformsBuffer.Release();
    internal::s_oPipeline.tLightViewBuffer.Release();
    internal::s_oPipeline.tMaterialInfoBuffer.Release();

    // Release depth textures
    internal::s_oPipeline.rDepthStencil.Release();
    internal::s_oPipeline.rDepthTexture.Release();

    // Release render targets
    internal::s_oPipeline.rDiffuseRT.Release();
    internal::s_oPipeline.rNormalRT.Release();
    internal::s_oPipeline.rSpecularRT.Release();

    // Release shaders (forward)
    internal::s_oPipeline.rForwardVS.Release();
    internal::s_oPipeline.rForwardPS.Release();

    // Release shaders (deferred)
    internal::s_oPipeline.rDeferredVS.Release();
    internal::s_oPipeline.rDrawTriangleVS.Release();
    internal::s_oPipeline.rDeferredShadowsVS.Release();
    internal::s_oPipeline.rDeferredGBuffer.Release();
    internal::s_oPipeline.rDeferredLights.Release();

    // Release stencils
    global::dx::SafeRelease(internal::s_oPipeline.pZPrepassStencilState);
    global::dx::SafeRelease(internal::s_oPipeline.pDeferredStencilState);
    global::dx::SafeRelease(internal::s_oPipeline.pDebugStencilState);

    // Release layouts
    global::dx::SafeRelease(internal::s_oPipeline.pStandardLayout);
    global::dx::SafeRelease(internal::s_oPipeline.pDebugLayout);

    // Release rasterizer, blending..
    global::dx::SafeRelease(internal::s_oPipeline.pLinearSampler);
    global::dx::SafeRelease(internal::s_oPipeline.pRasterizer);
    global::dx::SafeRelease(internal::s_oPipeline.pBlendState);
    global::dx::SafeRelease(internal::s_oPipeline.pUserMarker);

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
      ERROR_LOG("Error creating device!");
      return hResult;
    }

    // Setup basic pipeline
    hResult = InitPipeline(_uX, _uY);
    if (FAILED(hResult))
    {
      return hResult;
    }

    hResult = SetupLayouts();
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating layouts!");
      return hResult;
    }

    // Init constant buffers
    hResult = InitConstantBuffers();
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating constant buffers!");
      return hResult;
    }

    // Init shaders
    hResult = InitShaders();
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating shaders!");
      return hResult;
    }

    // Init ImGui
    if (!InitImGui())
    {
      ERROR_LOG("Error initializing imgui!");
      return E_FAIL;
    }

    // Set delegate
    utils::CDelegate<void(uint32_t, uint32_t)> rDelegate(&CRender::OnWindowResizeEvent, this);
    global::delegates::s_lstOnWindowResizeDelegates.emplace_back(rDelegate);

    // Get user def
    return global::dx::s_pDeviceContext->QueryInterface
    (
      __uuidof(ID3DUserDefinedAnnotation),
      reinterpret_cast<void**>(&internal::s_oPipeline.pUserMarker)
    );
  }
  // ------------------------------------
  HRESULT CRender::InitShaders()
  {
    // Forward shaders
    internal::s_oPipeline.rForwardVS.Release();
    HRESULT hResult = internal::s_oPipeline.rForwardVS.Init(g_SimpleVS, ARRAYSIZE(g_SimpleVS));
    if (FAILED(hResult))
    {
      return hResult;
    }

    internal::s_oPipeline.rForwardPS.Release();
    hResult = internal::s_oPipeline.rForwardPS.Init(g_SimplePS, ARRAYSIZE(g_SimplePS));
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Deferred shaders
    internal::s_oPipeline.rDeferredVS.Release();
    hResult = internal::s_oPipeline.rDeferredVS.Init(g_StandardVS, ARRAYSIZE(g_StandardVS));
    if (FAILED(hResult))
    {
      return hResult;
    }

    internal::s_oPipeline.rDeferredShadowsVS.Release();
    hResult = internal::s_oPipeline.rDeferredShadowsVS.Init(g_ShadowsVS, ARRAYSIZE(g_ShadowsVS));
    if (FAILED(hResult))
    {
      return hResult;
    }

    internal::s_oPipeline.rDrawTriangleVS.Release();
    hResult = internal::s_oPipeline.rDrawTriangleVS.Init(g_DrawTriangleVS, ARRAYSIZE(g_DrawTriangleVS));
    if (FAILED(hResult))
    {
      return hResult;
    }

    internal::s_oPipeline.rDeferredGBuffer.Release();
    hResult = internal::s_oPipeline.rDeferredGBuffer.Init(g_GBufferPS, ARRAYSIZE(g_GBufferPS));
    if (FAILED(hResult))
    {
      return hResult;
    }

    internal::s_oPipeline.rDeferredLights.Release();
    return internal::s_oPipeline.rDeferredLights.Init(g_LightsPS, ARRAYSIZE(g_LightsPS));
  }
  // ------------------------------------
  HRESULT CRender::InitConstantBuffers()
  {
    // Transforms buffer
    HRESULT hResult = internal::s_oPipeline.tTransformsBuffer.Init();
    if (FAILED(hResult))
    {
      return hResult;
    }
    // Light view buffer
    hResult = internal::s_oPipeline.tLightViewBuffer.Init();
    if (FAILED(hResult))
    {
      return hResult;
    }
    // Material info buffer
    return internal::s_oPipeline.tMaterialInfoBuffer.Init();
  }
  // ------------------------------------
  HRESULT CRender::SetupLayouts()
  {
    // Create standard layout
    HRESULT hResult = global::dx::s_pDevice->CreateInputLayout
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
    return global::dx::s_pDevice->CreateInputLayout
    (
      internal::s_tPrimitivesLayout,
      internal::s_iPrimitiveLayoutSize,
      g_SimpleVS,
      sizeof(g_SimpleVS),
      &internal::s_oPipeline.pDebugLayout
    );
  }
  // ------------------------------------
  HRESULT CRender::InitPipeline(uint32_t _uX, uint32_t _uY)
  {
    // Configure viewport
    SetViewport(_uX, _uY);

    // Setup depth stencils
    HRESULT hResult = SetupDepthStencils(_uX, _uY);
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
    internal::s_oPipeline.rRasterizerCfg.FillMode = D3D11_FILL_SOLID;
    internal::s_oPipeline.rRasterizerCfg.CullMode = D3D11_CULL_BACK;
    internal::s_oPipeline.rRasterizerCfg.FrontCounterClockwise = false;
    internal::s_oPipeline.rRasterizerCfg.DepthBias = 10; // decals -> (10)
    internal::s_oPipeline.rRasterizerCfg.DepthBiasClamp = 0.0f;
    internal::s_oPipeline.rRasterizerCfg.SlopeScaledDepthBias = 0.0f; // decals -> (1.5f)
    internal::s_oPipeline.rRasterizerCfg.DepthClipEnable = true;
    internal::s_oPipeline.rRasterizerCfg.ScissorEnable = true;
    internal::s_oPipeline.rRasterizerCfg.MultisampleEnable = false;
    internal::s_oPipeline.rRasterizerCfg.AntialiasedLineEnable = false;

    // Create rasterizer
    hResult = CreateRasterizerState(internal::s_oPipeline.rRasterizerCfg);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Set standard blend state config
    internal::s_oPipeline.rBlendStateCfg.BlendEnable = false;
    internal::s_oPipeline.rBlendStateCfg.SrcBlend = D3D11_BLEND_ONE;
    internal::s_oPipeline.rBlendStateCfg.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    internal::s_oPipeline.rBlendStateCfg.BlendOp = D3D11_BLEND_OP_ADD;
    internal::s_oPipeline.rBlendStateCfg.SrcBlendAlpha = D3D11_BLEND_ONE;
    internal::s_oPipeline.rBlendStateCfg.DestBlendAlpha = D3D11_BLEND_ZERO;
    internal::s_oPipeline.rBlendStateCfg.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    internal::s_oPipeline.rBlendStateCfg.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    hResult = CreateBlendState(internal::s_oPipeline.rBlendStateCfg);
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
    DXGI_SWAP_CHAIN_DESC rSwapChainDescriptor = DXGI_SWAP_CHAIN_DESC();
    rSwapChainDescriptor.BufferCount = 1;
    rSwapChainDescriptor.BufferDesc.Width = _uX;
    rSwapChainDescriptor.BufferDesc.Height = _uY;
    rSwapChainDescriptor.OutputWindow = m_pRenderWindow->GetHwnd();
    rSwapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rSwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    rSwapChainDescriptor.SampleDesc.Count = 1;
    rSwapChainDescriptor.SampleDesc.Quality = 0;
    rSwapChainDescriptor.Windowed = TRUE;

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
    return D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, uFlags, lstFeatureLevels,
      uNumFeatureLevels, D3D11_SDK_VERSION, &rSwapChainDescriptor, &internal::s_oPipeline.pSwapChain,
      &global::dx::s_pDevice, &oFeatureLevel, &global::dx::s_pDeviceContext);
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

    // Init pipeline
    hResult = InitPipeline(_uX, _uY);
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
  HRESULT CRender::SetupDepthStencils(uint32_t _uX, uint32_t _uY)
  {
    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC rTextureDesc = D3D11_TEXTURE2D_DESC();
    rTextureDesc.Width = _uX;
    rTextureDesc.Height = _uY;
    rTextureDesc.MipLevels = 1;
    rTextureDesc.ArraySize = 1;
    rTextureDesc.SampleDesc.Count = 1;
    rTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // Format
    rTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; // Depth stencil

    internal::s_oPipeline.rDepthStencil.Release();
    HRESULT hResult = internal::s_oPipeline.rDepthStencil.CreateTexture(rTextureDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating depth stencil texture!");
      return hResult;
    }

    // Set depth stencil view desc
    D3D11_DEPTH_STENCIL_VIEW_DESC rDepthStencilViewDesc = D3D11_DEPTH_STENCIL_VIEW_DESC();
    rDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    rDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    // Create the depth stencil view
    hResult = internal::s_oPipeline.rDepthStencil.CreateView(rDepthStencilViewDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating stencil view!");
      return hResult;
    }

    // Creating view from texture
    D3D11_SHADER_RESOURCE_VIEW_DESC rSRVDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
    rSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
    rSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    rSRVDesc.Texture2D.MipLevels = 1;

    internal::s_oPipeline.rDepthTexture.Release();
    ID3D11Texture2D* pStencilTexture = internal::s_oPipeline.rDepthStencil;
    hResult = internal::s_oPipeline.rDepthTexture.CreateViewFromTexture(pStencilTexture, rSRVDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating view!");
      return hResult;
    }

    // Create standard depth stencil state for zprepass
    D3D11_DEPTH_STENCIL_DESC rDepthStencilDesc = D3D11_DEPTH_STENCIL_DESC();
    rDepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    rDepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    rDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    rDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    rDepthStencilDesc.DepthEnable = true;
    rDepthStencilDesc.StencilEnable = false;

    // Front-face
    rDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    rDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    rDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    rDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Back-face
    rDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    rDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    rDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    rDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Z-Prepass
    global::dx::SafeRelease(internal::s_oPipeline.pZPrepassStencilState);
    hResult = global::dx::s_pDevice->CreateDepthStencilState(&rDepthStencilDesc, &internal::s_oPipeline.pZPrepassStencilState);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating zprepass stencil state!");
      return hResult;
    }

    // Deferred
    global::dx::SafeRelease(internal::s_oPipeline.pDeferredStencilState);
    rDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    rDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    hResult = global::dx::s_pDevice->CreateDepthStencilState(&rDepthStencilDesc, &internal::s_oPipeline.pDeferredStencilState);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating deferred stencil state!");
      return hResult;
    }

    // Primitives (debug)
    global::dx::SafeRelease(internal::s_oPipeline.pDebugStencilState);
    rDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    rDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    return global::dx::s_pDevice->CreateDepthStencilState(&rDepthStencilDesc, &internal::s_oPipeline.pDebugStencilState);
  }
  // ------------------------------------
  HRESULT CRender::SetupDeferredShading(uint32_t _uX, uint32_t _uY)
  {
    internal::s_oPipeline.rDiffuseRT.Release();
    HRESULT hResult = internal::s_oPipeline.rDiffuseRT.CreateRT(_uX, _uY, DXGI_FORMAT_R8G8B8A8_UNORM);
    if (FAILED(hResult))
    {
      return hResult;
    }

    internal::s_oPipeline.rNormalRT.Release();
    hResult = internal::s_oPipeline.rNormalRT.CreateRT(_uX, _uY, DXGI_FORMAT_R16G16B16A16_FLOAT);
    if (FAILED(hResult))
    {
      return hResult;
    }

    internal::s_oPipeline.rSpecularRT.Release();
    hResult = internal::s_oPipeline.rSpecularRT.CreateRT(_uX, _uY, DXGI_FORMAT_R8G8B8A8_UNORM);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Linear sampler
    D3D11_SAMPLER_DESC rSamplerDesc = D3D11_SAMPLER_DESC();
    rSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    rSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    rSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    rSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    rSamplerDesc.MipLODBias = 0.0f;
    rSamplerDesc.MaxAnisotropy = 1u;
    rSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    rSamplerDesc.BorderColor[0] = 0.0f;
    rSamplerDesc.BorderColor[1] = 0.0f;
    rSamplerDesc.BorderColor[2] = 0.0f;
    rSamplerDesc.BorderColor[3] = 0.0f;
    rSamplerDesc.MinLOD = 0.0f;
    rSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create sampler
    global::dx::SafeRelease(internal::s_oPipeline.pLinearSampler);
    return global::dx::s_pDevice->CreateSamplerState(&rSamplerDesc, &internal::s_oPipeline.pLinearSampler);
  }
  // ------------------------------------
  HRESULT CRender::CreateRasterizerState(const D3D11_RASTERIZER_DESC& _rRasterizerState)
  {
    // Create rasterizer state
    global::dx::SafeRelease(internal::s_oPipeline.pRasterizer);
    return global::dx::s_pDevice->CreateRasterizerState(&_rRasterizerState, &internal::s_oPipeline.pRasterizer);
  }
  // ------------------------------------
  HRESULT CRender::CreateBlendState(const D3D11_RENDER_TARGET_BLEND_DESC& _rBlendState)
  {
    // Create blend desc
    D3D11_BLEND_DESC oBlendDesc = D3D11_BLEND_DESC();
    oBlendDesc.AlphaToCoverageEnable = false;
    oBlendDesc.RenderTarget[0] = _rBlendState;

    // Create blend state
    global::dx::SafeRelease(internal::s_oPipeline.pBlendState);
    return global::dx::s_pDevice->CreateBlendState(&oBlendDesc, &internal::s_oPipeline.pBlendState);
  }
  // ------------------------------------
  void CRender::DrawModels(scene::CScene* _pScene)
  {
    // Bind buffer
    internal::s_oPipeline.tTransformsBuffer.Bind<render::EShader::E_VERTEX>(internal::s_oPipeline.uTransformSlot);
    global::dx::s_pDeviceContext->IASetInputLayout(internal::s_oPipeline.pStandardLayout);

    // Detach simple pixel shader
    internal::s_oPipeline.rForwardPS.Detach();
    // Attach deferred vertex shader
    internal::s_oPipeline.rDeferredVS.Attach();

    // Set GBuffer RTVs
    static constexpr uint32_t uRenderTargets(3);
    ID3D11RenderTargetView* lstGBufferRTV[uRenderTargets] =
    {
      internal::s_oPipeline.rDiffuseRT,
      internal::s_oPipeline.rNormalRT,
      internal::s_oPipeline.rSpecularRT
    };

    // Set render targets
    ID3D11DepthStencilView* pDepthStencilView = internal::s_oPipeline.rDepthStencil.GetView();
    global::dx::s_pDeviceContext->OMSetRenderTargets(uRenderTargets, lstGBufferRTV, pDepthStencilView);
    // Set depth stencil state
    global::dx::s_pDeviceContext->OMSetDepthStencilState(internal::s_oPipeline.pZPrepassStencilState, 1);

    // Set linear sampler(read textures)
    global::dx::s_pDeviceContext->PSSetSamplers(0, 1, &internal::s_oPipeline.pLinearSampler);
    // Attach g-buffer(pixel shader)
    internal::s_oPipeline.rDeferredGBuffer.Attach();
    // Set constant buffer (texture info)
    internal::s_oPipeline.tMaterialInfoBuffer.Bind<render::EShader::E_PIXEL>(internal::s_oPipeline.uMaterialInfoSlot);

    // Draw models
    _pScene->DrawModels(this);

    // Remove render targets
    ID3D11RenderTargetView* lstEmptyRTs[uRenderTargets] = { nullptr, nullptr, nullptr };
    global::dx::s_pDeviceContext->OMSetRenderTargets(uRenderTargets, lstEmptyRTs, nullptr);
  }
  // ------------------------------------
  void CRender::DrawPrimitives(scene::CScene* _pScene)
  {
    // Set input layout
    global::dx::s_pDeviceContext->IASetInputLayout(internal::s_oPipeline.pDebugLayout);
    // Set depth stencil state
    global::dx::s_pDeviceContext->OMSetDepthStencilState(internal::s_oPipeline.pDebugStencilState, 1);

    // Attach shaders
    internal::s_oPipeline.rForwardVS.Attach();
    internal::s_oPipeline.rForwardPS.Attach();

    // Bind buffer
    internal::s_oPipeline.tTransformsBuffer.Bind<render::EShader::E_VERTEX>(internal::s_oPipeline.uTransformSlot);

    // Draw primitives
    _pScene->DrawPrimitives(m_pCamera);
  }
  // ------------------------------------
  void CRender::DrawGBuffer()
  {
    // Attach triangle shader (vertex shader)
    internal::s_oPipeline.rDrawTriangleVS.Attach();
    // Attach calculate lights shader(pixel shader)
    internal::s_oPipeline.rDeferredLights.Attach();

    // Set transform constant
    internal::s_oPipeline.tTransformsBuffer.Bind<render::EShader::E_PIXEL>(internal::s_oPipeline.uTransformSlot);

    static constexpr uint32_t uTexturesSize(4);
    ID3D11ShaderResourceView* lstGBufferSRV[uTexturesSize] =
    {
      internal::s_oPipeline.rDepthTexture.GetView(),
      internal::s_oPipeline.rDiffuseRT.GetView(),
      internal::s_oPipeline.rNormalRT.GetView(),
      internal::s_oPipeline.rSpecularRT.GetView()
    };

    // Bind buffers
    global::dx::s_pDeviceContext->PSSetShaderResources(0, uTexturesSize, &lstGBufferSRV[0]);
    global::dx::s_pDeviceContext->OMSetRenderTargets(1, &internal::s_oPipeline.pBackBuffer, nullptr);

    // Draw triangle as fake quad!
    global::dx::s_pDeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    global::dx::s_pDeviceContext->IASetInputLayout(nullptr);
    global::dx::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    global::dx::s_pDeviceContext->Draw(3, 0);

    // Set invalid shaders
    ID3D11ShaderResourceView* lstEmptyTextures[uTexturesSize] = { nullptr, nullptr, nullptr, nullptr };
    global::dx::s_pDeviceContext->PSSetShaderResources(0, uTexturesSize, lstEmptyTextures);

    // Attach back buffer
    ID3D11DepthStencilView* pDepthStencilView = internal::s_oPipeline.rDepthStencil.GetView();
    global::dx::s_pDeviceContext->OMSetRenderTargets(1, &internal::s_oPipeline.pBackBuffer, pDepthStencilView);
  }
  // ------------------------------------
  void CRender::SetViewport(uint32_t _uX, uint32_t _uY)
  {
    if (!global::dx::s_pDeviceContext || !m_pRenderWindow)
    {
      ERROR_LOG("Rendering window or context is not valid!");
      return;
    }

    D3D11_VIEWPORT rViewport = D3D11_VIEWPORT();
    {
      rViewport.Width = static_cast<float>(_uX);
      rViewport.Height = static_cast<float>(_uY);
      rViewport.MinDepth = internal::s_fMinDepth;
      rViewport.MaxDepth = internal::s_fMaxDepth;
    }
    global::dx::s_pDeviceContext->RSSetViewports(1, &rViewport);
  }
  // ------------------------------------
  void CRender::PrepareFrame()
  {
    // Clear resources
    BeginMarker(internal::s_sPrepareFrameMrk);
    {
      // Clear back buffer
      global::dx::s_pDeviceContext->ClearRenderTargetView(internal::s_oPipeline.pBackBuffer, internal::s_v4ClearColor);

      // Clear RTs
      internal::s_oPipeline.rDiffuseRT.ClearRT(internal::s_v4ClearColor);
      internal::s_oPipeline.rNormalRT.ClearRT(internal::s_v4ClearColor);
      internal::s_oPipeline.rSpecularRT.ClearRT(internal::s_v4ClearColor);

      // Clear depth stencil view
      global::dx::s_pDeviceContext->ClearDepthStencilView(internal::s_oPipeline.rDepthStencil.GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

      // Prepare ImGu
      ImGui_ImplDX11_NewFrame();
      ImGui_ImplWin32_NewFrame();
      ImGui::NewFrame();

      // ImGuizmo
      ImGuizmo::BeginFrame();
    }
    EndMarker();
  }
  // ------------------------------------
  void CRender::Draw(scene::CScene* _pScene)
  {
    // Calculate projection and invert projection
#ifdef _DEBUG
    assert(m_pCamera);
#endif
    math::CMatrix4x4 mViewProjection = m_pCamera->GetViewProjection();
    internal::s_oTransforms.ViewProjection = mViewProjection;
    internal::s_oTransforms.InvViewProjection = math::CMatrix4x4::Invert(mViewProjection);

    // Set near + far
    internal::s_oTransforms.FarPlane = m_pCamera->GetFar();
    internal::s_oTransforms.NearPlane = m_pCamera->GetNear();

    // Write
    bool bOk = internal::s_oPipeline.tTransformsBuffer.WriteBuffer(internal::s_oTransforms);
    UNUSED_VAR(bOk);
#ifdef _DEBUG
    assert(bOk);
#endif

    // Cache models
    _pScene->CacheModels(m_pCamera);

    // Draw models
    BeginMarker(internal::s_sDrawGBufferMrk);
    {
      // Draw models
      DrawModels(_pScene);

      // Compute lighting
      render::lights::CLightManager* pLightManager = _pScene->GetLightManager();
      BeginMarker(internal::s_sComputeShadowsMrk);
      {
        const lights::CLightManager::TShadowMaps& lstShadowMaps = pLightManager->GetShadowMaps();
        if (lstShadowMaps.GetSize() > 0)
        {
          BeginMarker(internal::s_sZPrepassMrk);
          {
            // Clear depth stencil view
            const lights::CLightManager::TShadowMap& rShadowMap = *lstShadowMaps[0];
            const texture::TDepthStencil& rShadowDepth = rShadowMap.ShadowDepth;
            global::dx::s_pDeviceContext->ClearDepthStencilView(rShadowDepth.GetView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

            // Configure viewport
            uint32_t uWidth, uHeight;
            rShadowDepth.GetTextureSize(uWidth, uHeight);
            SetViewport(uWidth, uHeight);

            // Create orthographic
            float fHeight = 100.0f;
            float fWidth = fHeight * static_cast<float>(uWidth / static_cast<float>(uHeight));
            math::CMatrix4x4 mOrthographic = math::CMatrix4x4::CreateOrtographicMatrix(fWidth, fHeight, m_pCamera->GetNear(), m_pCamera->GetFar());

            // Create view matrix from directional light
            math::CVector3 v3SceneCenter = m_pCamera->GetPos() + (m_pCamera->GetDir() * 25.0f); // Max distance
            math::CVector3 v3ShadowPos = v3SceneCenter - (pLightManager->GetDirectionalLight()->GetDir() * 50.0f);
            math::CMatrix4x4 mViewMatrix = math::CMatrix4x4::LookAt(v3ShadowPos, v3SceneCenter, render::CRender::s_v3WorldUp);

            TLightView rLightView = TLightView();
            rLightView.LightViewProjection = mOrthographic * mViewMatrix;

            bOk = internal::s_oPipeline.tLightViewBuffer.WriteBuffer(rLightView);
#ifdef _DEBUG
            assert(bOk);
#endif // DEBU
            internal::s_oPipeline.tLightViewBuffer.Bind<render::EShader::E_VERTEX>(internal::s_oPipeline.uTransformSlot);

            // Set render target
            global::dx::s_pDeviceContext->OMSetRenderTargets(0, nullptr, rShadowDepth.GetView());
            // Set depth stencil state
            global::dx::s_pDeviceContext->OMSetDepthStencilState(internal::s_oPipeline.pZPrepassStencilState, 1);

            // Attach vertex shader for shadows (vertex shader)
            internal::s_oPipeline.rDeferredShadowsVS.Attach();
            // Detach pixel shader for models
            internal::s_oPipeline.rDeferredGBuffer.Detach();

            // Draw models only in zprepass for the light perspective
            _pScene->DrawModels(this);

            uint32_t uRenderWidth = m_pRenderWindow->GetWidth();
            uint32_t uRenderHeight = m_pRenderWindow->GetHeight();
            SetViewport(uRenderWidth, uRenderHeight);
          }
          EndMarker();
        }
      }
      EndMarker();

      // Compute lighting
      pLightManager->ApplyLighting();

      // Draw GBuffer
      DrawGBuffer();
    }
    EndMarker();

    // Draw primitives (forward rendering)
    BeginMarker(internal::s_sDrawPrimitivesMrk);
    {
      DrawPrimitives(_pScene);
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
  void CRender::PushMaterialInfo(const render::mat::CMaterial* _pMaterial)
  {
    if (_pMaterial)
    {
      // Set material data
      TMaterialInfo rMaterialInfo = TMaterialInfo();

      rMaterialInfo.DiffuseColor = _pMaterial->GetDiffuseColor();
      rMaterialInfo.SpecularColor = _pMaterial->GetSpecularColor();

      rMaterialInfo.HasDiffuseTexture = static_cast<bool>(_pMaterial->GetTexture(render::ETexture::DIFFUSE));
      rMaterialInfo.HasNormalTexture = static_cast<bool>(_pMaterial->GetTexture(render::ETexture::NORMAL));
      rMaterialInfo.HasSpecularTexture = static_cast<bool>(_pMaterial->GetTexture(render::ETexture::SPECULAR));

      bool bOk = internal::s_oPipeline.tMaterialInfoBuffer.WriteBuffer(rMaterialInfo);
      UNUSED_VAR(bOk);
#ifdef _DEBUG
      assert(bOk);
#endif // DEBUG
    }
  }
  // ------------------------------------
  void CRender::SetFillMode(D3D11_FILL_MODE _eFillMode)
  {
    // Update rasterizer
    internal::s_oPipeline.rRasterizerCfg.FillMode = _eFillMode;
    CreateRasterizerState(internal::s_oPipeline.rRasterizerCfg);
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
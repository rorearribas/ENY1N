#include "Render.h"

#include "Engine/Global/GlobalResources.h"
#include "Engine/Render/Window/RenderWindow.h"
#include "Engine/Render/Buffers/BufferTypes.h"
#include "Engine/Render/Buffers/ConstantBuffer.h"
#include "Engine/Render/Graphics/ShadowMap.h"
#include "Engine/Render/Resources/RenderTarget.h"
#include "RenderTypes.h"

// Debug
#include "Engine/Shaders/Forward/SimpleVS.h"
#include "Engine/Shaders/Forward/SimplePS.h"

// Deferred
#include "Engine/Shaders/Deferred/StandardVS.h"
#include "Engine/Shaders/Deferred/LightsPS.h"
#include "Engine/Shaders/Deferred/GBufferPS.h"
#include "Engine/Shaders/Deferred/DrawQuadVS.h"
#include "Engine/Shaders/Deferred/ShadowsVS.h"
#include "Lighting/DirectionalLight.h"

// Renderers
#include "Engine/Render/Renderers/DeferredRenderer.h"
#include "Engine/Render/Renderers/LightingRenderer.h"
#include "Engine/Render/Renderers/ForwardRenderer.h"
#include "Engine/Render/Renderers/ImGuiRenderer.h"

// ImGui
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/ImGui/imgui_impl_win32.h"
#include "Libs/ImGui/imgui_impl_dx11.h"
#include "Libs/ImGui/ImGuizmo.h"
#include <d3d11.h>

namespace render
{
  const math::CVector3 CRender::s_v3WorldUp(0.0f, 1.0f, 0.0f);

  namespace internal
  {
    static const wchar_t* s_sPrepareFrameMrk(L"Clear");
    static const wchar_t* s_sZPrepassMrk(L"ZPrepass");
    static const wchar_t* s_sDeferredPassMrk(L"Deferred");
    static const wchar_t* s_sForwardPassMark(L"Forward");
    static const wchar_t* s_sComputeShadowsMrk(L"ShadowMapping");
    static const wchar_t* s_sDrawPrimitivesMrk(L"Primitives");
    static const wchar_t* s_sImGuiMarker(L"ImGui");

    static const float s_v4ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static const float s_fMinDepth(0.0f);
    static const float s_fMaxDepth(1.0f);

    // Standard layout - VTX(36) / INST(64)
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

    // Debug layout - VTX(12) / INST(64)
    static constexpr int s_iPrimitiveLayoutSize(6);
    static const D3D11_INPUT_ELEMENT_DESC s_tPrimitivesLayout[s_iPrimitiveLayoutSize] =
    {
      // Vertex layout
      { "VERTEXPOS",          0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 12
      // Instancing
      { "INSTANCE_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 16
      { "INSTANCE_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 32
      { "INSTANCE_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 48
      { "INSTANCE_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 64
      { "COLOR",              0, DXGI_FORMAT_R32G32B32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 68
    };

    struct TRenderPipeline
    {
      // Swap chain + back buffer
      IDXGISwapChain* SwapChain = nullptr;
      ID3D11RenderTargetView* BackBuffer = nullptr;

      // Samplers
      ID3D11SamplerState* LinearSampler = nullptr;
      ID3D11SamplerState* ShadowSampler = nullptr;

      // Instancing buffers
      ID3D11Buffer* RenderInstancesBuffer = nullptr;
      ID3D11Buffer* PrimitiveInstancesBuffer = nullptr;

      // Global constant buffers
      CConstantBuffer<TCameraTransform> CameraTransformBuffer;
      static constexpr uint32_t CameraTransformSlot = 0;
      CConstantBuffer<TCameraTransform> LightingViewBuffer;
      static constexpr uint32_t LightingViewSlot = 2;
      CConstantBuffer<TMaterialInfo> MaterialBuffer;
      static constexpr uint32_t MaterialSlot = 0;

      // Depth
      ID3D11DepthStencilState* DepthStencilState = nullptr;
      texture::TDepthStencil DepthStencil;
      texture::TShaderResource DepthTexture;

      // Rasterizer
      ID3D11RasterizerState* DefaultRasterizer = nullptr;
      D3D11_RASTERIZER_DESC RasterizerCfg = D3D11_RASTERIZER_DESC();

      // Blend
      ID3D11BlendState* BlendState = nullptr;
      D3D11_RENDER_TARGET_BLEND_DESC BlendStateCfg = D3D11_RENDER_TARGET_BLEND_DESC();

      // Layouts
      ID3D11InputLayout* StandardLayout = nullptr;
      ID3D11InputLayout* DebugLayout = nullptr;

      // Debug
      ID3DUserDefinedAnnotation* pUserMarker = nullptr;

      // Forward
      shader::CShader<EShader::E_VERTEX> ForwardVS;
      shader::CShader<EShader::E_PIXEL> ForwardPS;

      // Deferred
      render::shader::CShader<EShader::E_VERTEX> DrawQuadVS;
      render::shader::CShader<EShader::E_VERTEX> DeferredShadowsVS;
      render::shader::CShader<EShader::E_VERTEX> DeferredVS;

      render::shader::CShader<EShader::E_PIXEL> DeferredGBufferPS;
      render::shader::CShader<EShader::E_PIXEL> DeferredLightsPS;
    };

    static TRenderPipeline Pipeline;
  }

  // ------------------------------------
  CRender::CRender(uint32_t _uWidth, uint32_t _uHeight)
  {
    // Create render window
    LOG("Creating render window...");
    m_pRenderWindow = std::make_unique<render::CRenderWindow>(_uWidth, _uHeight);
    SUCCESS_LOG("The window has been created successfully!");

    // Init render
    LOG("Initializing render...");
    HRESULT hResult = Init(_uWidth, _uHeight);
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
    internal::Pipeline.CameraTransformBuffer.Release();
    internal::Pipeline.LightingViewBuffer.Release();
    internal::Pipeline.MaterialBuffer.Release();

    // Release depth textures
    internal::Pipeline.DepthStencil.Release();
    internal::Pipeline.DepthTexture.Release();
    // Release shaders (forward)
    internal::Pipeline.ForwardVS.Release();
    internal::Pipeline.ForwardPS.Release();

    // Release shaders (deferred)
    internal::Pipeline.DeferredVS.Release();
    internal::Pipeline.DrawQuadVS.Release();
    internal::Pipeline.DeferredShadowsVS.Release();
    internal::Pipeline.DeferredGBufferPS.Release();
    internal::Pipeline.DeferredLightsPS.Release();

    // Layout + states
    global::api::SafeRelease(internal::Pipeline.StandardLayout);
    global::api::SafeRelease(internal::Pipeline.DebugLayout);

    // Release rasterizer, blending..
    global::api::SafeRelease(internal::Pipeline.LinearSampler);
    global::api::SafeRelease(internal::Pipeline.ShadowSampler);
    global::api::SafeRelease(internal::Pipeline.DefaultRasterizer);
    global::api::SafeRelease(internal::Pipeline.BlendState);
    global::api::SafeRelease(internal::Pipeline.pUserMarker);

    global::api::SafeRelease(internal::Pipeline.RenderInstancesBuffer);
    global::api::SafeRelease(internal::Pipeline.PrimitiveInstancesBuffer);

    // Release swap chain
    global::api::SafeRelease(internal::Pipeline.SwapChain);
    global::api::SafeRelease(internal::Pipeline.BackBuffer);

    // Release render window
    m_pRenderWindow.reset();
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
    hResult = InitBasicPipeline(_uX, _uY);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Setups precompiled shaders
    hResult = SetupPrecompiledShaders();
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating shaders!");
      return hResult;
    }

    // Setups constant buffers
    hResult = SetupConstantBuffers();
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating constant buffers!");
      return hResult;
    }

    // Setup render buffers
    hResult = SetupRenderBuffers();
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating render buffers!");
      return hResult;
    }

    hResult = SetupBlendState();
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating blend state!");
      return hResult;
    }

    // Setup rasterizers
    hResult = SetupRasterizers();
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating rasterizers!");
      return hResult;
    }

    // Setup samplers
    hResult = SetupSamplers();
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating samplers!");
      return hResult;
    }

    // Setup standard layouts
    hResult = SetupLayouts();
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating layouts!");
      return hResult;
    }

    // Set delegate
    utils::CDelegate<void(uint32_t, uint32_t)> rDelegate(&CRender::OnWindowResizeEvent, this);
    global::delegates::s_lstOnWindowResizeDelegates.emplace_back(rDelegate);

    // Get user def
    return global::api::DeviceContext->QueryInterface
    (
      __uuidof(ID3DUserDefinedAnnotation),
      reinterpret_cast<void**>(&internal::Pipeline.pUserMarker)
    );
  }
  // ------------------------------------
  HRESULT CRender::CreateDevice(uint32_t _uWidth, uint32_t _uHeight)
  {
    // Create descriptor
    DXGI_SWAP_CHAIN_DESC rSwapChainDescriptor = DXGI_SWAP_CHAIN_DESC();
    rSwapChainDescriptor.BufferCount = 1;
    rSwapChainDescriptor.BufferDesc.Width = _uWidth;
    rSwapChainDescriptor.BufferDesc.Height = _uHeight;
    rSwapChainDescriptor.OutputWindow = m_pRenderWindow->GetHandle();
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

    // Release device and context
    global::api::SafeRelease(global::api::Device);
    global::api::SafeRelease(global::api::DeviceContext);

    // Create device and swap chain
    return D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, uFlags, lstFeatureLevels,
      uNumFeatureLevels, D3D11_SDK_VERSION, &rSwapChainDescriptor, &internal::Pipeline.SwapChain,
      &global::api::Device, &oFeatureLevel, &global::api::DeviceContext);
  }
  // ------------------------------------
  HRESULT CRender::InitBasicPipeline(uint32_t _uWidth, uint32_t _uHeight)
  {
    // Configure viewport
    SetViewport(_uWidth, _uHeight);

    // Update scissor
    SetScissorRect(_uWidth, _uHeight);

    // Setup renderers
    HRESULT hResult = SetupRenderers(_uWidth, _uHeight);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Create back buffer
    return CreateBackBuffer();
  }
  // ------------------------------------
  void CRender::PrepareFrame()
  {
    // Clear resources
    BeginMarker(internal::s_sPrepareFrameMrk);
    {
      // Clear back buffer
      global::api::DeviceContext->ClearRenderTargetView(internal::Pipeline.BackBuffer, internal::s_v4ClearColor);

      // Prepare frame
      m_pDeferredRenderer->PrepareFrame();
      m_pForwardRenderer->PrepareFrame();
      m_pImGuiRenderer->PrepareFrame();
    }
    EndMarker();
  }
  // ------------------------------------
  void CRender::Draw(scene::CRenderScene& _rRenderScene)
  {
    // Deferred pass
    BeginMarker(internal::s_sDeferredPassMrk);
    {
      // Calculate projection and invert projection
#ifdef _DEBUG
      assert(m_pRenderCamera);
#endif
      math::CMatrix4x4 mViewProjection = m_pRenderCamera->GetViewProjection();
      TCameraTransform rTransforms = TCameraTransform();
      rTransforms.ViewProjection = mViewProjection;
      rTransforms.InvViewProjection = math::CMatrix4x4::Invert(mViewProjection);
      // Write
      bool bOk = internal::Pipeline.CameraTransformBuffer.WriteBuffer(rTransforms);
      UNUSED_VAR(bOk);
#ifdef _DEBUG
      assert(bOk);
#endif
      // Bind buffer
      internal::Pipeline.CameraTransformBuffer.Bind<render::EShader::E_VERTEX>(internal::Pipeline.CameraTransformSlot);

      // Detach simple pixel shader
      internal::Pipeline.ForwardPS.Detach();
      // Attach deferred vertex shader
      internal::Pipeline.DeferredVS.Attach();

      // Set standard layout
      global::api::DeviceContext->IASetInputLayout(internal::Pipeline.StandardLayout);
      //// Set depth stencil state
      //global::api::DeviceContext->OMSetDepthStencilState(internal::Pipeline.DepthStencilState, 1);

      // Set linear sampler(read textures)
      global::api::DeviceContext->PSSetSamplers(0, 1, &internal::Pipeline.LinearSampler);
      // Attach G-buffer(pixel shader)
      internal::Pipeline.DeferredGBufferPS.Attach();
      // Set constant buffer (texture info)
      internal::Pipeline.MaterialBuffer.Bind<render::EShader::E_PIXEL>(internal::Pipeline.MaterialSlot);

      // Deferred pass
      m_pDeferredRenderer->SetRenderCamera(m_pRenderCamera);
      m_pDeferredRenderer->Execute(_rRenderScene);

      // Lighting pass
      m_pLightingRenderer->SetRenderCamera(m_pShadowCamera);
      m_pLightingRenderer->SetShadowCamera(m_pShadowCamera);
      m_pLightingRenderer->Execute(_rRenderScene);

      // Set default rasterizer
      SetRasterizerState(internal::Pipeline.DefaultRasterizer);

      // Compute lighting pass
      ComputeLightingPass(_rRenderScene);
    }
    EndMarker();

    // Forward pass
    m_pForwardRenderer->SetRenderCamera(m_pRenderCamera);
    m_pForwardRenderer->Execute(_rRenderScene);

    // Update blend + rasterizer state
    SetBlendState(internal::Pipeline.BlendState, nullptr, 0xFFFFFFFF);
    SetRasterizerState(internal::Pipeline.DefaultRasterizer);

    // ImGui
    m_pImGuiRenderer->Execute(_rRenderScene);

    // Present
    const uint32_t uFlags = 0;
    internal::Pipeline.SwapChain->Present(m_bVerticalSync, uFlags);
  }
  // ------------------------------------
  void CRender::ShowRenderWindow(bool _bStatus)
  {
    if (m_pRenderWindow)
    {
      m_pRenderWindow->ShowWindow(_bStatus);
    }
  }
  // ------------------------------------
  void CRender::SetFillMode(D3D11_FILL_MODE _eFillMode)
  {
    // Update rasterizer
    internal::Pipeline.RasterizerCfg.FillMode = _eFillMode;

    global::api::SafeRelease(internal::Pipeline.DefaultRasterizer);
    CreateRasterizerState(internal::Pipeline.RasterizerCfg, &internal::Pipeline.DefaultRasterizer);
  }
  // ------------------------------------
  void CRender::PushMaterial(const render::mat::CMaterial* _pMaterial)
  {
    if (!_pMaterial)
    {
      return;
    }

    // Set material info
    TMaterialInfo rMaterialInfo = TMaterialInfo();
    rMaterialInfo.DiffuseColor = _pMaterial->GetDiffuseColor();
    rMaterialInfo.SpecularColor = _pMaterial->GetSpecularColor();

    texture::TSharedTexture pDiffuse = _pMaterial->GetTexture(render::ETexture::DIFFUSE);
    rMaterialInfo.HasDiffuseTexture = static_cast<bool>(pDiffuse);
    texture::TSharedTexture pNormal = _pMaterial->GetTexture(render::ETexture::NORMAL);
    rMaterialInfo.HasNormalTexture = static_cast<bool>(pNormal);
    texture::TSharedTexture pSpecular = _pMaterial->GetTexture(render::ETexture::SPECULAR);
    rMaterialInfo.HasSpecularTexture = static_cast<bool>(pSpecular);

    // Write buffer
    bool bOk = internal::Pipeline.MaterialBuffer.WriteBuffer(rMaterialInfo);
    UNUSED_VAR(bOk);
#ifdef _DEBUG
    assert(bOk);
#endif // DEBUG

    // Set textures 
    const uint32_t uTexturesSize(3);
    ID3D11ShaderResourceView* lstTextures[uTexturesSize] =
    {
      rMaterialInfo.HasDiffuseTexture ? pDiffuse->GetView() : nullptr,
      rMaterialInfo.HasNormalTexture ? pNormal->GetView() : nullptr,
      rMaterialInfo.HasSpecularTexture ? pSpecular->GetView() : nullptr
    };

    // Bind shaders
    global::api::DeviceContext->PSSetShaderResources(0, uTexturesSize, lstTextures);
  }
  // ------------------------------------
  void CRender::OnWindowResizeEvent(uint32_t _uWidth, uint32_t _uHeight)
  {
    // Remove current target view
    global::api::SafeRelease(internal::Pipeline.BackBuffer);

    // Resize buffers
    HRESULT hResult = internal::Pipeline.SwapChain->ResizeBuffers(0, _uWidth, _uHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG

    // Init pipeline
    hResult = InitBasicPipeline(_uWidth, _uHeight);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif // DEBUG
  }
  // ------------------------------------
  HRESULT CRender::SetupRenderers(uint32_t _uWidth, uint32_t _uHeight)
  {
    // Create deferred renderer
    if (!m_pDeferredRenderer)
    {
      m_pDeferredRenderer = std::make_unique<CDeferredRenderer>(this);
    }
    HRESULT hResult = m_pDeferredRenderer->Init(_uWidth, _uHeight);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Create forward renderer
    if (!m_pForwardRenderer)
    {
      m_pForwardRenderer = std::make_unique<CForwardRenderer>(this);
    }

    // Create lighting renderer
    if (!m_pLightingRenderer)
    {
      m_pLightingRenderer = std::make_unique<CLightingRenderer>(this);
      hResult = m_pLightingRenderer->Init(_uWidth, _uHeight);
      if (FAILED(hResult))
      {
        return hResult;
      }
    }

    // Create imgui renderer
    if (!m_pImGuiRenderer)
    {
      m_pImGuiRenderer = std::make_unique<CImGuiRenderer>(this);
      hResult = m_pImGuiRenderer->Init(m_pRenderWindow->GetHandle());
      if (FAILED(hResult))
      {
        return hResult;
      }
    }

    return S_OK;
  }
  // ------------------------------------
  HRESULT CRender::CreateBackBuffer()
  {
    ID3D11Texture2D* pTexture = nullptr;
    internal::Pipeline.SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture));
    if (!pTexture)
    {
      return E_FAIL;
    }

    HRESULT hResult = global::api::Device->CreateRenderTargetView(pTexture, nullptr, &internal::Pipeline.BackBuffer);
    if (FAILED(hResult))
    {
      return hResult;
    }
    pTexture->Release();
    return hResult;
  }
  // ------------------------------------
  HRESULT CRender::SetupPrecompiledShaders()
  {
    // Forward shaders
    internal::Pipeline.ForwardVS.Release();
    HRESULT hResult = internal::Pipeline.ForwardVS.Init(g_SimpleVS, ARRAYSIZE(g_SimpleVS));
    if (FAILED(hResult))
    {
      return hResult;
    }

    internal::Pipeline.ForwardPS.Release();
    hResult = internal::Pipeline.ForwardPS.Init(g_SimplePS, ARRAYSIZE(g_SimplePS));
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Deferred shaders
    internal::Pipeline.DeferredVS.Release();
    hResult = internal::Pipeline.DeferredVS.Init(g_StandardVS, ARRAYSIZE(g_StandardVS));
    if (FAILED(hResult))
    {
      return hResult;
    }

    internal::Pipeline.DeferredShadowsVS.Release();
    hResult = internal::Pipeline.DeferredShadowsVS.Init(g_ShadowsVS, ARRAYSIZE(g_ShadowsVS));
    if (FAILED(hResult))
    {
      return hResult;
    }

    internal::Pipeline.DrawQuadVS.Release();
    hResult = internal::Pipeline.DrawQuadVS.Init(g_DrawQuadVS, ARRAYSIZE(g_DrawQuadVS));
    if (FAILED(hResult))
    {
      return hResult;
    }

    internal::Pipeline.DeferredGBufferPS.Release();
    hResult = internal::Pipeline.DeferredGBufferPS.Init(g_GBufferPS, ARRAYSIZE(g_GBufferPS));
    if (FAILED(hResult))
    {
      return hResult;
    }

    internal::Pipeline.DeferredLightsPS.Release();
    return internal::Pipeline.DeferredLightsPS.Init(g_LightsPS, ARRAYSIZE(g_LightsPS));
  }
  // ------------------------------------
  HRESULT CRender::SetupConstantBuffers()
  {
    // Transforms buffer
    HRESULT hResult = internal::Pipeline.CameraTransformBuffer.Init();
    if (FAILED(hResult))
    {
      return hResult;
    }
    // Light view buffer
    hResult = internal::Pipeline.LightingViewBuffer.Init();
    if (FAILED(hResult))
    {
      return hResult;
    }
    // Material info buffer
    return internal::Pipeline.MaterialBuffer.Init();
  }
  // ------------------------------------
  HRESULT CRender::SetupRenderBuffers()
  {
    // Render instances buffer
    D3D11_BUFFER_DESC rVertexBufferDesc = D3D11_BUFFER_DESC();
    rVertexBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(render::gfx::TModelInstanceData) * render::gfx::s_uMaxInstances));
    rVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    rVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    rVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA rSubresourceData = D3D11_SUBRESOURCE_DATA();
    rSubresourceData.pSysMem = render::gfx::s_tModelInstanceData; // Global buffer
    global::api::SafeRelease(internal::Pipeline.RenderInstancesBuffer);
    HRESULT hResult = global::api::Device->CreateBuffer(&rVertexBufferDesc, &rSubresourceData, &internal::Pipeline.RenderInstancesBuffer);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Primitives buffer
    rVertexBufferDesc = D3D11_BUFFER_DESC();
    rVertexBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(render::gfx::TPrimitiveInstanceData) * render::gfx::s_uMaxInstances));
    rVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    rVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    rVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    rSubresourceData = D3D11_SUBRESOURCE_DATA();
    rSubresourceData.pSysMem = render::gfx::s_tPrimitiveInstanceData; // Global buffer
    global::api::SafeRelease(internal::Pipeline.PrimitiveInstancesBuffer);
    return global::api::Device->CreateBuffer(&rVertexBufferDesc, &rSubresourceData, &internal::Pipeline.PrimitiveInstancesBuffer);
  }
  // ------------------------------------
  HRESULT CRender::SetupBlendState()
  {
    internal::Pipeline.BlendStateCfg.BlendEnable = false;
    internal::Pipeline.BlendStateCfg.SrcBlend = D3D11_BLEND_ONE;
    internal::Pipeline.BlendStateCfg.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    internal::Pipeline.BlendStateCfg.BlendOp = D3D11_BLEND_OP_ADD;
    internal::Pipeline.BlendStateCfg.SrcBlendAlpha = D3D11_BLEND_ONE;
    internal::Pipeline.BlendStateCfg.DestBlendAlpha = D3D11_BLEND_ZERO;
    internal::Pipeline.BlendStateCfg.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    internal::Pipeline.BlendStateCfg.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    // Create blend desc
    D3D11_BLEND_DESC rBlendDesc = D3D11_BLEND_DESC();
    rBlendDesc.AlphaToCoverageEnable = false;
    rBlendDesc.RenderTarget[0] = internal::Pipeline.BlendStateCfg;

    // Create blend state
    global::api::SafeRelease(internal::Pipeline.BlendState);
    return CreateBlendState(rBlendDesc, &internal::Pipeline.BlendState);
  }
  // ------------------------------------
  HRESULT CRender::SetupRasterizers()
  {
    // Set standard rasterizer config
    internal::Pipeline.RasterizerCfg.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    internal::Pipeline.RasterizerCfg.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    internal::Pipeline.RasterizerCfg.FrontCounterClockwise = false;
    internal::Pipeline.RasterizerCfg.DepthBias = 0;
    internal::Pipeline.RasterizerCfg.DepthBiasClamp = 0.0f;
    internal::Pipeline.RasterizerCfg.SlopeScaledDepthBias = 0.0f;
    internal::Pipeline.RasterizerCfg.DepthClipEnable = true;
    internal::Pipeline.RasterizerCfg.ScissorEnable = true;
    internal::Pipeline.RasterizerCfg.MultisampleEnable = false;
    internal::Pipeline.RasterizerCfg.AntialiasedLineEnable = false;

    // Create default rasterizer
    return CreateRasterizerState(internal::Pipeline.RasterizerCfg, &internal::Pipeline.DefaultRasterizer);
  }
  // ------------------------------------
  HRESULT CRender::SetupSamplers()
  {
    // Linear sampler
    D3D11_SAMPLER_DESC rSamplerDesc = D3D11_SAMPLER_DESC();
    rSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    rSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    rSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    rSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    rSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    rSamplerDesc.BorderColor[0] = 0.0f;
    rSamplerDesc.BorderColor[1] = 0.0f;
    rSamplerDesc.BorderColor[2] = 0.0f;
    rSamplerDesc.BorderColor[3] = 0.0f;
    rSamplerDesc.MaxAnisotropy = 16u;
    rSamplerDesc.MipLODBias = 0.0f;
    rSamplerDesc.MinLOD = 0.0f;
    rSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create simple sampler
    global::api::SafeRelease(internal::Pipeline.LinearSampler);
    HRESULT hResult = global::api::Device->CreateSamplerState(&rSamplerDesc, &internal::Pipeline.LinearSampler);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Shadow sampler
    D3D11_SAMPLER_DESC rShadowSampler = D3D11_SAMPLER_DESC();
    rShadowSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    rShadowSampler.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    rShadowSampler.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    rShadowSampler.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    rShadowSampler.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    rShadowSampler.BorderColor[0] = 1.0f;
    rShadowSampler.BorderColor[1] = 1.0f;
    rShadowSampler.BorderColor[2] = 1.0f;
    rShadowSampler.BorderColor[3] = 1.0f;
    rShadowSampler.MaxAnisotropy = 1u;
    rShadowSampler.MinLOD = 0.0f;
    rShadowSampler.MaxLOD = 0.0f;

    // Create shadow sampler
    global::api::SafeRelease(internal::Pipeline.ShadowSampler);
    return global::api::Device->CreateSamplerState(&rShadowSampler, &internal::Pipeline.ShadowSampler);
  }
  // ------------------------------------
  HRESULT CRender::SetupLayouts()
  {
    // Create standard layout
    HRESULT hResult = global::api::Device->CreateInputLayout
    (
      internal::s_tStandardLayout,
      internal::s_iStandardLayoutSize,
      g_StandardVS,
      sizeof(g_StandardVS),
      &internal::Pipeline.StandardLayout
    );
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Create debug layout
    return global::api::Device->CreateInputLayout
    (
      internal::s_tPrimitivesLayout,
      internal::s_iPrimitiveLayoutSize,
      g_SimpleVS,
      sizeof(g_SimpleVS),
      &internal::Pipeline.DebugLayout
    );
  }
  // ------------------------------------
  D3D_PRIMITIVE_TOPOLOGY CRender::GetTopology(render::ERenderMode _eRenderMode)
  {
    switch (_eRenderMode)
    {
    case render::ERenderMode::SOLID: { return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; }
    case render::ERenderMode::WIREFRAME: { return D3D_PRIMITIVE_TOPOLOGY_LINELIST; }
    case render::ERenderMode::INVALID:
    default:
    {
      return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
    }
  }
  // ------------------------------------
  void CRender::SetRasterizerState(ID3D11RasterizerState* _pRasterizerState)
  {
    if (global::api::DeviceContext)
    {
      global::api::DeviceContext->RSSetState(_pRasterizerState);
    }
  }
  // ------------------------------------
  void CRender::SetInputLayout(ID3D11InputLayout* _pInputLayout)
  {
    if (global::api::DeviceContext)
    {
      global::api::DeviceContext->IASetInputLayout(_pInputLayout);
    }
  }
  // ------------------------------------
  void CRender::SetDepthStencilState(ID3D11DepthStencilState* _pDepthStencilState, uint32_t _uStencilRef)
  {
    if (global::api::DeviceContext)
    {
      global::api::DeviceContext->OMSetDepthStencilState(_pDepthStencilState, _uStencilRef);
    }
  }
  // ------------------------------------
  HRESULT CRender::CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC& _rDesc, ID3D11DepthStencilState** _ppDepthStencilState)
  {
    if (global::api::Device)
    {
      return global::api::Device->CreateDepthStencilState(&_rDesc, _ppDepthStencilState);
    }
    return E_FAIL;
  }
  // ------------------------------------
  void CRender::SetViewport(uint32_t _uWidth, uint32_t _uHeight)
  {
    if (!global::api::DeviceContext || !m_pRenderWindow)
    {
      ERROR_LOG("Rendering window or context is not valid!");
      return;
    }

    // Create viewport cfg
    D3D11_VIEWPORT rViewport = D3D11_VIEWPORT();
    {
      rViewport.Width = static_cast<float>(_uWidth);
      rViewport.Height = static_cast<float>(_uHeight);
      rViewport.MinDepth = internal::s_fMinDepth;
      rViewport.MaxDepth = internal::s_fMaxDepth;
    }

    // Apply viewport
    global::api::DeviceContext->RSSetViewports(1, &rViewport);
  }
  // ------------------------------------
  void CRender::SetScissorRect(uint32_t _uWidth, uint32_t _uHeight)
  {
    // Create scissor rect
    D3D11_RECT rRect = D3D11_RECT();
    {
      rRect.left = 0;
      rRect.top = 0;
      rRect.right = static_cast<LONG>(_uWidth);
      rRect.bottom = static_cast<LONG>(_uHeight);
    }

    // Set scissor rect
    global::api::DeviceContext->RSSetScissorRects(1, &rRect);
  }
  // ------------------------------------
  void CRender::BeginMarker(const wchar_t* _sMarker) const
  {
    if (internal::Pipeline.pUserMarker)
    {
      internal::Pipeline.pUserMarker->BeginEvent(_sMarker);
    }
  }
  // ------------------------------------
  void CRender::EndMarker() const
  {
    if (internal::Pipeline.pUserMarker)
    {
      internal::Pipeline.pUserMarker->EndEvent();
    }
  }
  // ------------------------------------
  void CRender::SetRenderTargets(uint32_t _uSize, ID3D11RenderTargetView** _pRenderTargets, ID3D11DepthStencilView* _pStencilView)
  {
    if (global::api::DeviceContext)
    {
      global::api::DeviceContext->OMSetRenderTargets(_uSize, _pRenderTargets, _pStencilView);
    }
  }
  // ------------------------------------
  void CRender::ClearRenderTargets(ID3D11RenderTargetView** _pRenderTargets, const float _v4ClearColor[4])
  {
    if (global::api::DeviceContext)
    {
      global::api::DeviceContext->ClearRenderTargetView(_pRenderTargets[0], _v4ClearColor);
    }
  }
  // ------------------------------------
  void CRender::ClearDepthStencil(ID3D11DepthStencilView* _pDepthStencilView, uint32_t uFlags, float _fDepth, uint8_t _uStencil)
  {
    if (global::api::DeviceContext && _pDepthStencilView)
    {
      global::api::DeviceContext->ClearDepthStencilView(_pDepthStencilView, uFlags, _fDepth, _uStencil);
    }
  }
  // ------------------------------------
  HRESULT CRender::CreateBlendState(D3D11_BLEND_DESC& _rDesc, ID3D11BlendState** _ppBlendState)
  {
    if (global::api::Device)
    {
      return global::api::Device->CreateBlendState(&_rDesc, _ppBlendState);
    }
    return E_FAIL;
  }
  // ------------------------------------
  void CRender::SetBlendState(ID3D11BlendState* _pBlendState, const float _v4BlendFactor[4], uint32_t _uSampleMask)
  {
    if (global::api::DeviceContext)
    {
      global::api::DeviceContext->OMSetBlendState(_pBlendState, _v4BlendFactor, _uSampleMask);
    }
  }
  // ------------------------------------
  HRESULT CRender::CreateRasterizerState(D3D11_RASTERIZER_DESC& _rDesc, ID3D11RasterizerState** _ppRasterizerState)
  {
    if(global::api::Device)
    {
      return global::api::Device->CreateRasterizerState(&_rDesc, _ppRasterizerState);
    }
    return E_FAIL;
  }
  // ------------------------------------
  void CRender::ComputeGBuffer(scene::CRenderScene& /*_rRenderScene*/)
  {
    //    // Calculate projection and invert projection
    //#ifdef _DEBUG
    //    assert(m_pRenderCamera);
    //#endif
    //    math::CMatrix4x4 mViewProjection = m_pRenderCamera->GetViewProjection();
    //    TCameraTransform rTransforms = TCameraTransform();
    //    rTransforms.ViewProjection = mViewProjection;
    //    rTransforms.InvViewProjection = math::CMatrix4x4::Invert(mViewProjection);
    //    // Write
    //    bool bOk = internal::Pipeline.CameraTransformBuffer.WriteBuffer(rTransforms);
    //    UNUSED_VAR(bOk);
    //#ifdef _DEBUG
    //    assert(bOk);
    //#endif
    //    // Bind buffer
    //    internal::Pipeline.CameraTransformBuffer.Bind<render::EShader::E_VERTEX>(internal::Pipeline.CameraTransformSlot);
    //
    //    // Detach simple pixel shader
    //    internal::Pipeline.ForwardPS.Detach();
    //    // Attach deferred vertex shader
    //    internal::Pipeline.DeferredVS.Attach();
    //
    //    // Set standard layout
    //    global::api::DeviceContext->IASetInputLayout(internal::Pipeline.StandardLayout);
    //    // Set depth stencil state
    //    global::api::DeviceContext->OMSetDepthStencilState(internal::Pipeline.DepthStencilState, 1);
    //
    //    // Set linear sampler(read textures)
    //    global::api::DeviceContext->PSSetSamplers(0, 1, &internal::Pipeline.LinearSampler);
    //    // Attach G-buffer(pixel shader)
    //    internal::Pipeline.DeferredGBufferPS.Attach();
    //    // Set constant buffer (texture info)
    //    internal::Pipeline.MaterialBuffer.Bind<render::EShader::E_PIXEL>(internal::Pipeline.MaterialSlot);
    //
    //    // Set render targets
    //    m_pDeferredRenderer->AttachRenderTargets(internal::Pipeline.DepthStencil);
    //
    //    // Cache models
    //    _rRenderScene.CacheModels(*m_pRenderCamera);
    //
    //    // Draw models
    //    DrawModels(_rRenderScene);
    //
    //    // Detach render targets
    //    m_pDeferredRenderer->DetachRenderTargets();
  }
  // ------------------------------------
  void CRender::ComputeShadowMapping(scene::CRenderScene& /*_rRenderScene*/)
  {
    //    BeginMarker(internal::s_sComputeShadowsMrk);
    //    {
    //      // Compute shadow map
    //      render::lights::CLightManager* pLightManager = _rRenderScene.GetLightManager();
    //      const lights::CLightManager::TShadowMaps& lstShadowMaps = pLightManager->GetShadowMaps();
    //
    //      utils::CWeakPtr<render::lights::CDirectionalLight> pDirLight = pLightManager->GetDirectionalLight();
    //      bool bCastShadows = pDirLight.IsValid() && pDirLight->CastShadows();
    //      if (bCastShadows && lstShadowMaps.GetSize() > 0)
    //      {
    //        // Set custom rasterizer for shadow mapping
    //        global::api::DeviceContext->RSSetState(internal::Pipeline.ShadowsRasterizer);
    //        {
    //          // Clear depth stencil view
    //          utils::CWeakPtr<render::gfx::CShadowMap> wpShadowMap = lstShadowMaps[0];
    //          const texture::TDepthStencil& rShadowStencil = wpShadowMap->GetStencil();
    //          global::api::DeviceContext->ClearDepthStencilView(rShadowStencil.GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    //
    //          // Configure viewport
    //          uint32_t uWidth = 0, uHeight = 0;
    //          rShadowStencil.GetTextureSize(uWidth, uHeight);
    //          SetViewport(uWidth, uHeight);
    //
    //          // Create view matrix from directional light
    //          const float fMaxDistance = 50.0f;
    //          math::CVector3 v3Dir = pLightManager->GetDirectionalLight()->GetDir();
    //          math::CVector3 v3SceneCenter = m_pRenderCamera->GetPos() + (m_pRenderCamera->GetDir() * fMaxDistance); // Max distance
    //          math::CVector3 v3ShadowPos = v3SceneCenter - (v3Dir * (fMaxDistance * 2.0f)); // Calculate shadow pos
    //
    //          // Orthographic values ( testing )
    //          float fAspectRatio = static_cast<float>(uWidth) / static_cast<float>(uHeight);
    //          float fHeight = 100.0f;
    //          float fWidth = fHeight * fAspectRatio;
    //
    //          const float fNear = m_pRenderCamera->GetNear();
    //          const float fFar = m_pRenderCamera->GetFar();
    //
    //          math::CMatrix4x4 mView = math::CMatrix4x4::LookAt(v3ShadowPos, v3SceneCenter, render::CRender::s_v3WorldUp);
    //          math::CMatrix4x4 mOrthographicProj = math::CMatrix4x4::CreateOrtographicMatrix(fWidth, fHeight, fNear, fFar);
    //
    //#ifdef _DEBUG
    //          assert(m_pShadowCamera);
    //#endif
    //          // Configure shadow camera
    //          m_pShadowCamera->SetProjectionMode(EProjectionMode::ORTOGRAPHIC);
    //          m_pShadowCamera->SetOrthographicSize(fHeight);
    //          m_pShadowCamera->SetProjectionMatrix(mOrthographicProj);
    //          m_pShadowCamera->SetViewMatrix(mView);
    //
    //          m_pShadowCamera->SetPos(v3ShadowPos);
    //          m_pShadowCamera->SetDir(v3Dir);
    //          m_pShadowCamera->SetAspectRatio(fAspectRatio);
    //
    //          m_pShadowCamera->SetNear(fNear);
    //          m_pShadowCamera->SetFar(fFar);
    //
    //          // Build frustum planes
    //          m_pShadowCamera->BuildFrustumPlanes();
    //
    //          // Calculate transforms for shadow mapping
    //          TCameraTransform rTransforms = TCameraTransform();
    //          {
    //            math::CMatrix4x4 mViewProjection = m_pShadowCamera->GetViewProjection();
    //            rTransforms.ViewProjection = mViewProjection;
    //            rTransforms.InvViewProjection = math::CMatrix4x4::Invert(mViewProjection);
    //          }
    //
    //          // Write buffer
    //          bool bOk = internal::Pipeline.LightingViewBuffer.WriteBuffer(rTransforms);
    //          UNUSED_VAR(bOk);
    //#ifdef _DEBUG
    //          assert(bOk);
    //#endif // DEBUG
    //          internal::Pipeline.LightingViewBuffer.Bind<render::EShader::E_VERTEX>(internal::Pipeline.CameraTransformSlot);
    //
    //          // Set render target
    //          SetRenderTargets(0u, nullptr, rShadowStencil.GetView());
    //
    //          // Set depth stencil state
    //          ID3D11DepthStencilState* pCurrentStencilState = nullptr;
    //          uint32_t uCurrentRef = 0;
    //          global::api::DeviceContext->OMGetDepthStencilState(&pCurrentStencilState, &uCurrentRef);
    //          if (pCurrentStencilState != internal::Pipeline.DepthStencilState)
    //          {
    //            global::api::DeviceContext->OMSetDepthStencilState(internal::Pipeline.DepthStencilState, 1);
    //          }
    //
    //          // Attach vertex shader for shadows (vertex shader)
    //          internal::Pipeline.DeferredShadowsVS.Attach();
    //          // Detach pixel shader for models
    //          internal::Pipeline.DeferredGBuffer.Detach();
    //
    //          // Cache models
    //          _rRenderScene.CacheModels(*m_pShadowCamera);
    //
    //          // Draw models only in z-prepass pass from the light view
    //          DrawModels(_rRenderScene);
    //
    //          uint32_t uRenderWidth = 0, uRenderHeight = 0;
    //          m_pRenderWindow->GetWindowSize(uRenderWidth, uRenderHeight);
    //          SetViewport(uRenderWidth, uRenderHeight);
    //        }
    //        // Restore rasterizer
    //        global::api::DeviceContext->RSSetState(internal::Pipeline.DefaultRasterizer);
    //      }
    //    }
    //    EndMarker();
  }
  // ------------------------------------
  void CRender::ComputeLightingPass(scene::CRenderScene& _rRenderScene)
  {
    // Set transform constant
    internal::Pipeline.CameraTransformBuffer.Bind<render::EShader::E_PIXEL>(internal::Pipeline.CameraTransformSlot);

    // Apply lighting
    render::lights::CLightManager* pLightManager = _rRenderScene.GetLightManager();
    pLightManager->ApplyLighting();

    utils::CWeakPtr<render::lights::CDirectionalLight> pDirLight = pLightManager->GetDirectionalLight();
    bool bCastShadows = pDirLight.IsValid() && pDirLight->CastShadows();
    const lights::CLightManager::TShadowMaps& lstShadowMaps = pLightManager->GetShadowMaps();

    // Shadow mapping texture
    ID3D11ShaderResourceView* pShadowTexture = nullptr;
    if (bCastShadows && lstShadowMaps.GetSize() > 0)
    {
      pShadowTexture = lstShadowMaps[0]->GetShadowTexture().GetView();
      global::api::DeviceContext->PSSetSamplers(1, 1, &internal::Pipeline.ShadowSampler);
      internal::Pipeline.LightingViewBuffer.Bind<render::EShader::E_PIXEL>(internal::Pipeline.LightingViewSlot);
    }

    static constexpr uint32_t uTexturesSize(5);
    ID3D11ShaderResourceView* lstGBufferSRV[uTexturesSize] =
    {
      internal::Pipeline.DepthTexture.GetView(),
      m_pDeferredRenderer->GetDiffuseRT()->GetRTView(),
      m_pDeferredRenderer->GetNormalRT()->GetRTView(),
      m_pDeferredRenderer->GetNormalRT()->GetRTView(),
      pShadowTexture
    };

    // Bind buffers
    global::api::DeviceContext->OMSetRenderTargets(1, &internal::Pipeline.BackBuffer, nullptr);
    global::api::DeviceContext->PSSetShaderResources(0, uTexturesSize, &lstGBufferSRV[0]);

    // Attach calculate lights shader (pixel shader)
    internal::Pipeline.DeferredLightsPS.Attach();

    // Draw quad to apply lighting
    DrawQuad();

    // Set invalid shaders
    ID3D11ShaderResourceView* lstEmptyTextures[uTexturesSize] = { nullptr, nullptr, nullptr, nullptr };
    global::api::DeviceContext->PSSetShaderResources(0, uTexturesSize, lstEmptyTextures);

    // Attach back buffer
    ID3D11DepthStencilView* pDepthStencilView = internal::Pipeline.DepthStencil.GetView();
    global::api::DeviceContext->OMSetRenderTargets(1, &internal::Pipeline.BackBuffer, pDepthStencilView);
  }
  // ------------------------------------
  void CRender::DrawQuad()
  {
    // Bind buffers
    SetRenderTargets(1u, &internal::Pipeline.BackBuffer);
    //global::api::DeviceContext->PSSetShaderResources(0, uTexturesSize, &lstGBufferSRV[0]);

    // Attach quad shader (vertex shader)
    internal::Pipeline.DrawQuadVS.Attach();

    // Setup quad vertex buffer
    global::api::DeviceContext->IASetVertexBuffers(0u, 0u, nullptr, nullptr, nullptr);
    global::api::DeviceContext->IASetInputLayout(nullptr);
    global::api::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Draw quad as fake triangle!
    const uint16_t uVertexCount = 3, uStartVertexLocation = 0;
    global::api::DeviceContext->Draw(uVertexCount, uStartVertexLocation);
  }
  // ------------------------------------
  void CRender::DrawModels(scene::CRenderScene& _rScene)
  {
    // Setup model vertex/index buffer
    uint16_t uDrawableCount = 0;
    const scene::TCachedModels& lstCacheModels = _rScene.GetCachedModels(uDrawableCount);
    if (uDrawableCount > 0)
    {
      const uint32_t uBuffersCount = 2, uIndexOffset = 0;

      ID3D11Buffer* pBuffers[uBuffersCount] = { _rScene.GetModelsVB(), internal::Pipeline.RenderInstancesBuffer };
      uint32_t lstStrides[uBuffersCount] = { sizeof(render::gfx::TVertexData), sizeof(render::gfx::TModelInstanceData) };
      uint32_t lstOffsets[uBuffersCount] = { 0, 0 };

      global::api::DeviceContext->IASetVertexBuffers(0, uBuffersCount, pBuffers, lstStrides, lstOffsets);
      global::api::DeviceContext->IASetIndexBuffer(_rScene.GetModelsIB(), DXGI_FORMAT_R32_UINT, uIndexOffset);
    }

    const scene::TModels& lstModels = _rScene.GetModels();
    for (uint16_t uI = 0; uI < uDrawableCount; uI++)
    {
      // Draw model
      const scene::TCachedModel& rCachedModel = lstCacheModels[uI];
      const render::gfx::CModel* pModel = lstModels[rCachedModel.Index].GetPtr();
      DrawModel(pModel, rCachedModel);
    }
  }
  // ------------------------------------
  void CRender::DrawModel(const render::gfx::CModel* _pModel, const scene::TCachedModel& _rCachedModel)
  {
    // Push buffers
    D3D11_MAPPED_SUBRESOURCE rMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
    HRESULT hResult = global::api::DeviceContext->Map(internal::Pipeline.RenderInstancesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &rMappedSubresource);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error mapping buffer!");
      return;
    }

    // Mapped instance data
    render::gfx::TModelInstanceData* pInstanceData = static_cast<render::gfx::TModelInstanceData*>(rMappedSubresource.pData);

    // Set model matrix
    uint16_t uInstance = 0;
    pInstanceData[uInstance++].Transform = _pModel->GetMatrix();

    // Set instances
    const render::gfx::TInstances& lstInstances = _pModel->GetInstances();
    for (uint16_t uJ = 0; uJ < _rCachedModel.InstanceCount; ++uJ)
    {
      uint16_t uID = _rCachedModel.DrawableInstances[uJ];
      pInstanceData[uInstance++].Transform = lstInstances[uID]->GetMatrix();
    }

    // Unmap
    global::api::DeviceContext->Unmap(internal::Pipeline.RenderInstancesBuffer, 0);

    // Set topology
    D3D_PRIMITIVE_TOPOLOGY eCurrentTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    global::api::DeviceContext->IAGetPrimitiveTopology(&eCurrentTopology);
    if (eCurrentTopology != D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
    {
      global::api::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    // Set values
    uint32_t uVtxOffset = _pModel->GetVtxBufferHandler().BeginOffset;
    uint16_t uInstances = _rCachedModel.Visible ? _rCachedModel.InstanceCount + 1 : _rCachedModel.InstanceCount;
    uint16_t uStartOffset = !_rCachedModel.Visible;

    // Draw meshes
    uint16_t uMeshCount = 0;
    const render::gfx::TMeshes& lstMeshes = _pModel->GetMeshes(uMeshCount);
    for (uint16_t uI = 0; uI < uMeshCount; uI++)
    {
      // Push material - constant buffer
      const render::gfx::CMesh& rMesh = lstMeshes[uI];
      PushMaterial(rMesh.GetMaterial());

      // Draw mesh
      uint32_t uIdxCount = rMesh.GetIndexCount();
      uint32_t uIdxOffset = rMesh.GetIdxBufferHandler().BeginOffset;
      global::api::DeviceContext->DrawIndexedInstanced(uIdxCount, uInstances, uIdxOffset, uVtxOffset, uStartOffset);
    }
  }
  // ------------------------------------
  void CRender::DrawPrimitives(scene::CRenderScene& _rRenderScene)
  {
    // Set input layout
    global::api::DeviceContext->IASetInputLayout(internal::Pipeline.DebugLayout);

    // Attach shaders
    internal::Pipeline.ForwardVS.Attach();
    internal::Pipeline.ForwardPS.Attach();

    // Bind buffer
    internal::Pipeline.CameraTransformBuffer.Bind<render::EShader::E_VERTEX>(internal::Pipeline.CameraTransformSlot);

    // Set layout
    const uint32_t uBuffersCount(2);
    uint32_t lstStrides[uBuffersCount] = { sizeof(math::CVector3), sizeof(render::gfx::TPrimitiveInstanceData) };
    uint32_t lstOffsets[uBuffersCount] = { 0, 0 };

    // Draw primitives
    uint16_t uDrawableCount = 0;
    const scene::TCachedPrimitives& lstCachedPrimitives = _rRenderScene.GetCachedPrimitives(uDrawableCount);
    if (uDrawableCount > 0)
    {
      // Set primitives global buffers
      ID3D11Buffer* pPrimitiveBuffers[uBuffersCount] = { _rRenderScene.GetPrimitivesVB(), internal::Pipeline.PrimitiveInstancesBuffer };
      global::api::DeviceContext->IASetVertexBuffers(0, uBuffersCount, pPrimitiveBuffers, lstStrides, lstOffsets);
      global::api::DeviceContext->IASetIndexBuffer(_rRenderScene.GetPrimitivesIB(), DXGI_FORMAT_R32_UINT, 0);

      const scene::TPrimitives& lstPrimitives = _rRenderScene.GetPrimitives();
      for (uint16_t uI = 0; uI < uDrawableCount; uI++)
      {
        render::gfx::CPrimitive* pPrimitive = lstPrimitives[lstCachedPrimitives[uI]].GetPtr();
        DrawPrimitive(pPrimitive);
      }
    }

#ifdef _DEBUG
    // Draw debug primitives
    const scene::TDebugPrimitives& lstDebugPrimitives = _rRenderScene.GetDebugPrimitives();
    const scene::TCachedDebugPrimitives& lstCachedDebugPrimitives = _rRenderScene.GetCachedDebugPrimitives(uDrawableCount);
    if (uDrawableCount > 0)
    {
      // Set debug global buffers
      ID3D11Buffer* pDebugPrimitiveBuffers[uBuffersCount] = { _rRenderScene.GetDebugPrimitivesVB(), internal::Pipeline.PrimitiveInstancesBuffer };
      global::api::DeviceContext->IASetVertexBuffers(0, uBuffersCount, pDebugPrimitiveBuffers, lstStrides, lstOffsets);
      global::api::DeviceContext->IASetIndexBuffer(_rRenderScene.GetDebugPrimitivesIB(), DXGI_FORMAT_R32_UINT, 0);

      for (uint16_t uI = 0; uI < uDrawableCount; uI++)
      {
        const render::gfx::CPrimitive* pDebugPrimitives = lstDebugPrimitives[(lstCachedDebugPrimitives[uI])];
        DrawPrimitive(pDebugPrimitives);
      }
    }

    // Clear debug items
    _rRenderScene.ClearDebugItems();
#endif
  }
  // ------------------------------------
  void CRender::DrawPrimitive(const render::gfx::CPrimitive* _pPrimitive)
  {
    // Apply Buffers
    D3D11_MAPPED_SUBRESOURCE rMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
    HRESULT hResult = global::api::DeviceContext->Map(internal::Pipeline.PrimitiveInstancesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &rMappedSubresource);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error mapping buffer!");
      return;
    }

    // Mapped instance data
    render::gfx::TPrimitiveInstanceData* pInstanceData = static_cast<render::gfx::TPrimitiveInstanceData*>(rMappedSubresource.pData);

    // Apply primitive data
    uint16_t uIndex = 0;
    pInstanceData[uIndex].Transform = _pPrimitive->GetMatrix();
    pInstanceData[uIndex].Color = _pPrimitive->GetColor();

    // Unmap
    global::api::DeviceContext->Unmap(internal::Pipeline.PrimitiveInstancesBuffer, 0);

    // Set topology
    D3D11_PRIMITIVE_TOPOLOGY eTargetTopology = GetTopology(_pPrimitive->GetRenderMode());
    D3D11_PRIMITIVE_TOPOLOGY eCurrentTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    global::api::DeviceContext->IAGetPrimitiveTopology(&eCurrentTopology);
    if (eCurrentTopology != eTargetTopology)
    {
      global::api::DeviceContext->IASetPrimitiveTopology(eTargetTopology);
    }

    // Set values - we don't currently support real primitive instances!
    uint32_t uIdxCount = _pPrimitive->GetIndexCount();
    uint32_t uIdxOffset = _pPrimitive->GetIdxBufferHandler().BeginOffset;
    uint32_t uVtxOffset = _pPrimitive->GetVtxBufferHandler().BeginOffset;

    // Draw primitive
    global::api::DeviceContext->DrawIndexedInstanced(uIdxCount, 1, uIdxOffset, uVtxOffset, 0);
  }
}
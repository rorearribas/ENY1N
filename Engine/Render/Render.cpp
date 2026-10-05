#include "Render.h"

#include "Engine/Global/GlobalResources.h"
#include "Engine/Render/Window/RenderWindow.h"
#include "Engine/Render/RenderContext/RenderContextDX11.h"
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
#include "Engine/Shaders/Deferred/LightingVS.h"
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

    // Standard layout - VTX(48) / INST(64)
    static constexpr int s_iStandardLayoutSize(8);
    static const D3D11_INPUT_ELEMENT_DESC s_tStandardLayout[s_iStandardLayoutSize] =
    {
      // Vertex layout
      { "VERTEXPOS",          0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 12
      { "NORMAL",             0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 24
      { "TANGENT",            0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 36
      { "UV",                 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 48
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
  }
  // ------------------------------------
  CRender::CRender(uint32_t _uWidth, uint32_t _uHeight)
  {
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
    m_oRenderPipeline.RenderCameraBuffer.Release();
    m_oRenderPipeline.LightingViewBuffer.Release();
    m_oRenderPipeline.MaterialBuffer.Release();

    // Layout + states
    global::api::SafeRelease(m_oRenderPipeline.StandardLayout);
    global::api::SafeRelease(m_oRenderPipeline.DebugLayout);

    // Release rasterizer, blending..
    global::api::SafeRelease(m_oRenderPipeline.LinearSampler);
    global::api::SafeRelease(m_oRenderPipeline.ShadowSampler);
    global::api::SafeRelease(m_oRenderPipeline.DefaultRasterizer);
    global::api::SafeRelease(m_oRenderPipeline.BlendState);
    global::api::SafeRelease(m_oRenderPipeline.pUserMarker);

    global::api::SafeRelease(m_oRenderPipeline.RenderInstancesBuffer);
    global::api::SafeRelease(m_oRenderPipeline.PrimitiveInstancesBuffer);

    // Release render target
    global::api::SafeRelease(m_oRenderPipeline.RenderTarget);

    // Release render window
    m_pRenderWindow.reset();
  }
  // ------------------------------------
  HRESULT CRender::Init(uint32_t _uWidth, uint32_t _uHeight)
  {
    // Create render window
    LOG("Creating render window...");
    m_pRenderWindow = std::make_unique<render::CRenderWindow>(_uWidth, _uHeight);
    SUCCESS_LOG("The window has been created successfully!");

    // Create device
    HRESULT hResult = CreateDevice(_uWidth, _uHeight);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating device!");
      return hResult;
    }

    // Setup basic pipeline
    hResult = InitBasicPipeline(_uWidth, _uHeight);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Setups precompiled shaders
    hResult = SetupPrecompiledPrograms();
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
    return m_pRenderContext->GetCommands()->QueryInterface
    (
      __uuidof(ID3DUserDefinedAnnotation),
      reinterpret_cast<void**>(&m_oRenderPipeline.pUserMarker)
    );
  }
  // ------------------------------------
  HRESULT CRender::CreateDevice(uint32_t _uWidth, uint32_t _uHeight)
  {
    if (!m_pRenderContext)
    {
      m_pRenderContext = std::make_unique<render::CRenderContextDX11>();
    }

    render::TRenderContextDesc rRenderContextDesc = render::TRenderContextDesc();
    {
      rRenderContextDesc.uWidth = _uWidth;
      rRenderContextDesc.uHeight = _uHeight;
      rRenderContextDesc.pOutputWindow = m_pRenderWindow->GetHandle();
      rRenderContextDesc.bFullscreen = false;
      rRenderContextDesc.bVSync = true;
    }
    HRESULT hResult = m_pRenderContext->Init(rRenderContextDesc);
    if (SUCCEEDED(hResult))
    {
      m_oTextureManager.Initialize(m_pRenderContext->GetDevice());
    }
    return hResult;
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
      ClearRenderTargets(&m_oRenderPipeline.RenderTarget, 1u, internal::s_v4ClearColor);

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
      // Push camera transform
      PushCameraTransform(*m_pRenderCamera);

      // Set default layout
      SetInputLayout(m_oRenderPipeline.StandardLayout);
      // Set default rasterizer
      SetRasterizerState(m_oRenderPipeline.DefaultRasterizer);

      // Attach deferred vertex shader
      m_oShaderManager.Bind(m_pRenderContext->GetCommands(), m_oRenderPipeline.GBuffer_ProgramID);
      // Attach G-buffer(pixel shader)
      // Set linear sampler(read textures)
      m_pRenderContext->GetCommands()->PSSetSamplers(0u, 1u, &m_oRenderPipeline.LinearSampler);

      // Bind camera transform buffer
      m_oRenderPipeline.RenderCameraBuffer.Bind<render::EShader::E_VERTEX>(m_pRenderContext->GetCommands(), m_oRenderPipeline.CameraTransformSlot);
      // Bind material buffer
      m_oRenderPipeline.MaterialBuffer.Bind<render::EShader::E_PIXEL>(m_pRenderContext->GetCommands(), m_oRenderPipeline.MaterialSlot);

      // Deferred pass
      m_pDeferredRenderer->SetRenderCamera(m_pRenderCamera);
      m_pDeferredRenderer->Draw(_rRenderScene);

      // Lighting pass
      m_pLightingRenderer->SetRenderCamera(m_pRenderCamera);
      m_pLightingRenderer->SetShadowCamera(m_pShadowCamera);
      m_pLightingRenderer->Draw(_rRenderScene);

      // Set default rasterizer
      SetRasterizerState(m_oRenderPipeline.DefaultRasterizer);

      // Compute lighting pass
      ComputeLightingPass(_rRenderScene);
    }
    EndMarker();

    // Forward pass
    {
      // Set render target
      SetRenderTargets(&m_oRenderPipeline.RenderTarget, 1u, m_pDeferredRenderer->GetDepthStencilView());
      SetRasterizerState(m_oRenderPipeline.DefaultRasterizer);
      SetDepthStencilState(m_pDeferredRenderer->GetDepthStencilState());

      // Set input layout
      m_pRenderContext->GetCommands()->IASetInputLayout(m_oRenderPipeline.DebugLayout);
      // Attach shaders
      m_oShaderManager.Bind(m_pRenderContext->GetCommands(), m_oRenderPipeline.Forward_ProgramID);
      // Bind buffer
      m_oRenderPipeline.RenderCameraBuffer.Bind<render::EShader::E_VERTEX>(m_pRenderContext->GetCommands(), m_oRenderPipeline.CameraTransformSlot);

      m_pForwardRenderer->SetRenderCamera(m_pRenderCamera);
      m_pForwardRenderer->Draw(_rRenderScene);
    }

    // Update blend + rasterizer state
    SetBlendState(m_oRenderPipeline.BlendState, nullptr, 0xFFFFFFFFu);
    SetRasterizerState(m_oRenderPipeline.DefaultRasterizer);

    // ImGui
    m_pImGuiRenderer->Draw(_rRenderScene);

    // Present
    const uint32_t uFlags = 0;
    m_pRenderContext->GetSwapChain()->Present(m_bVerticalSync, uFlags);
  }
  CRenderDeviceDX11& CRender::GetDevice()
  {
    return m_pRenderContext->GetDevice();
  }
  // ------------------------------------
  CRenderCommandsDX11& CRender::GetCommands()
  {
    return m_pRenderContext->GetCommands();
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
    m_oRenderPipeline.RasterizerCfg.FillMode = _eFillMode;

    global::api::SafeRelease(m_oRenderPipeline.DefaultRasterizer);
    CreateRasterizerState(m_oRenderPipeline.RasterizerCfg, &m_oRenderPipeline.DefaultRasterizer);
  }
  // ------------------------------------
  void CRender::PushMaterial(const render::mat::CMaterial* _pMaterial)
  {
    if (!_pMaterial)
    {
      return;
    }

    // Set material info
    buffertypes::TMaterialInfo rMaterialInfo = buffertypes::TMaterialInfo();
    rMaterialInfo.DiffuseColor = _pMaterial->GetDiffuseColor();
    rMaterialInfo.SpecularColor = _pMaterial->GetSpecularColor();

    // Diffuse
    texture::TSharedTexture pDiffuse = _pMaterial->GetTexture(render::ETexture::DIFFUSE);
    rMaterialInfo.HasDiffuseTexture = static_cast<bool>(pDiffuse);
    // Normal
    texture::TSharedTexture pNormal = _pMaterial->GetTexture(render::ETexture::NORMAL);
    rMaterialInfo.HasNormalTexture = static_cast<bool>(pNormal);
    // Specular
    texture::TSharedTexture pSpecular = _pMaterial->GetTexture(render::ETexture::SPECULAR);
    rMaterialInfo.HasSpecularTexture = static_cast<bool>(pSpecular);

    // Write buffer
    bool bOk = m_oRenderPipeline.MaterialBuffer.WriteBuffer(m_pRenderContext->GetCommands(), rMaterialInfo);
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
    m_pRenderContext->GetCommands()->PSSetShaderResources(0u, uTexturesSize, lstTextures);
  }
  // ------------------------------------
  void CRender::PushCameraTransform(const CCamera& _RenderCamera)
  {
    // Calculate projection and invert projection
    math::CMatrix4x4 mViewProjection = _RenderCamera.GetViewProjection();
    buffertypes::TCameraTransform rCameraTransform = buffertypes::TCameraTransform();
    {
      rCameraTransform.CameraPos = _RenderCamera.GetPos();
      rCameraTransform.ViewProjection = mViewProjection;
      rCameraTransform.InvViewProjection = math::CMatrix4x4::Invert(mViewProjection);
    }

    // Write
    bool bOk = m_oRenderPipeline.RenderCameraBuffer.WriteBuffer(m_pRenderContext->GetCommands(), rCameraTransform);
    UNUSED_VAR(bOk);
#ifdef _DEBUG
    assert(bOk);
#endif
  }
  // ------------------------------------
  void CRender::PushLightingTransform(const CCamera& _RenderCamera)
  {
    // Calculate transforms for shadow mapping
    math::CMatrix4x4 mViewProjection = _RenderCamera.GetViewProjection();
    buffertypes::TCameraTransform rCameraTransform = buffertypes::TCameraTransform();
    {
      rCameraTransform.CameraPos = _RenderCamera.GetPos();
      rCameraTransform.ViewProjection = mViewProjection;
      rCameraTransform.InvViewProjection = math::CMatrix4x4::Invert(mViewProjection);
    }

    bool bOk = m_oRenderPipeline.LightingViewBuffer.WriteBuffer(m_pRenderContext->GetCommands(), rCameraTransform);
    UNUSED_VAR(bOk);
#ifdef _DEBUG
    assert(bOk);
#endif // DEBUG
  }
  // ------------------------------------
  void CRender::PushShadowMappingPass()
  {
    // Detach all shaders in the current pipeline
    m_pRenderContext->GetCommands()->VSSetShader(nullptr, nullptr, 0u);
    m_pRenderContext->GetCommands()->PSSetShader(nullptr, nullptr, 0u);

    // Attach vertex shader for shadow mapping pass
    m_oShaderManager.Bind(m_pRenderContext->GetCommands(), m_oRenderPipeline.Shadow_ProgramID);
    // Attach lighting view buffer
    m_oRenderPipeline.LightingViewBuffer.Bind<render::EShader::E_VERTEX>(m_pRenderContext->GetCommands(), m_oRenderPipeline.CameraTransformSlot);
  }
  // ------------------------------------
  void CRender::OnWindowResizeEvent(uint32_t _uWidth, uint32_t _uHeight)
  {
    // Remove current target view
    global::api::SafeRelease(m_oRenderPipeline.RenderTarget);

    // Resize buffers
    HRESULT hResult = m_pRenderContext->GetSwapChain()->ResizeBuffers(0, _uWidth, _uHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
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
    HRESULT hResult = m_pDeferredRenderer->Init(m_pRenderContext->GetDevice(), _uWidth, _uHeight);
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
      hResult = m_pImGuiRenderer->Init(m_pRenderContext->GetDevice(), m_pRenderContext->GetCommands(), m_pRenderWindow->GetHandle());
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
    m_pRenderContext->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture));
    if (!pTexture)
    {
      return E_FAIL;
    }

    HRESULT hResult = m_pRenderContext->GetDevice()->CreateRenderTargetView(pTexture, nullptr, &m_oRenderPipeline.RenderTarget);
    if (FAILED(hResult))
    {
      return hResult;
    }
    pTexture->Release();
    return hResult;
  }
  // ------------------------------------
  HRESULT CRender::SetupPrecompiledPrograms()
  {
    // Forward program
    TShaderProgramData rShaderProgramData = TShaderProgramData();
    {
      rShaderProgramData.pVertexShader = g_SimpleVS;
      rShaderProgramData.tVertexShaderSize = ARRAYSIZE(g_SimpleVS);
      rShaderProgramData.pPixelShader = g_SimplePS;
      rShaderProgramData.tPixelShaderSize = ARRAYSIZE(g_SimplePS);
    }
    HRESULT hResult = m_oShaderManager.RegisterProgram
    (
      m_pRenderContext->GetDevice(), 
      rShaderProgramData,
      m_oRenderPipeline.Forward_ProgramID
    );
    if (FAILED(hResult))
    {
      return hResult;
    }

    // GBuffer program
    rShaderProgramData = TShaderProgramData();
    {
      rShaderProgramData.pVertexShader = g_StandardVS;
      rShaderProgramData.tVertexShaderSize = ARRAYSIZE(g_StandardVS);
      rShaderProgramData.pPixelShader = g_GBufferPS;
      rShaderProgramData.tPixelShaderSize = ARRAYSIZE(g_GBufferPS);
    }
    hResult = m_oShaderManager.RegisterProgram
    (
      m_pRenderContext->GetDevice(),
      rShaderProgramData, 
      m_oRenderPipeline.GBuffer_ProgramID
    );
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Shadow mapping program
    rShaderProgramData = TShaderProgramData();
    {
      rShaderProgramData.pVertexShader = g_LightingVS;
      rShaderProgramData.tVertexShaderSize = ARRAYSIZE(g_LightingVS);
    }
    hResult = m_oShaderManager.RegisterProgram
    (
      m_pRenderContext->GetDevice(), 
      rShaderProgramData, 
      m_oRenderPipeline.Shadow_ProgramID
    );

    // Deferred lighting program
    rShaderProgramData = TShaderProgramData();
    {
      rShaderProgramData.pVertexShader = g_DrawQuadVS;
      rShaderProgramData.tVertexShaderSize = ARRAYSIZE(g_DrawQuadVS);
      rShaderProgramData.pPixelShader = g_LightsPS;
      rShaderProgramData.tPixelShaderSize = ARRAYSIZE(g_LightsPS);
    }
    hResult = m_oShaderManager.RegisterProgram
    (
      m_pRenderContext->GetDevice(), 
      rShaderProgramData, 
      m_oRenderPipeline.DeferredLighting_ProgramID
    );
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Draw quad program
    rShaderProgramData = TShaderProgramData();
    {
      rShaderProgramData.pVertexShader = g_DrawQuadVS;
      rShaderProgramData.tVertexShaderSize = ARRAYSIZE(g_DrawQuadVS);
    }
    return m_oShaderManager.RegisterProgram
    (
      m_pRenderContext->GetDevice(), 
      rShaderProgramData, 
      m_oRenderPipeline.DrawQuad_ProgramID
    );
  }
  // ------------------------------------
  HRESULT CRender::SetupConstantBuffers()
  {
    // Render camera buffer
    HRESULT hResult = m_oRenderPipeline.RenderCameraBuffer.Init(m_pRenderContext->GetDevice());
    if (FAILED(hResult))
    {
      return hResult;
    }
    // Lighting view buffer
    hResult = m_oRenderPipeline.LightingViewBuffer.Init(m_pRenderContext->GetDevice());
    if (FAILED(hResult))
    {
      return hResult;
    }
    // Material info buffer
    return m_oRenderPipeline.MaterialBuffer.Init(m_pRenderContext->GetDevice());
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
    global::api::SafeRelease(m_oRenderPipeline.RenderInstancesBuffer);
    HRESULT hResult = m_pRenderContext->GetDevice()->CreateBuffer(&rVertexBufferDesc, &rSubresourceData, &m_oRenderPipeline.RenderInstancesBuffer);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Primitives instances buffer
    rVertexBufferDesc = D3D11_BUFFER_DESC();
    rVertexBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(render::gfx::TPrimitiveInstanceData) * render::gfx::s_uMaxInstances));
    rVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    rVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    rVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    rSubresourceData = D3D11_SUBRESOURCE_DATA();
    rSubresourceData.pSysMem = render::gfx::s_tPrimitiveInstanceData; // Global buffer
    global::api::SafeRelease(m_oRenderPipeline.PrimitiveInstancesBuffer);
    return m_pRenderContext->GetDevice()->CreateBuffer(&rVertexBufferDesc, &rSubresourceData, &m_oRenderPipeline.PrimitiveInstancesBuffer);
  }
  // ------------------------------------
  HRESULT CRender::SetupBlendState()
  {
    m_oRenderPipeline.BlendStateCfg.BlendEnable = false;
    m_oRenderPipeline.BlendStateCfg.SrcBlend = D3D11_BLEND_ONE;
    m_oRenderPipeline.BlendStateCfg.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    m_oRenderPipeline.BlendStateCfg.BlendOp = D3D11_BLEND_OP_ADD;
    m_oRenderPipeline.BlendStateCfg.SrcBlendAlpha = D3D11_BLEND_ONE;
    m_oRenderPipeline.BlendStateCfg.DestBlendAlpha = D3D11_BLEND_ZERO;
    m_oRenderPipeline.BlendStateCfg.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    m_oRenderPipeline.BlendStateCfg.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    // Create blend desc
    D3D11_BLEND_DESC rBlendDesc = D3D11_BLEND_DESC();
    rBlendDesc.AlphaToCoverageEnable = false;
    rBlendDesc.RenderTarget[0] = m_oRenderPipeline.BlendStateCfg;

    // Create blend state
    global::api::SafeRelease(m_oRenderPipeline.BlendState);
    return CreateBlendState(rBlendDesc, &m_oRenderPipeline.BlendState);
  }
  // ------------------------------------
  HRESULT CRender::SetupRasterizers()
  {
    // Set standard rasterizer config
    m_oRenderPipeline.RasterizerCfg.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    m_oRenderPipeline.RasterizerCfg.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    m_oRenderPipeline.RasterizerCfg.FrontCounterClockwise = false;
    m_oRenderPipeline.RasterizerCfg.DepthBias = 0;
    m_oRenderPipeline.RasterizerCfg.DepthBiasClamp = 0.0f;
    m_oRenderPipeline.RasterizerCfg.SlopeScaledDepthBias = 0.0f;
    m_oRenderPipeline.RasterizerCfg.DepthClipEnable = true;
    m_oRenderPipeline.RasterizerCfg.ScissorEnable = true;
    m_oRenderPipeline.RasterizerCfg.MultisampleEnable = false;
    m_oRenderPipeline.RasterizerCfg.AntialiasedLineEnable = false;

    // Create default rasterizer
    return CreateRasterizerState(m_oRenderPipeline.RasterizerCfg, &m_oRenderPipeline.DefaultRasterizer);
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
    global::api::SafeRelease(m_oRenderPipeline.LinearSampler);
    HRESULT hResult = m_pRenderContext->GetDevice()->CreateSamplerState(&rSamplerDesc, &m_oRenderPipeline.LinearSampler);
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
    global::api::SafeRelease(m_oRenderPipeline.ShadowSampler);
    return m_pRenderContext->GetDevice()->CreateSamplerState(&rShadowSampler, &m_oRenderPipeline.ShadowSampler);
  }
  // ------------------------------------
  HRESULT CRender::SetupLayouts()
  {
    // Create standard layout
    HRESULT hResult = m_pRenderContext->GetDevice()->CreateInputLayout
    (
      internal::s_tStandardLayout,
      internal::s_iStandardLayoutSize,
      g_StandardVS,
      sizeof(g_StandardVS),
      &m_oRenderPipeline.StandardLayout
    );
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Create debug layout
    return m_pRenderContext->GetDevice()->CreateInputLayout
    (
      internal::s_tPrimitivesLayout,
      internal::s_iPrimitiveLayoutSize,
      g_SimpleVS,
      sizeof(g_SimpleVS),
      &m_oRenderPipeline.DebugLayout
    );
  }
  // ------------------------------------
  D3D_PRIMITIVE_TOPOLOGY CRender::GetTopology(render::ERenderMode _eRenderMode)
  {
    switch (_eRenderMode)
    {
      case render::ERenderMode::SOLID:     { return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; }
      case render::ERenderMode::WIREFRAME: { return D3D_PRIMITIVE_TOPOLOGY_LINELIST; }
      case render::ERenderMode::INVALID:
      default: {return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED; }
    }
  }
  // ------------------------------------
  void CRender::SetRasterizerState(ID3D11RasterizerState* _pRasterizerState)
  {
    if (m_pRenderContext && m_pRenderContext->GetCommands())
    {
      m_pRenderContext->GetCommands()->RSSetState(_pRasterizerState);
    }
  }
  // ------------------------------------
  void CRender::SetInputLayout(ID3D11InputLayout* _pInputLayout)
  {
    if (m_pRenderContext && m_pRenderContext->GetCommands())
    {
      m_pRenderContext->GetCommands()->IASetInputLayout(_pInputLayout);
    }
  }
  // ------------------------------------
  void CRender::SetDepthStencilState(ID3D11DepthStencilState* _pDepthStencilState, uint32_t _uStencilRef)
  {
    if (m_pRenderContext && m_pRenderContext->GetCommands())
    {
      m_pRenderContext->GetCommands()->OMSetDepthStencilState(_pDepthStencilState, _uStencilRef);
    }
  }
  // ------------------------------------
  HRESULT CRender::CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC& _rDesc, ID3D11DepthStencilState** _ppDepthStencilState)
  {
    if (m_pRenderContext && m_pRenderContext->GetDevice())
    {
      return m_pRenderContext->GetDevice()->CreateDepthStencilState(&_rDesc, _ppDepthStencilState);
    }
    return E_FAIL;
  }
  // ------------------------------------
  void CRender::SetViewport(uint32_t _uWidth, uint32_t _uHeight)
  {
    if (!m_pRenderContext || !m_pRenderContext->GetCommands())
    {
      ERROR_LOG("Error: Rendering window or context is not valid!");
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
    m_pRenderContext->GetCommands()->RSSetViewports(1, &rViewport);
  }
  // ------------------------------------
  void CRender::SetScissorRect(uint32_t _uWidth, uint32_t _uHeight)
  {
    if (!m_pRenderContext || !m_pRenderContext->GetCommands())
    {
      ERROR_LOG("Error: Rendering window or context is not valid!");
      return;
    }

    // Create scissor rect
    D3D11_RECT rRect = D3D11_RECT();
    {
      rRect.left = 0;
      rRect.top = 0;
      rRect.right = static_cast<LONG>(_uWidth);
      rRect.bottom = static_cast<LONG>(_uHeight);
    }

    // Set scissor rect
    m_pRenderContext->GetCommands()->RSSetScissorRects(1, &rRect);
  }
  // ------------------------------------
  void CRender::BeginMarker(const wchar_t* _sMarker) const
  {
    if (m_oRenderPipeline.pUserMarker)
    {
      m_oRenderPipeline.pUserMarker->BeginEvent(_sMarker);
    }
  }
  // ------------------------------------
  void CRender::EndMarker() const
  {
    if (m_oRenderPipeline.pUserMarker)
    {
      m_oRenderPipeline.pUserMarker->EndEvent();
    }
  }
  // ------------------------------------
  void CRender::SetRenderTargets(ID3D11RenderTargetView** _pRenderTargets, uint32_t _uSize, ID3D11DepthStencilView* _pStencilView)
  {
    if (m_pRenderContext && m_pRenderContext->GetCommands())
    {
      m_pRenderContext->GetCommands()->OMSetRenderTargets(_uSize, _pRenderTargets, _pStencilView);
    }
  }
  // ------------------------------------
  void CRender::ClearRenderTargets(ID3D11RenderTargetView** _pRenderTargets, uint32_t _uSize, const float _v4ClearColor[4])
  {
    if (m_pRenderContext && m_pRenderContext->GetCommands())
    {
      for (uint32_t uIndex = 0; uIndex < _uSize; ++uIndex)
      {
        m_pRenderContext->GetCommands()->ClearRenderTargetView(_pRenderTargets[uIndex], _v4ClearColor);
      }
    }
  }
  // ------------------------------------
  void CRender::ClearDepthStencil(ID3D11DepthStencilView* _pDepthStencilView, uint32_t uFlags, float _fDepth, uint8_t _uStencil)
  {
    if (m_pRenderContext && m_pRenderContext->GetCommands() && _pDepthStencilView)
    {
      m_pRenderContext->GetCommands()->ClearDepthStencilView(_pDepthStencilView, uFlags, _fDepth, _uStencil);
    }
  }
  // ------------------------------------
  HRESULT CRender::CreateBlendState(D3D11_BLEND_DESC& _rDesc, ID3D11BlendState** _ppBlendState)
  {
    if (m_pRenderContext && m_pRenderContext->GetDevice())
    {
      return m_pRenderContext->GetDevice()->CreateBlendState(&_rDesc, _ppBlendState);
    }
    return E_FAIL;
  }
  // ------------------------------------
  void CRender::SetBlendState(ID3D11BlendState* _pBlendState, const float _v4BlendFactor[4], uint32_t _uSampleMask)
  {
    if (m_pRenderContext && m_pRenderContext->GetCommands())
    {
      m_pRenderContext->GetCommands()->OMSetBlendState(_pBlendState, _v4BlendFactor, _uSampleMask);
    }
  }
  // ------------------------------------
  HRESULT CRender::CreateRasterizerState(D3D11_RASTERIZER_DESC& _rDesc, ID3D11RasterizerState** _ppRasterizerState)
  {
    if (m_pRenderContext && m_pRenderContext->GetDevice())
    {
      return m_pRenderContext->GetDevice()->CreateRasterizerState(&_rDesc, _ppRasterizerState);
    }
    return E_FAIL;
  }
  // ------------------------------------
  void CRender::ComputeLightingPass(scene::CRenderScene& _rRenderScene)
  {
    // Apply lighting
    render::lights::CLightManager* pLightManager = _rRenderScene.GetLightManager();
    pLightManager->ApplyLighting();

    // Set transform constant
    m_oRenderPipeline.RenderCameraBuffer.Bind<render::EShader::E_PIXEL>(m_pRenderContext->GetCommands(), m_oRenderPipeline.CameraTransformSlot);

    utils::CWeakPtr<render::lights::CDirectionalLight> pDirLight = pLightManager->GetDirectionalLight();
    bool bCastShadows = pDirLight.IsValid() && pDirLight->CastShadows();
    const lights::CLightManager::TShadowMaps& lstShadowMaps = pLightManager->GetShadowMaps();

    // Shadow mapping texture
    ID3D11ShaderResourceView* pShadowTexture = nullptr;
    if (bCastShadows && lstShadowMaps.GetSize() > 0)
    {
      const utils::CWeakPtr<render::gfx::CShadowMap>& wpShadowMap = lstShadowMaps[0];
      pShadowTexture = wpShadowMap->GetShaderResource().GetView();
      m_pRenderContext->GetCommands()->PSSetSamplers(1, 1, &m_oRenderPipeline.ShadowSampler);
      m_oRenderPipeline.LightingViewBuffer.Bind<render::EShader::E_PIXEL>(m_pRenderContext->GetCommands(), m_oRenderPipeline.LightingViewSlot);
    }

    // Bind (Depth + GBuffer + Shadow) textures 
    static constexpr uint32_t uTexturesSize(5);
    ID3D11ShaderResourceView* lstTexturesSRV[uTexturesSize] =
    {
      m_pDeferredRenderer->GetShaderResourceView(), // Depth
      m_pDeferredRenderer->GetDiffuseRT().GetShaderResourceView(), // Render Target
      m_pDeferredRenderer->GetNormalRT().GetShaderResourceView(), // Render Target
      m_pDeferredRenderer->GetSpecularRT().GetShaderResourceView(), // Render Target
      pShadowTexture // Shadow texture
    };
    m_pRenderContext->GetCommands()->PSSetShaderResources(0u, uTexturesSize, &lstTexturesSRV[0]);

    // Bind buffers
    SetRenderTargets(&m_oRenderPipeline.RenderTarget, 1u, nullptr);

    // Set default rasterizer
    SetRasterizerState(m_oRenderPipeline.DefaultRasterizer);

    // Attach shader to calculate lights (pixel shader)
    m_oShaderManager.Bind(m_pRenderContext->GetCommands(), m_oRenderPipeline.DeferredLighting_ProgramID);

    // Draw quad to apply lighting
    DrawQuad();

    // Set invalid shaders
    ID3D11ShaderResourceView* lstEmptyTextures[uTexturesSize];
    memset(lstEmptyTextures, NULL, sizeof(lstEmptyTextures));
    m_pRenderContext->GetCommands()->PSSetShaderResources(0u, uTexturesSize, lstEmptyTextures);
  }
  // ------------------------------------
  void CRender::DrawQuad()
  {
    // Bind render target
    SetRenderTargets(&m_oRenderPipeline.RenderTarget, 1u);

    // Setup quad vertex buffer
    m_pRenderContext->GetCommands()->IASetVertexBuffers(0u, 0u, nullptr, nullptr, nullptr);
    m_pRenderContext->GetCommands()->IASetInputLayout(nullptr);
    m_pRenderContext->GetCommands()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Draw quad as fake triangle!
    const uint16_t uVertexCount = 3, uStartVertexLocation = 0;
    m_pRenderContext->GetCommands()->Draw(uVertexCount, uStartVertexLocation);

    // Remove back buffer
    SetRenderTargets(nullptr, 0u);
  }
  // ------------------------------------
  void CRender::DrawModels(scene::CRenderScene& _rScene)
  {
    // Setup
    uint16_t uDrawableCount = 0;
    const scene::TCachedModels& lstCacheModels = _rScene.GetCachedModels(uDrawableCount);
    if (uDrawableCount > 0)
    {
      // Setup vertex + index buffer
      const uint32_t uBuffersCount = 2, uIndexOffset = 0;
      ID3D11Buffer* pBuffers[uBuffersCount] = { _rScene.GetModelsVB(), m_oRenderPipeline.RenderInstancesBuffer };
      uint32_t lstStrides[uBuffersCount] = { sizeof(render::gfx::TVertexData), sizeof(render::gfx::TModelInstanceData) };
      uint32_t lstOffsets[uBuffersCount] = { 0, 0 };

      m_pRenderContext->GetCommands()->IASetVertexBuffers(0, uBuffersCount, pBuffers, lstStrides, lstOffsets);
      m_pRenderContext->GetCommands()->IASetIndexBuffer(_rScene.GetModelsIB(), DXGI_FORMAT_R32_UINT, uIndexOffset);
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
    HRESULT hResult = m_pRenderContext->GetCommands()->Map(m_oRenderPipeline.RenderInstancesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &rMappedSubresource);
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
    m_pRenderContext->GetCommands()->Unmap(m_oRenderPipeline.RenderInstancesBuffer, 0);

    // Set topology
    m_pRenderContext->GetCommands()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
      m_pRenderContext->GetCommands()->DrawIndexedInstanced(uIdxCount, uInstances, uIdxOffset, uVtxOffset, uStartOffset);
    }
  }
  // ------------------------------------
  void CRender::DrawPrimitives(scene::CRenderScene& _rRenderScene)
  {
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
      ID3D11Buffer* pPrimitiveBuffers[uBuffersCount] = { _rRenderScene.GetPrimitivesVB(), m_oRenderPipeline.PrimitiveInstancesBuffer };
      m_pRenderContext->GetCommands()->IASetVertexBuffers(0, uBuffersCount, pPrimitiveBuffers, lstStrides, lstOffsets);
      m_pRenderContext->GetCommands()->IASetIndexBuffer(_rRenderScene.GetPrimitivesIB(), DXGI_FORMAT_R32_UINT, 0);

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
      ID3D11Buffer* pDebugPrimitiveBuffers[uBuffersCount] = { _rRenderScene.GetDebugPrimitivesVB(), m_oRenderPipeline.PrimitiveInstancesBuffer };
      m_pRenderContext->GetCommands()->IASetVertexBuffers(0, uBuffersCount, pDebugPrimitiveBuffers, lstStrides, lstOffsets);
      m_pRenderContext->GetCommands()->IASetIndexBuffer(_rRenderScene.GetDebugPrimitivesIB(), DXGI_FORMAT_R32_UINT, 0);

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
    HRESULT hResult = m_pRenderContext->GetCommands()->Map(m_oRenderPipeline.PrimitiveInstancesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &rMappedSubresource);
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
    m_pRenderContext->GetCommands()->Unmap(m_oRenderPipeline.PrimitiveInstancesBuffer, 0);

    // Set topology
    D3D11_PRIMITIVE_TOPOLOGY eTargetTopology = GetTopology(_pPrimitive->GetRenderMode());
    m_pRenderContext->GetCommands()->IASetPrimitiveTopology(eTargetTopology);

    // Set values - we don't currently support real primitive instances!
    uint32_t uIdxCount = _pPrimitive->GetIndexCount();
    uint32_t uIdxOffset = _pPrimitive->GetIdxBufferHandler().BeginOffset;
    uint32_t uVtxOffset = _pPrimitive->GetVtxBufferHandler().BeginOffset;

    // Draw primitive
    m_pRenderContext->GetCommands()->DrawIndexedInstanced(uIdxCount, 1u, uIdxOffset, uVtxOffset, 0u);
  }
}






#pragma once
#include "Engine/Scenes/RenderScene.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Shaders/Shader.h"
#include "Engine/Render/Buffers/BufferTypes.h"
#include "Engine/Render/Buffers/ConstantBuffer.h"
#include "Engine/Render/Renderers/ShadowRenderer.h"
#include "Engine/Render/Managers/ShaderManager.h"
#include "Engine/Render/Managers/TextureManager.h"

namespace render { class CRenderWindow; }
namespace render { class CRenderContextDX11; }
namespace render { class CRenderDeviceDX11; }
namespace render { class CRenderCommandsDX11; }

namespace render { namespace gfx { class CModel; } }
namespace render { namespace gfx { class CPrimitive; } }
namespace render { namespace mat { class CMaterial; } }

namespace render { class CShadowRenderer; }
namespace render { class CDeferredRenderer; }
namespace render { class CForwardRenderer; }
namespace render { class CLightingRenderer; }
namespace render { class CImGuiRenderer; }

namespace render
{
  struct TRenderPipeline
  {
    // Global buffers for drawable things
    ID3D11Buffer* RenderInstancesBuffer = nullptr;
    ID3D11Buffer* PrimitiveInstancesBuffer = nullptr;

    // Global constant buffers
    CConstantBuffer<buffertypes::TCameraTransform> RenderCameraBuffer;
    static constexpr uint32_t CameraTransformSlot = 0;
    CConstantBuffer<buffertypes::TCameraTransform> LightingViewBuffer;
    static constexpr uint32_t LightingViewSlot = 2;
    CConstantBuffer<buffertypes::TMaterialInfo> MaterialBuffer;
    static constexpr uint32_t MaterialSlot = 0;

    // Default programs
    uintptr_t Forward_ProgramID;
    uintptr_t GBuffer_ProgramID;
    uintptr_t DrawQuad_ProgramID;

    uintptr_t Shadow_ProgramID;
    uintptr_t DeferredLighting_ProgramID;

    // Global states
    ID3D11RenderTargetView* RenderTarget = nullptr;
    ID3D11SamplerState* LinearSampler = nullptr;
    ID3D11SamplerState* ShadowSampler = nullptr;

    ID3D11RasterizerState* DefaultRasterizer = nullptr;
    D3D11_RASTERIZER_DESC RasterizerCfg = D3D11_RASTERIZER_DESC();

    ID3D11BlendState* BlendState = nullptr;
    D3D11_RENDER_TARGET_BLEND_DESC BlendStateCfg = D3D11_RENDER_TARGET_BLEND_DESC();

    ID3D11InputLayout* StandardLayout = nullptr;
    ID3D11InputLayout* DebugLayout = nullptr;
    ID3DUserDefinedAnnotation* pUserMarker = nullptr;
  };

  class CRender
  {
  public:
    static const math::CVector3 s_v3WorldUp;

  public:
    CRender(uint32_t _uWidth, uint32_t _uHeight);
    ~CRender();

    void PrepareFrame();
    void Draw(scene::CRenderScene& _rScene);

    inline const render::CRenderWindow* GetRenderWindow() const { return m_pRenderWindow.get(); }
    render::CRenderDeviceDX11& GetDevice();
    render::CRenderCommandsDX11& GetCommands();
    CTextureManager& GetTextureManager() { return m_oTextureManager; }

    inline void SetRenderCamera(render::CCamera* _pCamera) { m_pRenderCamera = _pCamera; }
    inline void SetShadowCamera(render::CCamera* _pCamera) { m_pShadowCamera = _pCamera; }

    void ShowRenderWindow(bool _bStatus);
    void SetFillMode(D3D11_FILL_MODE _eFillMode);

    void PushMaterial(const render::mat::CMaterial* _pMaterial);

    void PushCameraTransform(const CCamera& _RenderCamera);
    void PushLightingTransform(const CCamera& _RenderCamera);
    void PushShadowMappingPass();

    void BeginMarker(const wchar_t* _sMarker) const;
    void EndMarker() const;

    inline void SetVSync(bool _bEnabled) { m_bVerticalSync = _bEnabled; }
    inline bool IsVSyncEnabled() const { return m_bVerticalSync; }

    void SetRenderTargets(ID3D11RenderTargetView** _pRenderTargets, uint32_t _uSize, ID3D11DepthStencilView* _pStencilView = nullptr);
    void ClearRenderTargets(ID3D11RenderTargetView** _pRenderTargets, uint32_t _uSize, const float _v4ClearColor[4]);

    void SetClearColor(const float _v4ClearColor[4]);
    void ClearDepthStencil(ID3D11DepthStencilView* _pDepthStencilView, uint32_t uFlags, float _fDepth = 1.0f, uint8_t _uStencil = 0u);

    HRESULT CreateBlendState(D3D11_BLEND_DESC& _rDesc, ID3D11BlendState** _ppBlendState);
    void SetBlendState(ID3D11BlendState* _pBlendState, const float _v4BlendFactor[4] = nullptr, uint32_t _uSampleMask = 0xFFFFFFFF);

    HRESULT CreateRasterizerState(D3D11_RASTERIZER_DESC& _rDesc, ID3D11RasterizerState** _ppRasterizerState);
    void SetRasterizerState(ID3D11RasterizerState* _pRasterizerState);
    void SetInputLayout(ID3D11InputLayout* _pInputLayout);

    void SetDepthStencilState(ID3D11DepthStencilState* _pDepthStencilState, uint32_t _uStencilRef = 0);
    HRESULT CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC& _rDesc, ID3D11DepthStencilState** _ppDepthStencilState);

    void SetViewport(uint32_t _uWidth, uint32_t _uHeight);
    void SetScissorRect(uint32_t _uWidth, uint32_t _uHeight);

    void DrawPrimitives(scene::CRenderScene& _rRenderScene);
    void DrawModels(scene::CRenderScene& _rRenderScene);
    void DrawQuad();

  protected:
    void OnWindowResizeEvent(uint32_t _uWidth, uint32_t _uHeight);

  private:
    HRESULT Init(uint32_t _uWidth, uint32_t _uHeight);
    HRESULT CreateDevice(uint32_t _uWidth, uint32_t _uHeight);
    HRESULT InitBasicPipeline(uint32_t _uWidth, uint32_t _uHeight);

  private:
    HRESULT SetupRenderers(uint32_t _uWidth, uint32_t _uHeight);
    HRESULT CreateBackBuffer();

  private:
    HRESULT SetupPrecompiledPrograms();
    HRESULT SetupConstantBuffers();
    HRESULT SetupRenderBuffers();
    HRESULT SetupBlendState();
    HRESULT SetupRasterizers();
    HRESULT SetupSamplers();
    HRESULT SetupLayouts();

  private:
    D3D_PRIMITIVE_TOPOLOGY GetTopology(render::ERenderMode _eRenderMode);

  private:
    // Deferred
    void ComputeLightingPass(scene::CRenderScene& _rRenderScene);

  private:
    void DrawModel(const render::gfx::CModel* _pModel, const scene::TCachedModel& _rCachedModel);
    void DrawPrimitive(const render::gfx::CPrimitive* _pPrimitive);

  private:
    std::unique_ptr<render::CRenderWindow> m_pRenderWindow = nullptr;
    std::unique_ptr<render::CRenderContextDX11> m_pRenderContext = nullptr;

    TRenderPipeline m_oRenderPipeline;
    CShaderManager m_oShaderManager = CShaderManager();
    CTextureManager m_oTextureManager = CTextureManager();

    bool m_bVerticalSync = false;

    render::CCamera* m_pRenderCamera = nullptr;
    render::CCamera* m_pShadowCamera = nullptr;

    std::unique_ptr<render::CShadowRenderer> m_pShadowRenderer = nullptr;
    std::unique_ptr<render::CDeferredRenderer> m_pDeferredRenderer = nullptr;
    std::unique_ptr<render::CLightingRenderer> m_pLightingRenderer = nullptr;
    std::unique_ptr<render::CForwardRenderer> m_pForwardRenderer = nullptr;
    std::unique_ptr<render::CImGuiRenderer> m_pImGuiRenderer = nullptr;
  };
}



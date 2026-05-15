#pragma once
#include "Window/RenderWindow.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Shaders/Shader.h"

namespace render { namespace gfx { class CPrimitive; } }
namespace render { namespace mat { class CMaterial; } }
namespace scene { class CScene; }

namespace render { class CDeferredRenderer; }
namespace render { class CForwardRenderer; }
namespace render { class CLightingRenderer; }

namespace render
{
  class CRender
  {
  public:
    static const math::CVector3 s_v3WorldUp;

  public:
    CRender(uint32_t _uWidth, uint32_t _uHeight);
    ~CRender();

    void PrepareFrame();
    void Draw(scene::CScene* _pScene);

    inline render::CRenderWindow* GetRenderWindow() const { return m_pRenderWindow; }
    inline void SetRenderCamera(render::CCamera* _pCamera) { m_pRenderCamera = _pCamera; }
    inline void SetShadowCamera(render::CCamera* _pCamera) { m_pShadowCamera = _pCamera; }

    inline void ShowRenderWindow(bool _bStatus) { m_pRenderWindow->SetEnabled(_bStatus); }
    void SetFillMode(D3D11_FILL_MODE _eFillMode);
    void PushMaterial(const render::mat::CMaterial* _pMaterial);

    inline void SetVSync(bool _bEnabled) { m_bVerticalSync = _bEnabled; }
    inline bool IsVSyncEnabled() const { return m_bVerticalSync; }

  protected:
    void OnWindowResizeEvent(uint32_t _uWidth, uint32_t _uHeight);

  private:
    HRESULT Init(uint32_t _uWidth, uint32_t _uHeight);

    HRESULT SetupDeferredRendering(uint32_t _uWidth, uint32_t _uHeight);
    HRESULT InitConstantBuffers();
    HRESULT SetupLayouts();
    HRESULT InitShaders();

  private:
    HRESULT CreateDevice(uint32_t _uWidth, uint32_t _uHeight);
    HRESULT InitPipeline(uint32_t _uWidth, uint32_t _uHeight);

    HRESULT SetupDepthStencils(uint32_t _uWidth, uint32_t _uHeight);
    HRESULT CreateBackBuffer();

    HRESULT CreateRasterizerState(ID3D11RasterizerState*& _pRasterizer_, const D3D11_RASTERIZER_DESC& _rRasterizerCfg);
    HRESULT CreateBlendState(const D3D11_RENDER_TARGET_BLEND_DESC& _rBlendState);

    D3D_PRIMITIVE_TOPOLOGY GetTopology(render::ERenderMode _eRenderMode);

  private:
    void SetViewport(uint32_t _uWidth, uint32_t _uHeight);
    void SetScissorRect(uint32_t _uWidth, uint32_t _uHeight);
    bool InitImGui();

    void BeginMarker(const wchar_t* _sMarker) const;
    void EndMarker() const;

  private:
    // Deferred
    void DrawOpaqueModels(scene::CScene* _pScene);
    void ComputeGBuffer(scene::CScene* _pScene);

    void DrawModels(scene::CScene* _pScene); 
    void DrawPrimitives(render::CCamera* _pRenderCamera, scene::CScene* _pScene);
    void DrawPrimitive(const render::gfx::CPrimitive* _pPrimitive);

  private:
    render::CRenderWindow* m_pRenderWindow = nullptr;
    bool m_bVerticalSync = false;

    render::CCamera* m_pRenderCamera = nullptr;
    render::CCamera* m_pShadowCamera = nullptr;

    std::unique_ptr<CDeferredRenderer> m_pDeferredRenderer = nullptr;
    std::unique_ptr<CForwardRenderer> m_pForwardRenderer = nullptr;
    std::unique_ptr<CLightingRenderer> m_pLightingRenderer = nullptr;
  };
}



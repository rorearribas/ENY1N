#pragma once
#include "Window/RenderWindow.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Shaders/Shader.h"

namespace render { namespace gfx { class CModel; } }
namespace render { namespace gfx { class CPrimitive; } }
namespace render { namespace mat { class CMaterial; } }
namespace scene { class CRenderScene; }

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
    void Draw(scene::CRenderScene* _pScene);

    inline render::CRenderWindow* GetRenderWindow() const { return m_pRenderWindow.get(); }
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
    HRESULT CreateDevice(uint32_t _uWidth, uint32_t _uHeight);
    HRESULT InitBasicPipeline(uint32_t _uWidth, uint32_t _uHeight);

  private:
    HRESULT SetupDepthStencil(uint32_t _uWidth, uint32_t _uHeight);
    HRESULT SetupRenderers(uint32_t _uWidth, uint32_t _uHeight);
    HRESULT CreateBackBuffer();

  private:
    HRESULT SetupPrecompiledShaders();
    HRESULT SetupConstantBuffers();
    HRESULT SetupRenderBuffers();
    HRESULT SetupBlendState();
    HRESULT SetupRasterizers();
    HRESULT SetupSamplers();
    HRESULT SetupLayouts();
    HRESULT SetupImGui();

  private:
    D3D_PRIMITIVE_TOPOLOGY GetTopology(render::ERenderMode _eRenderMode);
    void SetViewport(uint32_t _uWidth, uint32_t _uHeight);
    void SetScissorRect(uint32_t _uWidth, uint32_t _uHeight);

    void BeginMarker(const wchar_t* _sMarker) const;
    void EndMarker() const;

  private:
    // Deferred
    void DrawOpaqueModels(scene::CRenderScene* _pScene);
    void ComputeGBuffer(scene::CRenderScene* _pScene);

    void DrawModels(scene::CRenderScene* _pScene);
    void DrawModel(const render::gfx::CModel* _pModel, bool _bVisible, render::gfx::TDrawableInstances _lstDrawableInstances, uint16_t _uInstanceCount);

    void DrawPrimitives(scene::CRenderScene* _pScene);
    void DrawPrimitive(const render::gfx::CPrimitive* _pPrimitive);

  private:
    std::unique_ptr<render::CRenderWindow> m_pRenderWindow = nullptr;
    bool m_bVerticalSync = false;

    render::CCamera* m_pRenderCamera = nullptr;
    render::CCamera* m_pShadowCamera = nullptr;

    std::unique_ptr<CDeferredRenderer> m_pDeferredRenderer = nullptr;
    std::unique_ptr<CForwardRenderer> m_pForwardRenderer = nullptr;
    std::unique_ptr<CLightingRenderer> m_pLightingRenderer = nullptr;
  };
}



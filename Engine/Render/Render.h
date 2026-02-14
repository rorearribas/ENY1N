#pragma once
#include "Window/RenderWindow.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Shaders/Shader.h"

namespace render { namespace mat { class CMaterial; } }
namespace scene { class CScene; }

namespace render
{
  class CRender
  {
  public:
    static const math::CVector3 s_v3WorldUp;

  public:
    CRender(uint32_t _uX, uint32_t _uY);
    ~CRender();

    void PrepareFrame();
    void Draw(scene::CScene* _pScene);

    inline render::CRenderWindow* GetRenderWindow() const { return m_pRenderWindow; }
    inline void SetRenderCamera(render::CCamera* _pCamera) { m_pCamera = _pCamera; }

    inline void ShowRenderWindow(bool _bStatus) { m_pRenderWindow->SetEnabled(_bStatus); }
    void SetFillMode(D3D11_FILL_MODE _eFillMode);
    void PushMaterialInfo(const render::mat::CMaterial* _pMaterial);

    inline void SetVSync(bool _bEnabled) { m_bVerticalSync = _bEnabled; }
    inline bool IsVSyncEnabled() const { return m_bVerticalSync; }

  protected:
    void OnWindowResizeEvent(uint32_t _uX, uint32_t _uY);

  private:
    HRESULT Init(uint32_t _uX, uint32_t _uY);

    HRESULT SetupDeferredShading(uint32_t _uX, uint32_t _uY);
    HRESULT InitConstantBuffers();
    HRESULT SetupLayouts();
    HRESULT InitShaders();

  private:
    HRESULT CreateDevice(uint32_t _uX, uint32_t _uY);
    HRESULT InitPipeline(uint32_t _uX, uint32_t _uY);

    HRESULT SetupDepthStencils(uint32_t _uX, uint32_t _uY);
    HRESULT CreateBackBuffer();

    HRESULT CreateRasterizerState(const D3D11_RASTERIZER_DESC& _rRasterizerState);
    HRESULT CreateBlendState(const D3D11_RENDER_TARGET_BLEND_DESC& _rBlendState);

  private:
    void ConfigureViewport(uint32_t _uX, uint32_t _uY);
    void SetScissorRect(uint32_t _uX, uint32_t _uY);
    bool InitImGui();

    void BeginMarker(const wchar_t* _sMarker) const;
    void EndMarker() const;

  private:
    // Deferred
    void DrawModels(scene::CScene* _pScene);
    void DrawPrimitives(scene::CScene* _pScene);
    void DrawGBuffer();

  private:
    render::CRenderWindow* m_pRenderWindow = nullptr;
    render::CCamera* m_pCamera = nullptr;
    bool m_bVerticalSync = false;
  };
}



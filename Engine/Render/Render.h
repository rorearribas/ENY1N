#pragma once
#include "Window/RenderWindow.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Shaders/Shader.h"

namespace scene { class CScene; }

namespace render
{
  enum class ERenderMode { SOLID, WIREFRAME };

  class CRender
  {
  public:
    static const math::CVector3 s_v3WorldUp;

  public:
    CRender(uint32_t _uX, uint32_t _uY);
    ~CRender();

    void BeginDraw();
    void Draw(scene::CScene* _pScene);
    void EndDraw();

    inline render::CRenderWindow* GetRenderWindow() const { return m_pRenderWindow; }
    inline void SetCurrentCamera(const render::CCamera* _pCamera) { m_pCamera = _pCamera; }
    void SetModelMatrix(const math::CMatrix4x4& _mModel);

    inline void ShowRenderWindow(bool _bStatus) { m_pRenderWindow->SetEnabled(_bStatus); }
    void SetFillMode(D3D11_FILL_MODE _eFillMode);

    inline void SetVSync(bool _bEnabled) { m_bVerticalSync = _bEnabled; }
    inline bool IsVSyncEnabled() const { return m_bVerticalSync; }

  private:
    void OnWindowResizeEvent(uint32_t _uX, uint32_t _uY);

    HRESULT Init(uint32_t _uX, uint32_t _uY);
    HRESULT CreateDevice(uint32_t _uX, uint32_t _uY);
    HRESULT InitBasicPipeline(uint32_t _uX, uint32_t _uY);

    HRESULT CreateDepthStencilView(uint32_t _uX, uint32_t _uY);
    HRESULT SetupDeferredShading(uint32_t _uX, uint32_t _uY);
    HRESULT CreateBackBuffer();

  private:
    HRESULT SetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& _oDepthStencilState);
    HRESULT CreateRasterizerState(const D3D11_RASTERIZER_DESC& _oRasterizerState);
    HRESULT CreateBlendState(const D3D11_RENDER_TARGET_BLEND_DESC& _oBlendState);

    void ConfigureViewport(uint32_t _uX, uint32_t _uY);
    void SetScissorRect(uint32_t _uX, uint32_t _uY);
    bool InitImGui();

    void BeginMarker(const wchar_t* _sMarker) const;
    void EndMarker() const;

  private:
    render::CRenderWindow* m_pRenderWindow = nullptr;
    const render::CCamera* m_pCamera = nullptr;
    bool m_bVerticalSync = false;
  };
}



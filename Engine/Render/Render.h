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

    void BeginDraw() const;
    void Draw(scene::CScene* _pScene) const;
    void EndDraw() const;

    void SetFillMode(D3D11_FILL_MODE _eFillMode) { CreateRasterizerState(_eFillMode); }
    void SetVSync(bool _bEnabled) { m_bVerticalSync = _bEnabled; }

    render::CRenderWindow* GetRenderWindow() const { return m_pRenderWindow; }
    bool IsVSyncEnabled() const { return m_bVerticalSync; }

  private:
    void OnWindowResizeEvent(uint32_t _uX, uint32_t _uY);

    HRESULT Init(uint32_t _uX, uint32_t _uY);
    HRESULT CreateDevice(uint32_t _uX, uint32_t _uY);
    HRESULT InitBasicPipeline(uint32_t _uX, uint32_t _uY);

    HRESULT CreateRenderTargetView();
    HRESULT CreateDepthStencilView(uint32_t _uX, uint32_t _uY);
    HRESULT CreateRasterizerState(D3D11_FILL_MODE _eFillMode = D3D11_FILL_SOLID);
    HRESULT CreateBlendState();

    void ConfigureViewport(uint32_t _uX, uint32_t _uY);
    void SetScissorRect(uint32_t _uX, uint32_t _uY);
    bool InitImGui();

  private:
    void BeginMarker(const wchar_t* _sMarker) const;
    void EndMarker() const;

    // Data
    render::CRenderWindow* m_pRenderWindow = nullptr;
    bool m_bVerticalSync = false;
  };
}



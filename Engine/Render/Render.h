#pragma once
#include "Window/RenderWindow.h"
#include "Engine/Camera/Camera.h"
#include "Shader.h"

namespace scene { class CScene; }

namespace render
{
  class CRender
  {
  public:
    CRender(uint32_t _uX, uint32_t _uY);
    ~CRender();

    void BeginDraw();
    void Draw(scene::CScene* _pScene);
    void EndDraw();

    render::CRenderWindow* GetRenderWindow() const { return m_pRenderWindow; }
    void SetVSync(bool _bEnabled) { m_bVerticalSync = _bEnabled; }
    bool IsVSyncEnabled() { return m_bVerticalSync; }

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

    // Data
    render::CRenderWindow* m_pRenderWindow = nullptr;
    bool m_bVerticalSync = false;
  };
}



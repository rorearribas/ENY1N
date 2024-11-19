#pragma once
#include "Engine/Render/Primitives/Primitive.h"
#include "Engine/Scenes/Scene.h"
#include "RenderWindow.h"
#include "Engine/Camera/Camera.h"

namespace render
{
  class CRender
  {
  private:
    struct SRenderingResources
    {
      // Swap and render target
      IDXGISwapChain* m_pSwapChain = nullptr;
      ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

      // Depth stencil
      ID3D11Texture2D* m_pDepthStencilTexture = nullptr;
      ID3D11DepthStencilState* m_pDepthStencilState = nullptr;
      ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

      // Rasterizer
      ID3D11RasterizerState* m_pRasterizerState = nullptr;
    };

  public:
    CRender(const UINT32& _uWidth, const UINT32& _uHeight);
    ~CRender();

    HRESULT Init();
    void DrawScene(scene::CScene* _pScene, float _fDeltaTime);

    const render::CRenderWindow* GetRenderWindow() const { return m_pRenderWindow; }
    const render::CCamera* GetCamera() const { return m_pCamera; }

    void SetVSync(bool _bEnabled) { m_bVerticalSync = _bEnabled; }
    bool IsVSyncEnabled() { return m_bVerticalSync; }

  private:
    void OnWindowResizeEvent(UINT32 _uX, UINT32 _uY);
    void ConfigureViewport(UINT32 _uX, UINT32 _uY);
    void DrawImGui();

    HRESULT CreateDevice();
    HRESULT CreateRenderTargetView();
    HRESULT CreateDepthStencilView(UINT32 _uX, UINT32 _uY);
    HRESULT CreateRasterizerState(D3D11_FILL_MODE _eFillMode);
    void UpdateScissor(UINT32 _uX, UINT32 _uY);

    void SetupCamera();
    bool InitImGui();

    // Render camera
    render::CCamera* m_pCamera = nullptr;

    // Render window
    render::CRenderWindow* m_pRenderWindow = nullptr;

    // Info
    SRenderingResources m_oRenderingResources = {};
    bool m_bVerticalSync = false;
  };
}



#pragma once
#include "Engine/Render/Primitives/Primitive.h"
#include "Engine/Scenes/Scene.h"
#include "RenderWindow.h"
#include "ConstantBuffer.h"
#include "Engine/Camera/Camera.h"

namespace render
{
  class CRender
  {
  public:
    CRender(const UINT32& _uWidth, const UINT32& _uHeight);
    ~CRender();

    HRESULT Init();

    const CRenderWindow* GetRenderWindow() { return m_pRenderWindow; }
    void DrawScene(scene::CScene* _pScene);
    void SetVSync(bool _bEnabled) { m_bVerticalSync = _bEnabled; }

  private:
    void OnWindowResizeEvent(UINT32 _uX, UINT32 _uY);
    void ConfigureViewport(UINT32 _uX, UINT32 _uY);
    void DrawImGui();

    HRESULT CreateDevice();
    HRESULT CreateRenderTargetView();
    void SetupCamera();
    bool InitImGui();

    // Render camera
    render::CCamera* m_pCamera = nullptr;

    // Render window
    render::CRenderWindow* m_pRenderWindow = nullptr;
    bool m_bVerticalSync = true;
  };
}



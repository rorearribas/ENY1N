#pragma once
#include "Engine/Render/Primitives/Primitive.h"
#include "Engine/Scenes/Scene.h"
#include "RenderWindow.h"

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

  private:
    void OnWindowResizeEvent(UINT32 _uX, UINT32 _uY);
    void ConfigureViewport(UINT32 _uX, UINT32 _uY);
    void DrawImGui();

    HRESULT CreateDevice();
    HRESULT CreateRenderTargetView();
    bool InitImGui();

    // Render window
    CRenderWindow* m_pRenderWindow = nullptr;
  };
}



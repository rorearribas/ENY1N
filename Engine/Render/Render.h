#pragma once
#include <vector>
#include "RenderWindow.h"
#include "Engine/Render/Primitives/Primitive.h"
#include "Engine/Scenes/Scene.h"

namespace render
{
  class CRender
  {
  public:
    CRender(const UINT32& _uWidth, const UINT32& _uHeight);
    ~CRender();

    HRESULT InitDevice();
    void DrawScene(scene::CScene* _pScene);
    const CRenderWindow* GetRenderWindow() { return m_pRenderWindow; }

  private:
    HRESULT CreateRenderTargetView();
    void ConfigureViewport();

    // Render window
    CRenderWindow* m_pRenderWindow = nullptr;
  };
}



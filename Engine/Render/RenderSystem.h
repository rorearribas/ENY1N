#pragma once
#include <vector>
#include "RenderWindow.h"
#include "Engine/Render/RenderItem/PrimitiveItem.h"
#include "Engine/Scenes/Scene.h"

namespace render
{
  class CRenderSystem
  {
  public:
    CRenderSystem(const UINT32& _uWidth, const UINT32& _uHeight);
    ~CRenderSystem();

    HRESULT InitDevice();
    void Update(scene::CScene* _pScene);
    const CRenderWindow* GetRenderWindow() { return m_pRenderWindow; }

  private:
    HRESULT CreateRenderTargetView();
    void ConfigureViewport();

    // Render window
    CRenderWindow* m_pRenderWindow = nullptr;
  };
}



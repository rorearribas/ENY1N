#pragma once
#include "..\Render\RenderSystem.h"

namespace engine {
  class CEngine
  {
  public:
    CEngine();
    ~CEngine() {}

  private:
    render::CRenderSystem* m_pRenderSystem = nullptr;
  };
}




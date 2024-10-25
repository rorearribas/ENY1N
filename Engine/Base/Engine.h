#pragma once
#include "..\Render\RenderSystem.h"
#include "..\Utils\Utils.h"

namespace engine 
{
  class CEngine : public CSingleton<CEngine>
  {
  public:
    CEngine() {}
    ~CEngine() {}

    void InitEngine(const UINT32& _uWidth, const UINT32& _uHeight);

  private:
    render::CRenderSystem* m_pRenderSystem = nullptr;
  };
}




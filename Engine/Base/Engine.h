#pragma once
#include "..\Render\RenderSystem.h"
#include "..\Utils\Utils.h"
#include "..\Utils\Tick.h"

namespace engine 
{
  class CEngine : public utils::CSingleton<CEngine>
  {
  public:
    CEngine() {}
    ~CEngine() {}

    void InitEngine(const UINT32& _uWidth, const UINT32& _uHeight);
    void Update();

    const UINT32& GetRenderWidth() { return m_pRenderSystem->GetRenderWindow()->GetWidth(); }
    const UINT32& GetRenderHeight() { return m_pRenderSystem->GetRenderWindow()->GetHeight(); }

  private:
    render::CRenderSystem* m_pRenderSystem = nullptr;
    utils::CTick* m_pTick = nullptr;
  };
}




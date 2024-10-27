#pragma once
#include "Engine/Render/RenderSystem.h"
#include "Libs/Utils/Singleton.h"
#include "Libs/Time/Fixedtick.h"

namespace render { namespace items { class CPrimitiveItem; } }

namespace engine 
{
  class CEngine : public utils::CSingleton<CEngine> {
  public:
    CEngine() {}
    ~CEngine() {}

    void InitEngine(const UINT32& _uWidth, const UINT32& _uHeight);
    void Update( );

    const UINT32& GetRenderWidth() { return m_pRenderSystem->GetRenderWindow()->GetWidth(); }
    const UINT32& GetRenderHeight() { return m_pRenderSystem->GetRenderWindow()->GetHeight(); }

    render::items::CPrimitiveItem* CreatePrimitiveItem(std::vector<float>& _vctVertexData);

  private:
    render::CRenderSystem* m_pRenderSystem = nullptr;
    global::CFixedTick* m_pFixedTick = nullptr;
  };
}




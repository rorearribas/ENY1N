#pragma once
#include "Engine/Render/RenderSystem.h"
#include "Engine/Scenes/SceneManager.h"
#include "Libs/Utils/Singleton.h"
#include "Libs/Time/Fixedtick.h"

namespace render { namespace items { class CPrimitiveItem; } }

namespace engine 
{
  class CEngine : public utils::CSingleton<CEngine> {
  public:
    CEngine() {}
    ~CEngine();

    void InitEngine(const UINT32& _uWidth, const UINT32& _uHeight);
    void UpdateEngine();

    const UINT32& GetRenderWidth() { return m_pRenderSystem->GetRenderWindow()->GetWidth(); }
    const UINT32& GetRenderHeight() { return m_pRenderSystem->GetRenderWindow()->GetHeight(); }

    const scene::CSceneManager* GetSceneManager() const { return m_pSceneManager.get(); }
    render::items::CPrimitiveItem* CreatePrimitiveItem(std::vector<float>& _vctVertexData, const UINT32& _uSceneIndex = 0);
  private:
    void Loop();

    std::unique_ptr<global::CFixedTick> m_pFixedTick = nullptr;
    std::unique_ptr<render::CRenderSystem> m_pRenderSystem = nullptr;
    std::unique_ptr<scene::CSceneManager> m_pSceneManager = nullptr;
  };
}




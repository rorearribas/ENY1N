#pragma once
#include "Engine/Render/Render.h"
#include "Engine/Scenes/SceneManager.h"
#include "Libs/Utils/Singleton.h"
#include "Libs/Time/Fixedtick.h"

namespace render { namespace primitive { class CPrimitive; } }

namespace engine 
{
  class CEngine : public utils::CSingleton<CEngine> {
  public:
    CEngine() {}
    ~CEngine();

    void InitEngine(const UINT32& _uWidth, const UINT32& _uHeight);
    void UpdateEngine();

    const scene::CSceneManager* GetSceneManager() const { return m_pSceneManager.get(); }
    const UINT32& GetRenderWidth() { return m_pRender->GetRenderWindow()->GetWidth(); }
    const UINT32& GetRenderHeight() { return m_pRender->GetRenderWindow()->GetHeight(); }

    render::primitive::CPrimitive* CreatePrimitive(const std::vector<render::primitive::CPrimitive::SPrimitiveInfo>& _vctVertexData, const UINT32& _uSceneIndex = 0);
    render::primitive::CPrimitive* CreatePrimitive(const render::primitive::CPrimitive::EPrimitiveType& _ePrimitiveType, const UINT32& _uSceneIndex = 0);
  private:
    void Loop();

    std::unique_ptr<global::CFixedTick> m_pFixedTick = nullptr;
    std::unique_ptr<render::CRender> m_pRender = nullptr;
    std::unique_ptr<scene::CSceneManager> m_pSceneManager = nullptr;
  };
}




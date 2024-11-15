#pragma once
#include "Engine/Render/Render.h"
#include "Engine/Scenes/SceneManager.h"
#include "Engine/Camera/Camera.h"
#include "Libs/Utils/Singleton.h"
#include "Libs/Time/TickRate.h"

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
    const render::CRender* GetRender() const { return m_pRender.get(); }

    render::primitive::CPrimitive* CreatePrimitive(const std::vector<render::primitive::CPrimitive::SPrimitiveInfo>& _vctVertexData, const UINT32& _uSceneIndex = 0);
    render::primitive::CPrimitive* CreatePrimitive(const render::primitive::CPrimitive::EPrimitiveType& _ePrimitiveType, const UINT32& _uSceneIndex = 0);

    void DestroyPrimitive(const render::primitive::CPrimitive* _pPrimitive);
    void DestroyAllPrimimitives(const UINT32& _uSceneIndex = 0);

  private:
    void Loop();

    std::unique_ptr<render::CRender> m_pRender = nullptr;
    std::unique_ptr<scene::CSceneManager> m_pSceneManager = nullptr;
    std::unique_ptr<tick::CTickRate> m_pTickRate = nullptr;

    bool m_bInitialized = false;
  };
}




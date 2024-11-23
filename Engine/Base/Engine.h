#pragma once
#include "Engine/Render/Render.h"
#include "Engine/Scenes/SceneManager.h"
#include "Engine/Camera/Camera.h"
#include "Libs/Utils/Singleton.h"

namespace render { class CCamera; }
namespace render { namespace primitive { class CPrimitive; } }

namespace engine 
{
  class CEngine : public utils::CSingleton<CEngine> {
  public:
    CEngine() {}
    ~CEngine();

    void InitEngine(UINT32 _uWidth, UINT32 _uHeight);

    void BeginDraw();
    void Draw();
    void EndDraw();

    void Update(float _fDeltaTime);

    const scene::CSceneManager* GetSceneManager() const { return m_pSceneManager.get(); }
    const render::CRender* GetRender() const { return m_pRender.get(); }

    render::primitive::CPrimitive* CreatePrimitive(const std::vector<render::primitive::CPrimitive::SPrimitiveInfo>& _vctVertexData, const UINT32& _uSceneIndex = 0);
    render::primitive::CPrimitive* CreatePrimitive(const render::primitive::CPrimitive::EPrimitiveType& _ePrimitiveType, const UINT32& _uSceneIndex = 0);

    void DestroyPrimitive(const render::primitive::CPrimitive* _pPrimitive);
    void DestroyAllPrimimitives(const UINT32& _uSceneIndex = 0);

  private:

    std::unique_ptr<scene::CSceneManager> m_pSceneManager = nullptr;
    std::unique_ptr<render::CRender> m_pRender = nullptr;
    bool m_bInitialized = false;
  };
}




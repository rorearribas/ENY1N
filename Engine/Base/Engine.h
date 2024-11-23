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

    void Update(float _fDeltaTime);
    void InitEngine(UINT32 _uWidth, UINT32 _uHeight);

    void BeginDraw();
    void DrawScene();
    void EndDraw();

    const scene::CSceneManager* GetSceneManager() const { return m_pSceneManager.get(); }
    const render::CRender* GetRender() const { return m_pRender.get(); }
    const render::CCamera* GetCamera() const { return m_pCamera.get(); }

    render::primitive::CPrimitive* CreatePrimitive(const std::vector<render::primitive::CPrimitive::SPrimitiveInfo>& _vctVertexData, const UINT32& _uSceneIndex = 0);
    render::primitive::CPrimitive* CreatePrimitive(const render::primitive::CPrimitive::EPrimitiveType& _ePrimitiveType, const UINT32& _uSceneIndex = 0);

    void DestroyPrimitive(const render::primitive::CPrimitive* _pPrimitive);
    void DestroyAllPrimimitives(const UINT32& _uSceneIndex = 0);

  private:
    void OnWindowResizeEvent(UINT32 _uX, UINT32 _uY);

    std::unique_ptr<scene::CSceneManager> m_pSceneManager = nullptr;
    std::unique_ptr<render::CRender> m_pRender = nullptr;
    std::unique_ptr<render::CCamera> m_pCamera = nullptr;

    bool m_bInitialized = false;
  };
}




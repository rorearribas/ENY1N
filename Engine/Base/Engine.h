#pragma once
#include "Engine/Render/Render.h"
#include "Engine/Scenes/SceneManager.h"
#include "Engine/Camera/Camera.h"
#include "Libs/Utils/Singleton.h"

namespace render { namespace graphics { class CModel; } }
namespace render { namespace graphics { class CPrimitive; } }
namespace render { class CCamera; }

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

    render::graphics::CPrimitive* CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData, const UINT32& _uSceneIndex = 0);
    render::graphics::CPrimitive* CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType, const UINT32& _uSceneIndex = 0);
    render::graphics::CModel* CreateModel(const char* _sPath, const UINT32& _uSceneIndex = 0);

    void DestroyPrimitive(const render::graphics::CPrimitive* _pPrimitive);
    void DestroyAllPrimimitives(const UINT32& _uSceneIndex = 0);

    void DestroyModel(const render::graphics::CModel* _pModel);
    void DestroyAllModels(const UINT32& _uSceneIndex = 0);

  private:
    void OnWindowResizeEvent(UINT32 _uX, UINT32 _uY);

    std::unique_ptr<scene::CSceneManager> m_pSceneManager = nullptr;
    std::unique_ptr<render::CRender> m_pRender = nullptr;
    std::unique_ptr<render::CCamera> m_pCamera = nullptr;

    bool m_bInitialized = false;
  public:
  };
}




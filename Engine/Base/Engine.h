#pragma once
#include "Engine/Render/Render.h"
#include "Engine/Scenes/SceneManager.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Render/Shader.h"
#include "Libs/Utils/Singleton.h"

namespace render { namespace lights { class CLight; } }
namespace render { namespace graphics { class CModel; } }
namespace render { namespace graphics { class CPrimitive; } }
namespace render { class CCamera; }

namespace engine 
{
  class CEngine : public utils::CSingleton<CEngine> {
  public:
    CEngine() {}
    ~CEngine();

    void InitEngine(uint32_t _uWidth, uint32_t _uHeight);
    void Update(float _fDeltaTime);

    void PushBeginDraw();
    void PushDrawProcess();
    void PushEndDraw();

    scene::CSceneManager* GetSceneManager() const { return m_pSceneManager.get(); }
    render::CRender* GetRender() const { return m_pRender.get(); }
    render::CCamera* GetCamera() const { return m_pCamera.get(); }

    // Creation elements
    render::graphics::CPrimitive* CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData, uint32_t _uSceneIndex = 0);
    render::graphics::CPrimitive* CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType, uint32_t _uSceneIndex = 0);
    render::graphics::CModel* CreateModel(const char* _sModelPath, const char* _sBaseMltDir, uint32_t _uSceneIndex = 0);

    render::lights::CDirectionalLight* CreateDirectionalLight(uint32_t _uSceneIndex = 0);
    render::lights::CPointLight* CreatePointLight(uint32_t _uSceneIndex = 0);
    render::lights::CSpotLight* CreateSpotLight(uint32_t _uSceneIndex = 0);

    void DestroyPrimitive(render::graphics::CPrimitive*& _pPrimitive);
    void DestroyModel(render::graphics::CModel*& _pModel);
    void DestroyLight(render::lights::CLight*& pLight_);

  private:
    void OnWindowResizeEvent(uint32_t _uX, uint32_t _uY);

    std::unique_ptr<scene::CSceneManager> m_pSceneManager = nullptr;
    std::unique_ptr<render::CRender> m_pRender = nullptr;
    std::unique_ptr<render::CCamera> m_pCamera = nullptr;

    bool m_bInitialized = false;
  };
}




#pragma once
#include "Engine/Render/Render.h"
#include "Engine/Scenes/SceneManager.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Render/Shader.h"
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

    scene::CSceneManager* GetSceneManager() const { return m_pSceneManager.get(); }
    render::CRender* GetRender() const { return m_pRender.get(); }
    render::CCamera* GetCamera() const { return m_pCamera.get(); }

    // Shaders
    render::shader::CShader<render::shader::SPixelShader>* GetPixelShader() const { return m_pGlobalPixelShader.get();}
    render::shader::CShader<render::shader::SVertexShader>* GetVertexShader() const { return m_pGlobalVertexShader.get();}

    // Creation
    render::graphics::CPrimitive* CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData, const uint32_t& _uSceneIndex = 0);
    render::graphics::CPrimitive* CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType, const uint32_t& _uSceneIndex = 0);
    render::graphics::CModel* CreateModel(const char* _sModelPath, const char* _sBaseMltDir, const uint32_t& _uSceneIndex = 0);

    render::lights::CDirectionalLight* CreateDirectionalLight();
    render::lights::CPointLight* CreatePointLight();

    void DestroyPrimitive(render::graphics::CPrimitive*& _pPrimitive);
    void DestroyModel(render::graphics::CModel*& _pModel);
    void DestroyLight(render::lights::CLight*& pLight_);

    void DestroyAllPrimimitives(const uint32_t& _uSceneIndex = 0);
    void DestroyAllModels(const uint32_t& _uSceneIndex = 0);

  private:
    void OnWindowResizeEvent(uint32_t _uX, uint32_t _uY);

    std::unique_ptr<scene::CSceneManager> m_pSceneManager = nullptr;

    std::unique_ptr<render::CRender> m_pRender = nullptr;
    std::unique_ptr<render::CCamera> m_pCamera = nullptr;

    std::unique_ptr<render::shader::CShader<render::shader::SPixelShader>> m_pGlobalPixelShader;
    std::unique_ptr<render::shader::CShader<render::shader::SVertexShader>> m_pGlobalVertexShader;

    bool m_bInitialized = false;
  public:
  };
}




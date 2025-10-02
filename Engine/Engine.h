#pragma once
#include "Engine/Render/Render.h"
#include "Libs/Utils/Singleton.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Scenes/SceneManager.h"

namespace render { namespace lights { class CBaseLight; } }
namespace render { namespace lights { class CSpotLight; } }
namespace render { namespace lights { class CPointLight; } }
namespace render { namespace lights { class CDirectionalLight; } }

namespace render { namespace graphics { class CModel; } }
namespace render { namespace graphics { class CPrimitive; } }
namespace render { class CCamera; }

namespace engine
{
  class CEngine : public utils::CSingleton<CEngine>
  {
  public:
    CEngine() {}
    ~CEngine();

    void Init(uint32_t _uWidth, uint32_t _uHeight);

    void PushBeginDraw();
    void PushDrawProcess();
    void PushEndDraw();

    scene::CSceneManager* GetSceneManager() const { return m_pSceneManager.get(); }
    render::CRender* GetRender() const { return m_pRender.get(); }
    render::CCamera* GetCamera() const { return m_pCamera.get(); }

    // Debug creation
    void DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode = render::ERenderMode::WIREFRAME);
    void DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Size, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, render::ERenderMode = render::ERenderMode::WIREFRAME);
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode = render::ERenderMode::WIREFRAME);
    void DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode = render::ERenderMode::WIREFRAME);
    void DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color);

    // Element creation
    render::graphics::CPrimitive* const CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType, render::ERenderMode = render::ERenderMode::SOLID, uint32_t _uSceneIndex = 0);
    render::graphics::CModel* const CreateModel(const char* _sModelPath, const char* _sBaseMltDir, uint32_t _uSceneIndex = 0);

    render::lights::CDirectionalLight* const CreateDirectionalLight(uint32_t _uSceneIndex = 0);
    render::lights::CPointLight* const CreatePointLight(uint32_t _uSceneIndex = 0);
    render::lights::CSpotLight* const CreateSpotLight(uint32_t _uSceneIndex = 0);

    void DestroyPrimitive(render::graphics::CPrimitive*& _pPrimitive);
    void DestroyModel(render::graphics::CModel*& _pModel);
    void DestroyLight(render::lights::CBaseLight*& pLight_);

  private:
    void OnWindowResizeEvent(uint32_t _uX, uint32_t _uY);

    std::unique_ptr<scene::CSceneManager> m_pSceneManager = nullptr;
    std::unique_ptr<render::CRender> m_pRender = nullptr;
    std::unique_ptr<render::CCamera> m_pCamera = nullptr;

    bool m_bInitialized = false;
  };
}




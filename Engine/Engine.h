#pragma once
#include "Engine/Render/Render.h"
#include "Libs/Utils/Singleton.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Scenes/SceneManager.h"

namespace render { namespace lights { class CBaseLight; } }
namespace render { namespace lights { class CSpotLight; } }
namespace render { namespace lights { class CPointLight; } }
namespace render { namespace lights { class CDirectionalLight; } }

namespace render { namespace gfx { class CModel; } }
namespace render { namespace gfx { class CPrimitive; } }
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

    scene::CSceneManager* GetSceneManager() const { return m_pSceneManager; }
    render::CRender* GetRender() const { return m_pRender; }
    render::CCamera* GetCamera() const { return m_pCamera; }

    // Debug creation
    void DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode);
    void DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Size, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color);

    // Element creation
    render::gfx::CPrimitive* const CreatePrimitive(render::gfx::EPrimitiveType _eType, render::ERenderMode _eRenderMode, uint32_t _uSceneIndex = 0);
    render::gfx::CModel* const CreateModel(const char* _sModelPath, uint32_t _uSceneIndex = 0);

    render::lights::CDirectionalLight* const CreateDirectionalLight(uint32_t _uSceneIndex = 0);
    render::lights::CPointLight* const CreatePointLight(uint32_t _uSceneIndex = 0);
    render::lights::CSpotLight* const CreateSpotLight(uint32_t _uSceneIndex = 0);

    void DestroyPrimitive(render::gfx::CPrimitive*& _pPrimitive);
    void DestroyModel(render::gfx::CModel*& _pModel);
    void DestroyLight(render::lights::CBaseLight*& pLight_);

  private:
    void OnWindowResizeEvent(uint32_t _uX, uint32_t _uY);

    scene::CSceneManager* m_pSceneManager = nullptr;
    render::CRender* m_pRender = nullptr;
    render::CCamera* m_pCamera = nullptr;

    bool m_bInitialized = false;
  };
}




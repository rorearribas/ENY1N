#pragma once
#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Scenes/SceneManager.h"
#include "Libs/Utils/Singleton.h"

namespace render { class CRender; }
namespace render { class CCamera; }

namespace render { namespace gfx { class CModel; } }
namespace render { namespace gfx { class CPrimitive; } }

namespace render { namespace lights { class CLight; } }
namespace render { namespace lights { class CSpotLight; } }
namespace render { namespace lights { class CPointLight; } }
namespace render { namespace lights { class CDirectionalLight; } }

namespace engine
{
  class CEngine : public utils::CSingleton<CEngine>
  {
  public:
    CEngine() {}
    ~CEngine();

    void Init(uint32_t _uWidth, uint32_t _uHeight);

    void PushBeginDraw();
    void PushDraw();
    void PushEndDraw();

    // Getters
    inline scene::CSceneManager* GetSceneManager() const { return m_pSceneManager; }
    inline render::CRender* GetRender() const { return m_pRender; }
    inline render::CCamera* GetCamera() const { return m_pCamera; }

    // Handle graphics
    render::gfx::CPrimitive* const CreatePrimitive(render::EPrimitiveType _eType, render::ERenderMode _eRenderMode, uint32_t _uSceneIndex = 0);
    bool DestroyPrimitive(render::gfx::CPrimitive*& _pPrimitive_);

    utils::CWeakPtr<render::gfx::CModel> const LoadModel(const char* _sModelPath, uint32_t _uSceneIndex = 0);
    bool DestroyModel(utils::CWeakPtr<render::gfx::CModel> _wpModel_, uint32_t _uSceneIndex = 0);

    // Handle lights
    render::lights::CDirectionalLight* const CreateDirectionalLight(uint32_t _uSceneIndex = 0);
    render::lights::CPointLight* const CreatePointLight(uint32_t _uSceneIndex = 0);
    render::lights::CSpotLight* const CreateSpotLight(uint32_t _uSceneIndex = 0);
    bool DestroyLight(render::lights::CLight*& _pLight_);

    // Debug creation
    void DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode);
    void DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Size, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color);

  private:
    void OnWindowResizeEvent(uint32_t _uX, uint32_t _uY);

    scene::CSceneManager* m_pSceneManager = nullptr;
    render::CRender* m_pRender = nullptr;
    render::CCamera* m_pCamera = nullptr;

    bool m_bInitialized = false;
  };
}




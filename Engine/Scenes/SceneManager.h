#pragma once
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Utils/Plane.h"
#include "Engine/Scenes/Scene.h"
#include "Libs/Utils/Singleton.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/RenderTypes.h"

namespace scene
{
  class CSceneManager
  {
  public:
    static int constexpr s_iMaxScenes = 1;
    typedef std::array<CScene*, s_iMaxScenes> TSceneList;

  public:
    CSceneManager() { CreateScenes(); }
    ~CSceneManager() { DestroyAllScenes(); }

    // Handle scene
    void SetSceneEnabled(uint32_t _uIndex, bool _bEnabled) const;
    inline scene::CScene* const GetCurrentScene() { return m_pCurrentScene; };
    inline const TSceneList& GetScenes() { return m_lstScenes; }

    // Handle graphics
    render::gfx::CPrimitive* const CreatePrimitive(const render::EPrimitive&, render::ERenderMode, uint32_t _uSceneIndex = 0);
    bool DestroyPrimitive(render::gfx::CPrimitive*& _pPrimitive_, uint32_t _uSceneIndex = 0);

    utils::CWeakPtr<render::gfx::CModel> const LoadModel(const char*, uint32_t _uSceneIndex = 0);
    bool DestroyModel(utils::CWeakPtr<render::gfx::CModel> _wpModel_, uint32_t _uSceneIndex = 0);

    // Handle lights
    render::lights::CDirectionalLight* const CreateDirectionalLight(uint32_t _uSceneIndex = 0);
    render::lights::CPointLight* const CreatePointLight(uint32_t _uSceneIndex = 0);
    render::lights::CSpotLight* const CreateSpotLight(uint32_t _uSceneIndex = 0);
    bool DestroyLight(render::lights::CLight*& _pLight_, uint32_t _uSceneIndex = 0);

    // Debug creation
    void DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode);
    void DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Size, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iStacks, int _iSlices, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color);

  private:
    void CreateScenes();
    void DestroyAllScenes();

  private:
    TSceneList m_lstScenes = TSceneList();
    mutable scene::CScene* m_pCurrentScene = nullptr;
  };
}




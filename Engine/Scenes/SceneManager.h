#pragma once
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Scenes/RenderScene.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/RenderTypes.h"

#include "Libs/Utils/Singleton.h"
#include "Libs/Utils/FixedPool.h"

namespace scene
{
  class CSceneManager
  {
  public:
    static const uint32_t s_uMaxCameras = 2u;
    typedef utils::CFixedPool<render::CCamera, s_uMaxCameras> TCameraList;

    static int constexpr s_iMaxScenes = 1;
    typedef utils::CFixedPool<scene::CRenderScene, s_iMaxScenes> TSceneList;

  public:
    CSceneManager() { Setup(); }
    ~CSceneManager() { Clear(); }

    // Handle scene
    void SetSceneEnabled(uint32_t _uIndex, bool _bEnabled) const;
    inline scene::CRenderScene* const GetCurrentScene() { return m_pCurrentScene.IsValid() ? m_pCurrentScene.GetPtr() : nullptr; };

    inline const TSceneList& GetScenes() { return m_lstScenes; }
    inline render::CCamera* GetRenderCamera() const { return m_lstCameras.front(); }
    inline render::CCamera* GetShadowCamera() const { return m_lstCameras.back(); }

    // Handle models
    utils::CWeakPtr<render::gfx::CModel> const LoadModel(const char*, uint32_t _uSceneIndex = 0);
    bool DestroyModel(utils::CWeakPtr<render::gfx::CModel> _wpModel_, uint32_t _uSceneIndex = 0);

    // Handle primitives
    utils::CWeakPtr<render::gfx::CPrimitive> const CreatePrimitive(const render::EPrimitive&, render::ERenderMode, uint32_t _uSceneIndex = 0);
    bool DestroyPrimitive(utils::CWeakPtr<render::gfx::CPrimitive> _pPrimitive_, uint32_t _uSceneIndex = 0);

    // Handle lights
    utils::CWeakPtr<render::lights::CDirectionalLight> const CreateDirectionalLight(uint32_t _uSceneIndex = 0);
    utils::CWeakPtr<render::lights::CPointLight> const CreatePointLight(uint32_t _uSceneIndex = 0);
    utils::CWeakPtr<render::lights::CSpotLight> const CreateSpotLight(uint32_t _uSceneIndex = 0);
    bool DestroyLight(utils::CWeakPtr<render::lights::CLight> _pLight, uint32_t _uSceneIndex = 0);

    // Debug creation
#ifdef _DEBUG
    void DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode);
    void DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iStacks, int _iSlices, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawPlane(const math::CPlane& _rPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color);
#endif

  private:
    void Setup();
    void Clear();

  private:
    TSceneList m_lstScenes = TSceneList();
    TCameraList m_lstCameras = TCameraList();
    mutable utils::CWeakPtr<scene::CRenderScene> m_pCurrentScene;
  };
}




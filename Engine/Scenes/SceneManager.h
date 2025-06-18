#pragma once
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Utils/Plane.h"
#include "Engine/Scenes/Scene.h"
#include "Libs/Utils/Singleton.h"
#include "Engine/Render/Render.h"

namespace scene
{
  class CSceneManager
  {
  public:
    static int constexpr s_iMaxScenes = 5;
    typedef std::array<CScene*, s_iMaxScenes> TSceneList;

    CSceneManager();
    ~CSceneManager();

    const TSceneList& GetScenes() { return m_vctScenes; }
    scene::CScene* const GetCurrentScene() { return m_pCurrentScene; };

    void SetSceneEnabled(uint32_t _uIndex, bool _bEnabled) const;
    void DisableAllScenes() const;

    // Debug creation
    void DrawCapsule(const math::CVector3& _v3Pos, float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode = render::ERenderMode::WIREFRAME);
    void DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Size, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, render::ERenderMode = render::ERenderMode::WIREFRAME);
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iStacks, int _iSlices, const math::CVector3& _v3Color, render::ERenderMode = render::ERenderMode::WIREFRAME);
    void DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode = render::ERenderMode::WIREFRAME);
    void DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color);

    // Element creation
    render::graphics::CPrimitive* const CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType&, render::ERenderMode = render::ERenderMode::SOLID, uint32_t _uSceneIndex = 0);
    render::graphics::CModel* const CreateModel(const char*, const char*, uint32_t _uSceneIndex = 0);

    render::lights::CDirectionalLight* const CreateDirectionalLight(uint32_t _uSceneIndex = 0);
    render::lights::CPointLight* const CreatePointLight(uint32_t _uSceneIndex = 0);
    render::lights::CSpotLight* const CreateSpotLight(uint32_t _uSceneIndex = 0);

    void DestroyPrimitive(render::graphics::CPrimitive*& _pPrimitive_, uint32_t _uSceneIndex = 0);
    void DestroyModel(render::graphics::CModel*& pModel_, uint32_t _uSceneIndex = 0);
    void DestroyLight(render::lights::CBaseLight*& _pLight_, uint32_t _uSceneIndex = 0);

  private:
    void CreateScenes();
    void DestroyAllScenes();

    TSceneList m_vctScenes = TSceneList();
    mutable scene::CScene* m_pCurrentScene = nullptr;
  };
}




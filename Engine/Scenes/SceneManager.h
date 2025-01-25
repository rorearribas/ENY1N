#pragma once
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Scenes/Scene.h"
#include "Libs/Utils/Singleton.h"
#include <array>

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

    void SetSceneEnabled(const UINT32& _uIndex, bool _bEnabled) const;
    void DisableAllScenes() const;

    render::graphics::CPrimitive* CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData, const uint32_t& _uSceneIndex = 0);
    render::graphics::CPrimitive* CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType, const uint32_t& _uSceneIndex = 0);
    render::graphics::CModel* CreateModel(const char* _sModelPath, const char* _sBaseMltDir, const uint32_t& _uSceneIndex = 0);

    render::lights::CDirectionalLight* CreateDirectionalLight(const uint32_t& _uSceneIndex = 0);
    render::lights::CPointLight* CreatePointLight(const uint32_t& _uSceneIndex = 0);

    void DestroyModel(render::graphics::CModel*& pModel_);
    void DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_);
    void DestroyLight(render::lights::CLight*& pLight_);

    void DestroyAllPrimimitives(const UINT32& _uSceneIndex);
    void DestroyAllModels(const UINT32& _uSceneIndex);

  private:
    void CreateScenes();
    void DestroyAllScenes();

    TSceneList m_vctScenes = {};
    mutable scene::CScene* m_pCurrentScene = nullptr;
  };
}




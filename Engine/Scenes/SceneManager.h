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

    void Draw();
    const TSceneList& GetScenes() { return m_vctScenes; }
    scene::CScene* const GetCurrentScene() { return m_pCurrentScene; };

    void SetSceneEnabled(const UINT32& _uIndex, bool _bEnabled) const;
    void DisableAllScenes() const;

    render::graphics::CPrimitive* CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData, const UINT32& _uSceneIndex = 0);
    render::graphics::CPrimitive* CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType, const UINT32& _uSceneIndex = 0);
    render::graphics::CModel* CreateModel(const char* _sModelPath, const char* _sBaseMltDir, const UINT32& _uSceneIndex = 0);

    void DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_);
    void DestroyAllPrimimitives(const UINT32& _uSceneIndex);

    void DestroyModel(render::graphics::CModel*& pModel_);
    void DestroyAllModels(const UINT32& _uSceneIndex);

  private:
    void CreateScenes();
    void DestroyAllScenes();

    TSceneList m_vctScenes = {};
    mutable scene::CScene* m_pCurrentScene = nullptr;
  };
}




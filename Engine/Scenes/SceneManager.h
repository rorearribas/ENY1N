#pragma once
#include "Engine/Render/Primitives/Primitive.h"
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

    render::primitive::CPrimitive* CreatePrimitive(const std::vector<render::primitive::CPrimitive::SPrimitiveInfo>& _vctVertexData, const UINT32& _uSceneIndex = 0);
    render::primitive::CPrimitive* CreatePrimitive(const render::primitive::CPrimitive::EPrimitiveType& _ePrimitiveType, const UINT32& _uSceneIndex = 0);

    void DestroyPrimitive(const render::primitive::CPrimitive* _pPrimitive);
    void DestroyAllPrimimitives(const UINT32& _uSceneIndex);

  private:
    void CreateScenes();
    void DestroyAllScenes();

    TSceneList m_vctScenes = {};
    mutable scene::CScene* m_pCurrentScene = nullptr;
  };
}




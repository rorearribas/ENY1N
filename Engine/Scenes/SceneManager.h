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

    CSceneManager() {}
    ~CSceneManager();

    void InitScenes();
    void DisableAllScenes() const;
    void SetSceneEnabled(const UINT32& _uIndex, bool _bEnabled) const;

    const TSceneList& GetScenes() { return m_vctScenes; }
    render::primitive::CPrimitive* CreatePrimitive(const std::vector<render::primitive::CPrimitive::SPrimitiveInfo>& _vctVertexData, const UINT32& _uSceneIndex = 0);
    render::primitive::CPrimitive* CreatePrimitive(const render::primitive::CPrimitive::EPrimitiveType& _ePrimitiveType, const UINT32& _uSceneIndex = 0);
  private:
    void DestroyAllScenes();
    TSceneList m_vctScenes = {};
  };
}




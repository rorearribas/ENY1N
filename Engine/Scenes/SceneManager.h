#pragma once
#include "Engine/Render/RenderItem/PrimitiveItem.h"
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
    void SetSceneEnabled(bool _bEnabled, const UINT32& _uIndex) const;

    const TSceneList& GetScenes() { return m_vctScenes; }
    render::items::CPrimitiveItem* CreatePrimitiveItem(std::vector<float>& _vctVertexData, const UINT32& _uSceneIndex = 0);
  private:
    void DestroyAllScenes();
    TSceneList m_vctScenes = {};
  };
}




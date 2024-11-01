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

    CSceneManager() {}
    ~CSceneManager();

    void InitScenes();
    const std::array<CScene*, s_iMaxScenes>& GetScenes() { return m_vctScenes; }
    render::items::CPrimitiveItem* CreatePrimitiveItem(std::vector<float>& _vctVertexData, const UINT32& _uSceneIndex = 0);

  private:
    void DestroyAllScenes();
    std::array<CScene*, s_iMaxScenes> m_vctScenes = {};
  };
}




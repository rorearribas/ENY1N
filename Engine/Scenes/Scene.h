#pragma once
#include "Engine/Render/RenderItem/PrimitiveItem.h"
#include <array>

namespace scene
{
  class CScene 
  {
  public:
    static int constexpr s_iMaxPrimitives = 200;
    typedef std::array<render::items::CPrimitiveItem*, s_iMaxPrimitives> TPrimitiveList;

  public:
    CScene(const UINT32& _uIndex) : m_uSceneIdx(_uIndex) {}
    ~CScene();

    void DrawScene();
    void SetSceneEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }

    render::items::CPrimitiveItem* CreatePrimitiveItem(std::vector<float>& _vctVertexData);
    const TPrimitiveList& GetPrimitives() const { return m_vctPrimitiveItems; }

    const UINT32& GetSceneIndex() const { return m_uSceneIdx; }
    const bool& IsSceneEnabled() const { return m_bEnabled; }

  private:
    void DrawPrimitives();
    void DestroyAllPrimitives();

    TPrimitiveList m_vctPrimitiveItems = {};
    int m_iRegisteredPrimitives = -1;

    bool m_bEnabled = false;
    UINT32 m_uSceneIdx = 0;
  };
}



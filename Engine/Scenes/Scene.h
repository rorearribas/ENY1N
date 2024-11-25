#pragma once
#include "Engine/Render/Graphics/Primitive.h"
#include <array>

namespace scene
{
  class CScene 
  {
  public:
    static int constexpr s_iMaxPrimitives = 500;
    typedef std::array<render::graphics::CPrimitive*, s_iMaxPrimitives> TPrimitiveList;

  public:
    CScene(const UINT32& _uIndex) : m_uSceneIdx(_uIndex) {}
    ~CScene();

    void DrawScene();
    void SetSceneEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }

    const TPrimitiveList& GetPrimitives() { return m_vctPrimitiveItems; }
    render::graphics::CPrimitive* CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData);
    render::graphics::CPrimitive* CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType);

    void DestroyPrimitive(const render::graphics::CPrimitive* _pPrimitive);
    void DestroyAllPrimitives();

    const UINT32& GetSceneIndex() const { return m_uSceneIdx; }
    const bool& IsEnabled() const { return m_bEnabled; }

  private:
    // Draw
    void DrawPrimitives();

    TPrimitiveList m_vctPrimitiveItems = {};
    int m_iRegisteredPrimitives = 0;

    bool m_bEnabled = false;
    UINT32 m_uSceneIdx = 0;
  };
}



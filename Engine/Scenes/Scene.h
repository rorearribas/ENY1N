#pragma once
#include "Engine/Render/Primitives/Primitive.h"
#include <array>

namespace scene
{
  class CScene 
  {
  public:
    static int constexpr s_iMaxPrimitives = 500;
    typedef std::array<render::primitive::CPrimitive*, s_iMaxPrimitives> TPrimitiveList;

  public:
    CScene(const UINT32& _uIndex) : m_uSceneIdx(_uIndex) {}
    ~CScene();

    void DrawScene();
    void SetSceneEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }

    const TPrimitiveList& GetPrimitives() const { return m_vctPrimitiveItems; }
    render::primitive::CPrimitive* CreatePrimitive(const std::vector<render::primitive::CPrimitive::SPrimitiveInfo>& _vctVertexData);
    render::primitive::CPrimitive* CreatePrimitive(const render::primitive::CPrimitive::EPrimitiveType& _ePrimitiveType);

    const UINT32& GetSceneIndex() const { return m_uSceneIdx; }
    const bool& IsEnabled() const { return m_bEnabled; }

  private:
    void DrawPrimitives();
    void DestroyAllPrimitives();

    TPrimitiveList m_vctPrimitiveItems = {};
    int m_iRegisteredPrimitives = 0;

    bool m_bEnabled = false;
    UINT32 m_uSceneIdx = 0;
  };
}



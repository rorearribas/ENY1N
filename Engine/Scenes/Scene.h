#pragma once
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include <array>

namespace scene
{
  class CScene
  {
  public:
    static int constexpr s_iMaxPrimitives = 500;
    static int constexpr s_iMaxModels = 5000;

    typedef std::array<render::graphics::CPrimitive*, s_iMaxPrimitives> TPrimitiveList;
    typedef std::array<render::graphics::CModel*, s_iMaxModels> TModelList;

  public:
    CScene(const UINT32& _uIndex) : m_uSceneIdx(_uIndex) {}
    ~CScene();

    void DrawScene();
    void SetSceneEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }

    const TPrimitiveList& GetPrimitives() { return m_vctPrimitiveItems; }
    const TModelList& GetModels() { return m_vctModels; }

    render::graphics::CPrimitive* CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData);
    render::graphics::CPrimitive* CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType);
    render::graphics::CModel* CreateModel(const char* _sModelPath, const char* _sBaseMltDir);

    void DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_);
    void DestroyModel(render::graphics::CModel*& pModel_);

    const UINT32& GetSceneIndex() const { return m_uSceneIdx; }
    const bool& IsEnabled() const { return m_bEnabled; }

    void DestroyAllPrimitives();
    void DestroyAllModels();

  private:
    void DrawPrimitives();
    void DrawModels();

  private:
    bool m_bEnabled = false;
    UINT32 m_uSceneIdx = 0;

    TPrimitiveList m_vctPrimitiveItems = {};
    int m_iRegisteredPrimitives = 0;

    TModelList m_vctModels = {};
    int m_iRegisteredModels = 0;
  };
}



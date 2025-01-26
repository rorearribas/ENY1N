#pragma once
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include <array>

namespace render { namespace lights { class CLight; } }
namespace render { namespace lights { class CPointLight; } }
namespace render { namespace lights { class CDirectionalLight; } }
namespace render { class CRender; }

namespace scene
{
  class CScene
  {
  public:
    static int constexpr s_iMaxLights = 100;
    static int constexpr s_iMaxModels = 2500;
    static int constexpr s_iMaxPrimitives = 500;

    typedef std::array<render::graphics::CPrimitive*, s_iMaxPrimitives> TPrimitiveList;
    typedef std::array<render::graphics::CModel*, s_iMaxModels> TModelList;
    typedef std::array<render::lights::CLight*, s_iMaxLights> TLightsList;

  public:
    CScene(const UINT32& _uIndex) : m_uSceneIdx(_uIndex) {}
    ~CScene();

    const TPrimitiveList& GetPrimitives() { return m_vctPrimitiveItems; }
    const TModelList& GetModels() { return m_vctModels; }
    const TLightsList& GetLights() { return m_vctLights; }

    // Graphics
    render::graphics::CPrimitive* CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData);
    render::graphics::CPrimitive* CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType);
    render::graphics::CModel* CreateModel(const char* _sModelPath, const char* _sBaseMltDir);

    // Lights
    render::lights::CDirectionalLight* CreateDirectionalLight();
    render::lights::CPointLight* CreatePointLight();

    void DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_);
    void DestroyModel(render::graphics::CModel*& pModel_);
    void DestroyLight(render::lights::CLight*& pLight_);

    void SetSceneEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }
    const UINT32& GetSceneIndex() const { return m_uSceneIdx; }
    const bool& IsEnabled() const { return m_bEnabled; }

    void DestroyAllPrimitives();
    void DestroyAllModels();
    void DestroyAllLights();

  private:
    friend class render::CRender;

    void DrawPrimitives();
    void DrawModels();
    void UpdateLights();

  private:
    bool m_bEnabled = false;
    UINT32 m_uSceneIdx = 0;

    TPrimitiveList m_vctPrimitiveItems = {};
    uint32_t m_uRegisteredPrimitives = 0;

    TModelList m_vctModels = {};
    uint32_t m_uRegisteredModels = 0;

    TLightsList m_vctLights = {};
    uint32_t m_uRegisteredLights  = 0;
  };
}
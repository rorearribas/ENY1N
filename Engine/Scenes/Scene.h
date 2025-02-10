#pragma once
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Libs/Macros/GlobalMacros.h"
#include <cassert>
#include <array>

namespace render { class CRender; }
namespace render { namespace lights { class CSpotLight; } }
namespace render { namespace lights { class CLight; } }
namespace render { namespace lights { class CPointLight; } }
namespace render { namespace lights { class CDirectionalLight; } }

namespace scene
{
  class CScene
  {
  private:
    static int constexpr s_iMaxSpotLights = 100;
    static int constexpr s_iMaxPointLights = 100;

  public:
    static int constexpr s_iMaxLights = 200;
    static int constexpr s_iMaxModels = 2500;
    static int constexpr s_iMaxPrimitives = 500;

    // Lights
    typedef std::array<render::lights::CPointLight*, s_iMaxPointLights> TPointLightsList;
    typedef std::array<render::lights::CSpotLight*, s_iMaxSpotLights> TSpotLightsList;
    // Graphics
    typedef std::array<render::graphics::CPrimitive*, s_iMaxPrimitives> TPrimitiveList;
    typedef std::array<render::graphics::CModel*, s_iMaxModels> TModelList;

  public:
    CScene(const UINT32& _uIndex);
    ~CScene();

    // Graphics
    render::graphics::CPrimitive* CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData);
    render::graphics::CPrimitive* CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType);
    render::graphics::CModel* CreateModel(const char* _sModelPath, const char* _sBaseMltDir);

    // Lights
    render::lights::CDirectionalLight* CreateDirectionalLight();
    render::lights::CPointLight* CreatePointLight();
    render::lights::CSpotLight* CreateSpotLight();

    void DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_);
    void DestroyModel(render::graphics::CModel*& pModel_);
    void DestroyLight(render::lights::CLight*& pLight_);

    void SetSceneEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }
    const bool& IsEnabled() const { return m_bEnabled; }
    const uint32_t& GetSceneIndex() const { return m_uSceneIdx; }

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

    // Primitives
    TPrimitiveList m_vctPrimitiveItems = {};
    uint32_t m_uRegisteredPrimitives = 0;

    // Models
    TModelList m_vctModels = {};
    uint32_t m_uRegisteredModels = 0;

    // Point lights
    TPointLightsList m_vctPointLights = {};
    uint32_t m_uRegisteredPointLights  = 0;
    // Spot lights
    TSpotLightsList m_vctSpotLights = {};
    uint32_t m_uRegisteredSpotLights  = 0;
    // Directional light
    render::lights::CDirectionalLight* m_pDirectionalLight = nullptr;
    // Global lightning buffer
    CConstantBuffer<SGlobalLightningData<s_iMaxPointLights, s_iMaxSpotLights>> m_oLightningBuffer;
  };
}
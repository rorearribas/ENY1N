#pragma once

#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Lights/LightManager.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Utils/Plane.h"

#include "Libs/Utils/FixedPool.h"
#include "Libs/Utils/UniquePtrList.h"

namespace render { class CCamera; }
namespace render { class CRender; }
namespace render { namespace lights { class CDirectionalLight; } }

namespace scene
{
  class CScene
  {
  private:
    struct TCachedModel
    {
      bool Visible = false;
      render::gfx::TDrawableInstances DrawableInstances;
      uint16_t InstanceCount = 0;
    };

  private:
    // Models
    static constexpr uint16_t s_uMaxModels = 512u;
    typedef utils::CUniquePtrList<render::gfx::CModel, s_uMaxModels> TModels;
    typedef std::array<TCachedModel, s_uMaxModels> TCachedModels;

    // Primitives
    static constexpr uint16_t s_uMaxPrimitives = 128u;
    typedef utils::CFixedPool<render::gfx::CPrimitive, s_uMaxPrimitives> TPrimitives;

    // Debug primitives
    static constexpr uint16_t s_uMaxDebugPrimitives = 256u;
    typedef utils::CFixedPool<render::gfx::CPrimitive, s_uMaxDebugPrimitives> TDebugItems;

  public:
    CScene(uint32_t _uIndex) : m_uSceneIdx(_uIndex) {}
    ~CScene();

    // Handle scene
    inline void SetEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }
    inline const bool IsEnabled() const { return m_bEnabled; }
    inline const uint32_t& GetSceneIndex() const { return m_uSceneIdx; }

    // Handle graphics
    render::gfx::CPrimitive* const CreatePrimitive(render::EPrimitive _eType, render::ERenderMode _eRenderMode);
    bool DestroyPrimitive(render::gfx::CPrimitive*& _pPrimitive_);

    utils::CWeakPtr<render::gfx::CModel> const LoadModel(const char* _sModelPath);
    bool DestroyModel(utils::CWeakPtr<render::gfx::CModel> _pModel_);

    // Handle lights
    render::lights::CDirectionalLight* const CreateDirectionalLight();
    render::lights::CPointLight* const CreatePointLight();
    render::lights::CSpotLight* const CreateSpotLight();
    bool DestroyLight(render::lights::CLight*& pLight_);

    // Debug
    void DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode);
    void DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color);

  private:
    friend class render::CRender;
    void Clear();

    // Handle scene
    void CacheModels(const render::CCamera* _pCamera);
    void ApplyLighting();

    // Draw calls
    void DrawModels();
    void DrawPrimitives(const render::CCamera* _pCamera);

  private:
    bool m_bEnabled = false;
    uint32_t m_uSceneIdx = 0;

  private:
    // Models
    TModels m_lstModels = TModels();
    TCachedModels m_lstCachedModels = TCachedModels();
    uint32_t m_uCachedSize = 0;

    // Primitives
    TPrimitives m_lstPrimitives = TPrimitives();
    TDebugItems m_lstDebugItems = TDebugItems();

    // Lights
    render::lights::CLightManager m_oLightManager;
  };
}
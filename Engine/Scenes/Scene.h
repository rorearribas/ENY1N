#pragma once

#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Lighting/LightManager.h"
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
  // Models
  struct TCachedModel
  {
    bool Visible = false;
    uint16_t Index = 0;

    render::gfx::TDrawableInstances DrawableInstances;
    uint16_t InstanceCount = 0;
  };

  static constexpr uint16_t s_uMaxModels = 128u;
  typedef utils::CUniquePtrList<render::gfx::CModel, s_uMaxModels> TModels;
  typedef std::array<TCachedModel, s_uMaxModels> TCachedModels;

  // Primitives
  static constexpr uint16_t s_uMaxPrimitives = 256u;
  typedef utils::CFixedPool<render::gfx::CPrimitive, s_uMaxPrimitives> TPrimitives;
  typedef std::array<uint16_t, s_uMaxPrimitives> TCachedPrimitives;

  // Debug primitives
  static constexpr uint16_t s_uMaxDebugPrimitives = 1024u;
  typedef utils::CFixedPool<render::gfx::CPrimitive, s_uMaxDebugPrimitives> TDebugPrimitives;
  typedef std::array<uint16_t, s_uMaxDebugPrimitives> TCachedDebugPrimitives;

  class CScene
  {
  public:
    CScene(uint32_t _uIndex) : m_uSceneIdx(_uIndex) {}
    ~CScene();

    // Handle scene
    inline void SetEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }
    inline const bool IsEnabled() const { return m_bEnabled; }
    inline const uint32_t& GetSceneIndex() const { return m_uSceneIdx; }

    // Scene items
    inline const TModels& GetModels() const { return m_lstModels; }
    inline const TPrimitives& GetPrimitives() const { return m_lstPrimitives; }
    inline const TDebugPrimitives& GetDebugPrimitives() const { return m_lstDebugPrimitives; }

    // Cached items
    const TCachedModels& GetCacheModels(uint16_t& _uDrawableCount_) const;
    const TCachedPrimitives& GetCachedPrimitives(uint16_t& _uDrawableCount_) const;
    const TCachedDebugPrimitives& GetCachedDebugPrimitives(uint16_t& _uDrawableCount_) const;

    // Handle graphics
    render::gfx::CPrimitive* const CreatePrimitive(render::EPrimitive _eType, render::ERenderMode _eRenderMode);
    bool DestroyPrimitive(render::gfx::CPrimitive*& _pPrimitive_);
    inline void ClearDebugItems() { m_lstDebugPrimitives.Clear(); }

    utils::CWeakPtr<render::gfx::CModel> const LoadModel(const char* _sModelPath);
    bool DestroyModel(utils::CWeakPtr<render::gfx::CModel> _pModel_);

    // Handle lights
    render::lights::CLightManager* const GetLightManager() { return &m_oLightManager; }
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

    // Cache items
    void CacheModels(render::CCamera* _pCamera);
    void CachePrimitives(render::CCamera* _pCamera);
    void CacheDebugPrimitives(render::CCamera* _pCamera);

    // Draw calls
    void ApplyLighting();

  private:
    bool m_bEnabled = false;
    uint32_t m_uSceneIdx = 0;

  private:
    // Models
    TModels m_lstModels = TModels();
    TCachedModels m_lstCachedModels = TCachedModels();
    uint16_t m_uDrawableModels = 0;

    // Primitives
    TPrimitives m_lstPrimitives = TPrimitives();
    TCachedPrimitives m_lstCachedPrimitives = TCachedPrimitives();
    uint16_t m_uDrawablePrimitives = 0;

    TDebugPrimitives m_lstDebugPrimitives = TDebugPrimitives();
    TCachedDebugPrimitives m_lstCachedDebugPrimitives = TCachedDebugPrimitives();
    uint16_t m_uDrawableDebugPrimitives = 0;

    // Lights
    render::lights::CLightManager m_oLightManager;
  };
}
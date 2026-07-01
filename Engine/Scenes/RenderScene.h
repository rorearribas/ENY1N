#pragma once

#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Buffers/RenderBuffer.h"
#include "Engine/Render/Lighting/LightManager.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Render/Spatial/Octree.h"
#include "Engine/Utils/Plane.h"

#include "Libs/Utils/FixedPool.h"
#include "Libs/Utils/ArenaPool.h"

namespace render { class CCamera; }
namespace render { class CRender; }
namespace render { namespace lights { class CDirectionalLight; } }

namespace scene
{
  // Models memory limits
  constexpr uint32_t MAX_MODELS_VB_SIZE = 1024u * 1024u * 256u;
  constexpr uint32_t MAX_MODELS_IB_SIZE = 1024u * 1024u * 128u;
  // Primitives memory limits
  constexpr uint32_t MAX_PRIMITIVES_VB_SIZE = 1024u * 1024u * 32u;
  constexpr uint32_t MAX_PRIMITIVES_IB_SIZE = 1024u * 1024u * 16u;
#ifdef _DEBUG
  // Debug primitives memory limits
  constexpr uint32_t MAX_DEBUG_PRIMITIVES_VB_SIZE = 1024u * 1024u * 128u;
  constexpr uint32_t MAX_DEBUG_PRIMITIVES_IB_SIZE = 1024u * 1024u * 64u;
#endif

  // Models
  struct TCachedModel
  {
    bool Visible = false;
    uint16_t Index = 0;

    render::gfx::TDrawableInstances DrawableInstances = render::gfx::TDrawableInstances();
    uint16_t InstanceCount = 0;
  };

  // Models
  static constexpr uint16_t s_uMaxModels = 256u;
  typedef utils::CFixedPool<render::gfx::CModel, s_uMaxModels> TModels;
  typedef std::array<TCachedModel, s_uMaxModels> TCachedModels;

  // Primitives
  static constexpr uint16_t s_uMaxPrimitives = 128u;
  typedef utils::CFixedPool<render::gfx::CPrimitive, s_uMaxPrimitives> TPrimitives;
  typedef std::array<uint16_t, s_uMaxPrimitives> TCachedPrimitives;

  // Debug primitives
#ifdef _DEBUG
  static constexpr uint16_t s_uMaxDebugPrimitives = 256u;
  static constexpr uint16_t s_uMaxChunkSize = s_uMaxDebugPrimitives * sizeof(render::gfx::CPrimitive);
  typedef utils::CArenaPool<render::gfx::CPrimitive, s_uMaxChunkSize, s_uMaxDebugPrimitives> TDebugPrimitives;
  typedef std::array<uint16_t, s_uMaxDebugPrimitives> TCachedDebugPrimitives;
#endif

  class CRenderScene
  {
  public:
    CRenderScene(uint32_t _uIndex) : m_uSceneIdx(_uIndex) { SetupBuffers(); }
    ~CRenderScene();

    // Handle scene
    inline void SetEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }
    inline const bool IsEnabled() const { return m_bEnabled; }
    inline const uint32_t& GetSceneIndex() const { return m_uSceneIdx; }

    // Cached items
    void CacheModels(render::CCamera* _pCamera);
    const TCachedModels& GetCachedModels(uint16_t& _uDrawableCount_) const;
    void CachePrimitives(render::CCamera* _pCamera);
    const TCachedPrimitives& GetCachedPrimitives(uint16_t& _uDrawableCount_) const;

    // Scene items
    inline const TModels& GetModels() const { return m_lstModels; }
    inline const TPrimitives& GetPrimitives() const { return m_lstPrimitives; }

    // Handle models
    utils::CWeakPtr<render::gfx::CModel> const LoadModel(const char* _sModelPath);
    bool DestroyModel(utils::CWeakPtr<render::gfx::CModel> _pModel_);

    // Buffers - Models
    ID3D11Buffer* GetModelsVB() const { return m_oModelsVB; }
    ID3D11Buffer* GetModelsIB() const { return m_oModelsIB; }

    // Handle primitives
    utils::CWeakPtr<render::gfx::CPrimitive> const CreatePrimitive(render::EPrimitive _eType, render::ERenderMode _eRenderMode);
    bool DestroyPrimitive(utils::CWeakPtr<render::gfx::CPrimitive> _pPrimitive_);

    // Buffers - Primitives
    ID3D11Buffer* GetPrimitivesVB() const { return m_oPrimitivesVB; }
    ID3D11Buffer* GetPrimitivesIB() const { return m_oPrimitivesIB; }

    // Handle lights
    render::lights::CLightManager* const GetLightManager() { return &m_oLightManager; }
    utils::CWeakPtr<render::lights::CDirectionalLight> const CreateDirectionalLight();
    utils::CWeakPtr<render::lights::CPointLight> const CreatePointLight();
    utils::CWeakPtr<render::lights::CSpotLight> const CreateSpotLight();
    bool DestroyLight(utils::CWeakPtr<render::lights::CLight> _wpLight);

#ifdef _DEBUG
    void CacheDebugPrimitives(render::CCamera* _pCamera);
    const TCachedDebugPrimitives& GetCachedDebugPrimitives(uint16_t& _uDrawableCount_) const;
    inline const TDebugPrimitives& GetDebugPrimitives() const { return m_lstDebugPrimitives; }

    // Debug
    void DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode);
    void DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawPlane(const math::CPlane& _rPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color);

    // Buffers - Debug Primitives
    ID3D11Buffer* GetDebugPrimitivesVB() const { return m_oDebugPrimitivesVB; }
    ID3D11Buffer* GetDebugPrimitivesIB() const { return m_oDebugPrimitivesIB; }
    void ClearDebugItems();

    void DrawOctree() const;
    void RebuildOctree();
#endif

  private:
    HRESULT SetupBuffers();
    void Clear();

  private:
    bool m_bEnabled = false;
    uint32_t m_uSceneIdx = 0;
    render::lights::CLightManager m_oLightManager;

  private:
    // Models
    TModels m_lstModels = TModels();
    TCachedModels m_lstCachedModels = TCachedModels();
    uint16_t m_uDrawableModels = 0;

#ifdef _DEBUG
    // Testing octree
    std::unique_ptr<COctree<render::gfx::CModel>> m_pOctreeModels;
    std::unique_ptr<COctree<render::gfx::CRenderInstance>> m_pOctreeInstances;
#endif

    // Primitives
    TPrimitives m_lstPrimitives = TPrimitives();
    TCachedPrimitives m_lstCachedPrimitives = TCachedPrimitives();
    uint16_t m_uDrawablePrimitives = 0;

    // Debug primitives
#ifdef _DEBUG
    TDebugPrimitives m_lstDebugPrimitives = TDebugPrimitives();
    TCachedDebugPrimitives m_lstCachedDebugPrimitives = TCachedDebugPrimitives();
    uint16_t m_uDrawableDebugPrimitives = 0;
#endif
  private:
    // Render buffers - models
    CRenderBuffer<render::gfx::TVertexData> m_oModelsVB;
    CRenderBuffer<uint32_t> m_oModelsIB;

    // Render buffers - primitives
    CRenderBuffer<math::CVector3> m_oPrimitivesVB;
    CRenderBuffer<uint32_t> m_oPrimitivesIB;

#ifdef _DEBUG
    // Render buffer - debug primitives
    CRenderBuffer<math::CVector3> m_oDebugPrimitivesVB;
    CRenderBuffer<uint32_t> m_oDebugPrimitivesIB;
#endif
  };
}
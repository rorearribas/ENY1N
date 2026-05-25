#pragma once

#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Lighting/LightManager.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Utils/Plane.h"

#include "Libs/Utils/FixedPool.h"
#include "Libs/Utils/UniquePtrList.h"
#include "Engine/Render/Buffers/RenderBuffer.h"

namespace render { class CCamera; }
namespace render { class CRender; }
namespace render { namespace lights { class CDirectionalLight; } }

namespace scene
{
  // GPU Memory
  constexpr uint32_t MAX_MODELS_VB_SIZE = 1024u * 1024u * 256u;
  constexpr uint32_t MAX_MODELS_IB_SIZE = 1024u * 1024u * 128u;
  constexpr uint32_t MAX_PRIMITIVES_VB_SIZE = 1024u * 1024u * 32u;
  constexpr uint32_t MAX_PRIMITIVES_IB_SIZE = 1024u * 1024u * 16u;

  // Models
  struct TCachedModel
  {
    bool Visible = false;
    uint16_t Index = 0;

    render::gfx::TDrawableInstances DrawableInstances;
    uint16_t InstanceCount = 0;
  };

  // Models
  static constexpr uint16_t s_uMaxModels = 256u;
  typedef utils::CUniquePtrList<render::gfx::CModel, s_uMaxModels> TModels;
  typedef std::array<TCachedModel, s_uMaxModels> TCachedModels;

  // Primitives
  static constexpr uint16_t s_uMaxPrimitives = 128u;
  typedef utils::CFixedPool<render::gfx::CPrimitive, s_uMaxPrimitives> TPrimitives;
  typedef std::array<uint16_t, s_uMaxPrimitives> TCachedPrimitives;

  // Debug primitives
  static constexpr uint16_t s_uMaxDebugPrimitives = 256u;
  typedef utils::CFixedPool<render::gfx::CPrimitive, s_uMaxDebugPrimitives> TDebugPrimitives;
  typedef std::array<uint16_t, s_uMaxDebugPrimitives> TCachedDebugPrimitives;

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
    void CacheDebugPrimitives(render::CCamera* _pCamera);
    const TCachedDebugPrimitives& GetCachedDebugPrimitives(uint16_t& _uDrawableCount_) const;

    // Scene items
    inline const TModels& GetModels() const { return m_lstModels; }
    inline const TPrimitives& GetPrimitives() const { return m_lstPrimitives; }
    inline const TDebugPrimitives& GetDebugPrimitives() const { return m_lstDebugPrimitives; }

    // Handle models
    utils::CWeakPtr<render::gfx::CModel> const LoadModel(const char* _sModelPath);
    bool DestroyModel(utils::CWeakPtr<render::gfx::CModel> _pModel_);

    // Handle primitives
    render::gfx::CPrimitive* const CreatePrimitive(render::EPrimitive _eType, render::ERenderMode _eRenderMode);
    bool DestroyPrimitive(render::gfx::CPrimitive*& _pPrimitive_);
    void ClearDebugItems();

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

    // Buffers - Models
    ID3D11Buffer* GetModelsVB() const { return m_oModelsVB; }
    ID3D11Buffer* GetModelsIB() const { return m_oModelsIB; }

    // Buffers - Primitives
    ID3D11Buffer* GetPrimitivesVB() const { return m_oPrimitivesVB; }
    ID3D11Buffer* GetPrimitivesIB() const { return m_oPrimitivesIB; }

    // Buffers - Debug Primitives
    ID3D11Buffer* GetDebugPrimitivesVB() const { return m_oDebugPrimitivesVB; }
    ID3D11Buffer* GetDebugPrimitivesIB() const { return m_oDebugPrimitivesIB; }

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

    // Primitives
    TPrimitives m_lstPrimitives = TPrimitives();
    TCachedPrimitives m_lstCachedPrimitives = TCachedPrimitives();
    uint16_t m_uDrawablePrimitives = 0;

    // Debug primitives
    TDebugPrimitives m_lstDebugPrimitives = TDebugPrimitives();
    TCachedDebugPrimitives m_lstCachedDebugPrimitives = TCachedDebugPrimitives();
    uint16_t m_uDrawableDebugPrimitives = 0;

  private:
    // Render buffers - models
    CRenderBuffer<render::gfx::TVertexData> m_oModelsVB;
    CRenderBuffer<uint32_t> m_oModelsIB;

    // Render buffers - primitives
    CRenderBuffer<math::CVector3> m_oPrimitivesVB;
    CRenderBuffer<uint32_t> m_oPrimitivesIB;

    // Render buffer - debug primitives
    CRenderBuffer<math::CVector3> m_oDebugPrimitivesVB;
    CRenderBuffer<uint32_t> m_oDebugPrimitivesIB;
  };
}
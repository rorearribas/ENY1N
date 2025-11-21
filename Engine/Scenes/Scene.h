#pragma once
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Render/Lights/SpotLight.h"
#include "Engine/Render/Lights/PointLight.h"
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
    //@Note: if you change this values you will also need to change the array size in hlsl
    static constexpr uint32_t s_uMaxSpotLights = 100u;
    static constexpr uint32_t s_uMaxPointLights = 100u;

    typedef CConstantBuffer<SGlobalLightingData<s_uMaxPointLights, s_uMaxSpotLights>> TLightingBuffer;
    typedef utils::CFixedPool<render::lights::CPointLight, s_uMaxPointLights> TPointLightsList;
    typedef utils::CFixedPool<render::lights::CSpotLight, s_uMaxSpotLights> TSpotLightsList;

  private:
    // Models
    static constexpr uint32_t s_uMaxModels = 500u;
    typedef utils::CUniquePtrList<render::gfx::CModel, s_uMaxModels> TModels;
    typedef CConstantBuffer<SHandleInstancing> TInstancingBuffer;

    // Primitives
    static constexpr uint32_t s_uMaxPrimitives = 500u;
    typedef utils::CFixedPool<render::gfx::CPrimitive, s_uMaxPrimitives> TPrimitives;

    // Debug primitives
    static constexpr uint32_t s_uMaxDebugPrimitives = 1000u;
    typedef utils::CFixedPool<render::gfx::CPrimitive, s_uMaxDebugPrimitives> TDebugItems;

  public:
    CScene(uint32_t _uIndex);
    ~CScene();

    // Handle scene
    inline void SetEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }
    inline const bool IsEnabled() const { return m_bEnabled; }
    inline const uint32_t& GetSceneIndex() const { return m_uSceneIdx; }

    // Handle graphics
    render::gfx::CPrimitive* const CreatePrimitive(render::EPrimitiveType _eType, render::ERenderMode _eRenderMode);
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
    void DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Size, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color);

  private:
    friend class render::CRender;

    // Draw calls
    void DrawModels(const render::CCamera* _pCamera);
    void DrawPrimitives();
    void DrawDebug();
    void UpdateLightning();

    void Clear();

  private:
    bool m_bEnabled = false;
    uint32_t m_uSceneIdx = 0;

  private:
    // Graphics
    TModels m_lstModels = TModels();
    TPrimitives m_lstPrimitives = TPrimitives();
    TDebugItems m_lstDebugItems = TDebugItems();

    // Lights -> Lighting manager WIP
    render::lights::CDirectionalLight* m_pDirectionalLight = nullptr;
    TPointLightsList m_lstPointLights = TPointLightsList();
    TSpotLightsList m_lstSpotLights = TSpotLightsList();

    // Buffers
    TLightingBuffer m_oLightingBuffer;
    CConstantBuffer<SHandleInstancing> m_oInstancingBuffer;
  };
}
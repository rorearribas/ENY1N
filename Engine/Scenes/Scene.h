#pragma once
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Render/Lights/SpotLight.h"
#include "Engine/Render/Lights/PointLight.h"
#include "Engine/Utils/Plane.h"

#include "Libs/Utils/FixedPool.h"
#include "Libs/Utils/FixedList.h"

namespace render { class CRender; }
namespace render { namespace lights { class CDirectionalLight; } }

namespace scene
{
  class CScene
  {
  private:
    //@Note: if you change this values you will also need to change the array size in hlsl
    static uint32_t constexpr s_uMaxSpotLights = 100u;
    static uint32_t constexpr s_uMaxPointLights = 100u;

    typedef utils::CFixedPool<render::lights::CPointLight, s_uMaxPointLights> TPointLightsList;
    typedef utils::CFixedPool<render::lights::CSpotLight, s_uMaxSpotLights> TSpotLightsList;

  private:
    static uint32_t constexpr s_uMaxModels = 10000u;
    static uint32_t constexpr s_uMaxInstancesPerModel = 128u;

    static uint32_t constexpr s_uMaxPrimitives = 1000u;
    static uint32_t constexpr s_uMaxDebugItems = 5000u;

    typedef utils::CFixedList<render::gfx::CModel, s_uMaxModels> TModels;
    typedef utils::CFixedPool<render::gfx::CPrimitive, s_uMaxPrimitives> TPrimitives;
    typedef utils::CFixedPool<render::gfx::CPrimitive, s_uMaxDebugItems> TDebugItems;

  public:
    CScene(uint32_t _uIndex);
    ~CScene();

    inline void SetEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }
    inline const bool IsEnabled() const { return m_bEnabled; }
    inline const uint32_t& GetSceneIndex() const { return m_uSceneIdx; }

    // Objects
    render::gfx::CPrimitive* const CreatePrimitive(render::EPrimitiveType _eType, render::ERenderMode _eRenderMode);
    render::gfx::CModel* const LoadModel(const char* _sModelPath);

    // Lighting
    render::lights::CDirectionalLight* const CreateDirectionalLight();
    render::lights::CPointLight* const CreatePointLight();
    render::lights::CSpotLight* const CreateSpotLight();

    void DestroyModel(render::gfx::CModel*& pModel_);
    void DestroyPrimitive(render::gfx::CPrimitive*& pPrimitive_);
    void DestroyLight(render::lights::CLight*& pLight_);

    // Debug
    void DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode);
    void DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Size, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color);

  private:
    friend class render::CRender;

    // Draw calls
    void DrawModels();
    void DrawPrimitives();
    void DrawDebug();
    void ApplyLightning();

    void Clear();

  private:
    bool m_bEnabled = false;
    uint32_t m_uSceneIdx = 0;

  private:
    // Graphics
    TModels m_lstModels = TModels();
    TPrimitives m_lstPrimitives = TPrimitives();
    TDebugItems m_lstDebugItems = TDebugItems();

    // Lights
    render::lights::CDirectionalLight* m_pDirectionalLight = nullptr;
    TPointLightsList m_lstPointLights = TPointLightsList();
    TSpotLightsList m_lstSpotLights = TSpotLightsList();

    // Buffers
    CConstantBuffer<SGlobalLightingData<s_uMaxPointLights, s_uMaxSpotLights>> m_oGlobalLightingBuffer;
    CConstantBuffer<SHandleInstancing> m_oHandleInstancingBuffer;
  };
}
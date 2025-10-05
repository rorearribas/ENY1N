#pragma once
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Render/Lights/SpotLight.h"
#include "Engine/Render/Lights/PointLight.h"
#include "Libs/Utils/FixedPool.h"

namespace render { class CRender; }
namespace render { namespace lights { class CDirectionalLight; } }

namespace scene
{
  class CScene
  {
  private:
    //@Note: if you change this values you will also need to change the array size in hlsl
    static int constexpr s_iMaxSpotLights = 100;
    static int constexpr s_iMaxPointLights = 100;

    typedef utils::CFixedPool<render::lights::CPointLight, s_iMaxPointLights> TPointLightsList;
    typedef utils::CFixedPool<render::lights::CSpotLight, s_iMaxSpotLights> TSpotLightsList;

  private:
    static int constexpr s_iMaxModels = 1000;
    static int constexpr s_iMaxDebugItems = 1000;
    static int constexpr s_iMaxPrimitives = 10000;

    typedef utils::CFixedPool<render::gfx::CPrimitive, s_iMaxDebugItems> TDebugItemList;
    typedef utils::CFixedPool<render::gfx::CPrimitive, s_iMaxPrimitives> TPrimitiveList;
    typedef utils::CFixedPool<render::gfx::CModel, s_iMaxPrimitives> TModelList;

  public:
    CScene(uint32_t _uIndex);
    ~CScene();

    const uint32_t& GetSceneIndex() const { return m_uSceneIdx; }
    void SetSceneEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }
    const bool IsEnabled() const { return m_bEnabled; }

    // Debug creation
    void DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode);
    void DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Size, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode);
    void DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color);

    // Element creation
    render::gfx::CPrimitive* const CreatePrimitive(render::gfx::EPrimitiveType _eType, render::ERenderMode _eRenderMode);
    render::gfx::CModel* const CreateModel(const char* _sModelPath);

    render::lights::CDirectionalLight* const CreateDirectionalLight();
    render::lights::CPointLight* const CreatePointLight();
    render::lights::CSpotLight* const CreateSpotLight();

    void DestroyModel(render::gfx::CModel*& pModel_);
    void DestroyPrimitive(render::gfx::CPrimitive*& pPrimitive_);
    void DestroyLight(render::lights::CBaseLight*& pLight_);

  private:
    friend class render::CRender;

    // Draw calls
    void DrawPrimitives();
    void DrawModels();
    void UpdateLighting();

  private:
    void DestroyAllPrimitives();
    void DestroyAllModels();
    void DestroyAllLights();

  private:
    bool m_bEnabled = false;
    uint32_t m_uSceneIdx = 0;

    // Graphics
    TModelList m_vctModels = TModelList();
    TPrimitiveList m_vctPrimitives = TPrimitiveList();
    TDebugItemList m_vctDebugItems = TDebugItemList();

    // Lights
    render::lights::CDirectionalLight* m_pDirectionalLight = nullptr;
    TPointLightsList m_vctPointLights = TPointLightsList();
    TSpotLightsList m_vctSpotLights = TSpotLightsList();

    // Global lightning buffer
    CConstantBuffer<SGlobalLightingData<s_iMaxPointLights, s_iMaxSpotLights>> m_oGlobalLightingBuffer;
  };
}
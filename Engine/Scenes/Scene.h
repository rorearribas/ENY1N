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
    static int constexpr s_iMaxSpotLights = 100;
    static int constexpr s_iMaxPointLights = 100;

    static int constexpr s_iMaxModels = 1000;
    static int constexpr s_iMaxDebugItems = 1000;
    static int constexpr s_iMaxPrimitives = 10000;

    // Lights
    typedef utils::CFixedPool<render::lights::CPointLight, s_iMaxPointLights> TPointLightsList;
    typedef utils::CFixedPool<render::lights::CSpotLight, s_iMaxSpotLights> TSpotLightsList;

    // Graphics
    typedef utils::CFixedPool<render::graphics::CPrimitive, s_iMaxDebugItems> TDebugItemList;
    typedef utils::CFixedPool<render::graphics::CPrimitive, s_iMaxPrimitives> TPrimitiveList;
    typedef utils::CFixedPool<render::graphics::CModel, s_iMaxPrimitives> TModelList;

  public:
    CScene(uint32_t _uIndex);
    ~CScene();

    const uint32_t& GetSceneIndex() const { return m_uSceneIdx; }
    void SetSceneEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }
    const bool& IsEnabled() const { return m_bEnabled; }

    // Debug creation
    void DrawLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest, const math::CVector3& _v3Color);
    void DrawCube(const math::CVector3& _v3Pos, float _fSize, const math::CVector3& _v3Color, render::ERenderMode = render::ERenderMode::WIREFRAME);
    void DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode = render::ERenderMode::WIREFRAME);
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iStacks, int _iSlices, const math::CVector3& _v3Color, render::ERenderMode = render::ERenderMode::WIREFRAME);

    // Element creation
    render::graphics::CPrimitive* const CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType, render::ERenderMode = render::ERenderMode::SOLID);
    render::graphics::CModel* const CreateModel(const char* _sModelPath, const char* _sBaseMltDir);

    render::lights::CDirectionalLight* const CreateDirectionalLight();
    render::lights::CPointLight* const CreatePointLight();
    render::lights::CSpotLight* const CreateSpotLight();

    void DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_);
    void DestroyModel(render::graphics::CModel*& pModel_);
    void DestroyLight(render::lights::CBaseLight*& pLight_);

  private:
    friend class render::CRender;

    // Draw calls
    void DrawPrimitives();
    void DrawModels();
    void UpdateLighting();

  private:
    bool DestroyPointLight(render::lights::CPointLight* pLight_);
    bool DestroySpotLight(render::lights::CSpotLight* pLight_);

    void DestroyAllPrimitives();
    void DestroyAllModels();
    void DestroyAllLights();

  private:
    bool m_bEnabled = false;
    UINT32 m_uSceneIdx = 0;

    // Graphics
    TModelList m_vctModels = TModelList();
    TPrimitiveList m_vctPrimitiveItems = TPrimitiveList();
    TDebugItemList m_vctDebugItems = TDebugItemList();

    // Lights
    render::lights::CDirectionalLight* m_pDirectionalLight = nullptr;
    TPointLightsList m_vctPointLights = TPointLightsList();
    TSpotLightsList m_vctSpotLights = TSpotLightsList();

    // Global lightning buffer
    CConstantBuffer<SGlobalLightingData<s_iMaxPointLights, s_iMaxSpotLights>> m_oLightingBuffer;
  };
}
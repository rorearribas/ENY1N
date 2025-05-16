#pragma once
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/Utils/FixedList.h"
#include <cassert>
#include <array>
#include <algorithm>

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

    static int constexpr s_iMaxModels = 500;
    static int constexpr s_iMaxPrimitives = 500;
    static int constexpr s_iMaxTemporalItems = 500;

    // Lights
    typedef utils::CFixedList<render::lights::CPointLight, s_iMaxPointLights> TPointLightsList;
    typedef utils::CFixedList<render::lights::CSpotLight, s_iMaxSpotLights> TSpotLightsList;

    // Graphics
    typedef utils::CFixedList<render::graphics::CPrimitive, s_iMaxTemporalItems> TTemporalItemList;
    typedef utils::CFixedList<render::graphics::CPrimitive, s_iMaxPrimitives> TPrimitiveList;
    typedef utils::CFixedList<render::graphics::CModel, s_iMaxPrimitives> TModelList;

  public:
    CScene(const UINT32& _uIndex);
    ~CScene();

    const uint32_t& GetSceneIndex() const { return m_uSceneIdx; }
    void SetSceneEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }
    const bool& IsEnabled() const { return m_bEnabled; }

    // Debug creation
    void DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iStacks, int _iSlices, render::ERenderMode = render::ERenderMode::SOLID, bool _bPermanent = false);
    void DrawCube(const math::CVector3& _v3Pos, float _fSize, render::ERenderMode = render::ERenderMode::SOLID, bool _bPermanent = false);
    void DrawLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest, const math::CVector3& _v3Color, bool _bPermanent = false);

    // Element creation
    render::graphics::CPrimitive* const CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType, render::ERenderMode = render::ERenderMode::SOLID);
    render::graphics::CModel* const CreateModel(const char* _sModelPath, const char* _sBaseMltDir);

    render::lights::CDirectionalLight* const CreateDirectionalLight();
    render::lights::CPointLight* const CreatePointLight();
    render::lights::CSpotLight* const CreateSpotLight();

    void DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_);
    void DestroyModel(render::graphics::CModel*& pModel_);
    void DestroyLight(render::lights::CLight*& pLight_);

  private:
    friend class render::CRender;

    bool DestroyPointLight(render::lights::CPointLight* pLight_);
    bool DestroySpotLight(render::lights::CSpotLight* pLight_);

    void DestroyAllPrimitives();
    void DestroyAllModels();

    void DestroyAllLights();

    void DrawPrimitives();
    void DrawModels();
    void ApplyLights();

  private:
    bool m_bEnabled = false;
    UINT32 m_uSceneIdx = 0;

    // Graphics
    TModelList m_vctModels = TModelList();
    TPrimitiveList m_vctPrimitiveItems = TPrimitiveList();
    TTemporalItemList m_vctTemporalItems = TTemporalItemList();

    // Lights
    render::lights::CDirectionalLight* m_pDirectionalLight = nullptr;
    TPointLightsList m_vctPointLights = TPointLightsList();
    TSpotLightsList m_vctSpotLights = TSpotLightsList();

    // Global lightning buffer
    CConstantBuffer<SGlobalLightningData<s_iMaxPointLights, s_iMaxSpotLights>> m_oLightningBuffer;
  };
}
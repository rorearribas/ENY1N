#include "LightManager.h"
#include "Engine/Render/Lighting/DirectionalLight.h"
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace lights
  {
    // ------------------------------------
    CLightManager::CLightManager()
    {
      HRESULT hResult = Setup();
      UNUSED_VAR(hResult);
#ifdef _DEBUG
      assert(!FAILED(hResult));
#endif // DEBUG
    }
    // ------------------------------------
    CLightManager::~CLightManager()
    {
      Clean();
    }
    // ------------------------------------
    HRESULT CLightManager::Setup()
    {
      // Init constant buffer
      return m_oLightingBuffer.Init();
    }
    // ------------------------------------
    void CLightManager::ComputeShadows()
    {
      if (m_lstShadowMaps.IsEmpty())
      {
        return;
      }
    }
    // ------------------------------------
    void CLightManager::ApplyLighting()
    {
      // Get data
      TLightingData rLightingData = TLightingData();

      // Update directional light
      if (m_pDirectionalLight)
      {
        rLightingData.DirectionalLight.Dir = m_pDirectionalLight->GetDir();
        rLightingData.DirectionalLight.Color = m_pDirectionalLight->GetColor();
        rLightingData.DirectionalLight.Intensity = m_pDirectionalLight->GetIntensity();
        rLightingData.DirectionalLight.CastShadows = m_pDirectionalLight->CastShadows();
      }

      // Update point lights
      for (uint32_t uIndex = 0; uIndex < m_lstPointLights.GetSize(); uIndex++)
      {
        // Get point light
        utils::CWeakPtr<render::lights::CPointLight> wpPointLight = m_lstPointLights[uIndex];

        // Set data
        rLightingData.PointLights[uIndex].Pos = wpPointLight->GetPos();
        rLightingData.PointLights[uIndex].Range = wpPointLight->GetRange();
        rLightingData.PointLights[uIndex].Color = wpPointLight->GetColor();
        rLightingData.PointLights[uIndex].Intensity = wpPointLight->GetIntensity();
      }
      // Set the number of registered point lights
      rLightingData.RegisteredPointLights = static_cast<int>(m_lstPointLights.GetSize());

      // Update spot lights
      for (uint32_t uIndex = 0; uIndex < m_lstSpotLights.GetSize(); uIndex++)
      {
        // Get spotlight
        utils::CWeakPtr<render::lights::CSpotLight> wpSpotLight = m_lstSpotLights[uIndex];

        // Set data
        rLightingData.SpotLights[uIndex].Pos = wpSpotLight->GetPos();
        rLightingData.SpotLights[uIndex].Dir = wpSpotLight->GetDir();
        rLightingData.SpotLights[uIndex].Range = wpSpotLight->GetRange();
        rLightingData.SpotLights[uIndex].Color = wpSpotLight->GetColor();
        rLightingData.SpotLights[uIndex].Intensity = wpSpotLight->GetIntensity();
      }
      // Set the number of registered spot lights
      rLightingData.RegisteredSpotLights = static_cast<int>(m_lstSpotLights.GetSize());

      // Write buffer
      bool bOk = m_oLightingBuffer.WriteBuffer(rLightingData);
      UNUSED_VAR(bOk);
      assert(bOk);

      // Bind buffer
      const uint32_t uSlot(1);
      m_oLightingBuffer.Bind<render::EShader::E_PIXEL>(uSlot);
    }
    // ------------------------------------
    utils::CWeakPtr<render::lights::CDirectionalLight> CLightManager::GetDirectionalLight()
    {
      return m_pDirectionalLight.GetRef();
    }
    // ------------------------------------
    utils::CWeakPtr<render::lights::CDirectionalLight> const CLightManager::CreateDirectionalLight()
    {
      if (m_pDirectionalLight)
      {
        WARNING_LOG("There is a directional light!");
        return m_pDirectionalLight.GetRef();
      }

      // Create directional light
      m_pDirectionalLight.Make_Unique();
      m_pDirectionalLight->SetCastShadows(true);

      // Create shadow map - testing
      utils::CWeakPtr<render::gfx::CShadowMap> pShadowMap = m_lstShadowMaps.Create();
      pShadowMap->Setup(2048u, 2048u);

      return m_pDirectionalLight.GetRef();
    }
    // ------------------------------------
    utils::CWeakPtr<render::lights::CPointLight> const CLightManager::CreatePointLight()
    {
      if (m_lstPointLights.GetSize() >= s_uMaxPointLights)
      {
        WARNING_LOG("You have reached maximum point lights in the current scene!");
        return utils::CWeakPtr<render::lights::CPointLight>();
      }
      return m_lstPointLights.Create();
    }
    // ------------------------------------
    utils::CWeakPtr<render::lights::CSpotLight> const CLightManager::CreateSpotLight()
    {
      if (m_lstSpotLights.GetSize() >= s_uMaxSpotLights)
      {
        WARNING_LOG("You have reached maximum spot lights in the current scene!");
        return utils::CWeakPtr<render::lights::CSpotLight>();
      }
      return m_lstSpotLights.Create();
    }
    // ------------------------------------
    bool CLightManager::DestroyLight(utils::CWeakPtr<render::lights::CLight> _wpLight)
    {
      if (!_wpLight.IsValid())
      {
        return false;
      }

      bool bOk = false;
      switch (_wpLight->GetLightType())
      {
        case render::ELight::DIRECTIONAL_LIGHT:
        {
          m_pDirectionalLight.Release();
          bOk = true;
        }
        break;
        case render::ELight::POINT_LIGHT:
        {
          auto* pPointLight = static_cast<render::lights::CPointLight*>(_wpLight.GetPtr());
          bOk = m_lstPointLights.Remove(pPointLight);
        }
        break;
        case render::ELight::SPOT_LIGHT:
        {
          auto* pPointLight = static_cast<render::lights::CSpotLight*>(_wpLight.GetPtr());
          bOk = m_lstSpotLights.Remove(pPointLight);
        }
        break;
        default: break;
      }
      return bOk;
    }
    // ------------------------------------
    void CLightManager::Clean()
    {
      // Destroy lights
      m_pDirectionalLight.Release();
      m_lstPointLights.Clear();
      m_lstSpotLights.Clear();
    }
  }
}


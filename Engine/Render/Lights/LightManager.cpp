#include "LightManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Engine/Render/Lights/PointLight.h"
#include "Engine/Render/Lights/DirectionalLight.h"
#include "Engine/Render/Lights/SpotLight.h"
#include <algorithm>
#include <iostream>
#include <cassert>

namespace render
{
  namespace lights
  {
    // ------------------------------------
    CLightManager::CLightManager()
    {
      HRESULT hResult = m_oLightingBuffer.Init();
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
    void CLightManager::Apply()
    {
      // Get data
      auto& rLightingBuffer = m_oLightingBuffer.GetData();

      // Update directional light
      if (m_pDirectionalLight)
      {
        rLightingBuffer.DirectionalLight.Intensity = m_pDirectionalLight->GetIntensity();
        rLightingBuffer.DirectionalLight.Color = m_pDirectionalLight->GetColor();
        rLightingBuffer.DirectionalLight.Direction = m_pDirectionalLight->GetDir();
      }

      // Update point lights
      for (uint32_t uIndex = 0; uIndex < m_lstPointLights.GetMaxSize(); uIndex++)
      {
        if (render::lights::CPointLight* pPointLight = m_lstPointLights[uIndex])
        {
          rLightingBuffer.PointLights[uIndex].Position = pPointLight->GetPos();
          rLightingBuffer.PointLights[uIndex].Color = pPointLight->GetColor();
          rLightingBuffer.PointLights[uIndex].Intensity = pPointLight->GetIntensity();
          rLightingBuffer.PointLights[uIndex].Range = pPointLight->GetRange();
        }
      }
      // Set the number of registered point lights
      rLightingBuffer.RegisteredPointLights = static_cast<int>(m_lstPointLights.GetSize());

      // Update spot lights
      for (uint32_t uIndex = 0; uIndex < m_lstSpotLights.GetMaxSize(); uIndex++)
      {
        if (render::lights::CSpotLight* pSpotLight = m_lstSpotLights[uIndex])
        {
          rLightingBuffer.SpotLights[uIndex].Position = pSpotLight->GetPos();
          rLightingBuffer.SpotLights[uIndex].Direction = pSpotLight->GetDir();
          rLightingBuffer.SpotLights[uIndex].Color = pSpotLight->GetColor();
          rLightingBuffer.SpotLights[uIndex].Range = pSpotLight->GetRange();
          rLightingBuffer.SpotLights[uIndex].Intensity = pSpotLight->GetIntensity();
        }
      }
      // Set the number of registered spot lights
      rLightingBuffer.RegisteredSpotLights = static_cast<int>(m_lstSpotLights.GetSize());

      // Write buffer
      bool bOk = m_oLightingBuffer.WriteBuffer();
      UNUSED_VAR(bOk);
      assert(bOk);

      // Bind buffer
      m_oLightingBuffer.SetSlot(1);
      m_oLightingBuffer.Bind<render::EShader::E_PIXEL>();
    }
    // ------------------------------------
    render::lights::CDirectionalLight* const CLightManager::CreateDirectionalLight()
    {
      if (m_pDirectionalLight)
      {
        WARNING_LOG("There is a directional light in the current scene!");
        return m_pDirectionalLight;
      }
      m_pDirectionalLight = new render::lights::CDirectionalLight();
      return m_pDirectionalLight;
    }
    // ------------------------------------
    render::lights::CPointLight* const CLightManager::CreatePointLight()
    {
      if (m_lstPointLights.GetSize() >= s_uMaxPointLights)
      {
        WARNING_LOG("You have reached maximum point lights in the current scene!");
        return nullptr;
      }
      return m_lstPointLights.Create();
    }
    // ------------------------------------
    render::lights::CSpotLight* const CLightManager::CreateSpotLight()
    {
      if (m_lstSpotLights.GetSize() >= s_uMaxSpotLights)
      {
        WARNING_LOG("You have reached maximum spot lights in the current scene!");
        return nullptr;
      }
      return m_lstSpotLights.Create();
    }
    // ------------------------------------
    bool CLightManager::DestroyLight(render::lights::CLight*& _pLight_)
    {
      bool bOk = false;
      switch (_pLight_->GetLightType())
      {
      case render::ELight::DIRECTIONAL_LIGHT:
      {
        bOk = global::ReleaseObject(m_pDirectionalLight);
      }
      break;
      case render::ELight::POINT_LIGHT:
      {
        render::lights::CPointLight* pPointLight = static_cast<render::lights::CPointLight*>(_pLight_);
        bOk = m_lstPointLights.Remove(pPointLight);
      }
      break;
      case render::ELight::SPOT_LIGHT:
      {
        render::lights::CSpotLight* pSpotLight = static_cast<render::lights::CSpotLight*>(_pLight_);
        bOk = m_lstSpotLights.Remove(pSpotLight);
      }
      break;
      default:
        break;
      }
      _pLight_ = nullptr; // Set as nullptr
      return bOk;
    }
    // ------------------------------------
    void CLightManager::Clean()
    {
      // Destroy lights
      global::ReleaseObject(m_pDirectionalLight);
      m_lstPointLights.Clear();
      m_lstSpotLights.Clear();
    }
  }
}


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
    CLightsManager::CLightsManager()
    {
      HRESULT hResult = m_oLightningBuffer.Init(global::dx::s_pDevice, global::dx::s_pDeviceContext);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));
    }
    // ------------------------------------
    CLightsManager::~CLightsManager()
    {
      Clean();
    }
    // ------------------------------------
    void CLightsManager::Update()
    {
      // Get data
      auto& oGlobalLightningData = m_oLightningBuffer.GetData();

      // Update directional light
      if (m_pDirectionalLight)
      {
        oGlobalLightningData.DirectionalLight.Intensity = m_pDirectionalLight->GetIntensity();
        oGlobalLightningData.DirectionalLight.Color = m_pDirectionalLight->GetColor();
        oGlobalLightningData.DirectionalLight.Direction = m_pDirectionalLight->GetDir();
      }

      // Update point lights
      for (uint32_t uIndex = 0; uIndex < m_uRegisteredPointLights; uIndex++)
      {
        render::lights::CPointLight* pPointLight = m_lstPointLights[uIndex];
        oGlobalLightningData.PointLights[uIndex].Position = pPointLight->GetPosition();
        oGlobalLightningData.PointLights[uIndex].Color = pPointLight->GetColor();
        oGlobalLightningData.PointLights[uIndex].Intensity = pPointLight->GetIntensity();
        oGlobalLightningData.PointLights[uIndex].Range = pPointLight->GetRange();
      }
      // Set the number of registered point lights
      oGlobalLightningData.RegisteredPointLights = static_cast<int>(m_uRegisteredPointLights);

      // Update spot lights
      for (uint32_t uIndex = 0; uIndex < m_uRegisteredSpotLights; uIndex++)
      {
        render::lights::CSpotLight* pSpotLight = m_lstSpotLights[uIndex];
        oGlobalLightningData.SpotLights[uIndex].Position = pSpotLight->GetPosition();
        oGlobalLightningData.SpotLights[uIndex].Direction = pSpotLight->GetDir();
        oGlobalLightningData.SpotLights[uIndex].Color = pSpotLight->GetColor();
        oGlobalLightningData.SpotLights[uIndex].Range = pSpotLight->GetRange();
        oGlobalLightningData.SpotLights[uIndex].Intensity = pSpotLight->GetIntensity();
      }
      // Set the number of registered spot lights
      oGlobalLightningData.RegisteredSpotLights = static_cast<int>(m_uRegisteredSpotLights);

      // Write buffer
      bool bOk = m_oLightningBuffer.WriteBuffer();
      UNUSED_VAR(bOk);
      assert(bOk);

      // Push buffer
      ID3D11Buffer* pConstantBuffer = m_oLightningBuffer.GetBuffer();
      global::dx::s_pDeviceContext->PSSetConstantBuffers(1, 1, &pConstantBuffer);
    }
    // ------------------------------------
    void CLightsManager::DestroyLight(render::lights::CLight*& _pLight_)
    {
      switch (_pLight_->GetLightType())
      {
      case render::ELightType::DIRECTIONAL_LIGHT:
      {
        global::ReleaseObject(m_pDirectionalLight);
        _pLight_ = nullptr;
      }
      break;
      case render::ELightType::POINT_LIGHT:
      {
        DestroyPointLight(_pLight_);
      }
      break;
      case render::ELightType::SPOT_LIGHT:
      {
        DestroySpotLight(_pLight_);
      }
      break;
      default:
        break;
      }
    }
    // ------------------------------------
    render::lights::CDirectionalLight* CLightsManager::CreateDirectionalLight()
    {
      if (m_pDirectionalLight)
      {
        WARNING_LOG("There is a directional light in the current scene!" );
        return m_pDirectionalLight;
      }
      m_pDirectionalLight = new render::lights::CDirectionalLight();
      return m_pDirectionalLight;
    }
    // ------------------------------------
    render::lights::CPointLight* CLightsManager::CreatePointLight()
    {
      if (m_uRegisteredPointLights >= s_uMaxSpotLights)
      {
        WARNING_LOG("You have reached maximum point lights in the current scene");
        return nullptr;
      }
      render::lights::CPointLight*& pPointLight = m_lstPointLights[m_uRegisteredPointLights++];
      pPointLight = new render::lights::CPointLight();
      return pPointLight;
    }
    // ------------------------------------
    render::lights::CSpotLight* CLightsManager::CreateSpotLight()
    {
      if (m_uRegisteredSpotLights >= s_uMaxSpotLights)
      {
        WARNING_LOG("You have reached maximum spot lights in the current scene");
        return nullptr;
      }
      render::lights::CSpotLight*& pSpotLight = m_lstSpotLights[m_uRegisteredSpotLights++];
      pSpotLight = new render::lights::CSpotLight();
      return pSpotLight;
    }
    // ------------------------------------
    void CLightsManager::Clean()
    {
      // Destroy directional light
      global::ReleaseObject(m_pDirectionalLight);

      // Destroy point lights
      std::for_each(m_lstPointLights.begin(), m_lstPointLights.end(), [](render::lights::CPointLight*& _pLight)
      {
        global::ReleaseObject(_pLight);
      });
      m_uRegisteredPointLights = 0;

      // Destroy spot lights
      std::for_each(m_lstSpotLights.begin(), m_lstSpotLights.end(), [](render::lights::CSpotLight*& _pLight)
      {
        global::ReleaseObject(_pLight);
      });
      m_uRegisteredSpotLights = 0;
    }
    // ------------------------------------
    void CLightsManager::DestroyPointLight(render::lights::CLight*& pLight_)
    {
      auto it = std::find(m_lstPointLights.begin(), m_lstPointLights.end(), pLight_);
      if (it != m_lstPointLights.end())
      {
        global::ReleaseObject(*it);
        m_uRegisteredPointLights--;

        auto oReorderFunc = std::remove_if(m_lstPointLights.begin(), m_lstPointLights.end(),
        [](render::lights::CLight* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
        std::fill(oReorderFunc, m_lstPointLights.end(), nullptr); // Set nullptr
      }
      pLight_ = nullptr;
    }
    // ------------------------------------
    void CLightsManager::DestroySpotLight(render::lights::CLight*& pLight_)
    {
      auto it = std::find(m_lstSpotLights.begin(), m_lstSpotLights.end(), pLight_);
      if (it != m_lstSpotLights.end())
      {
        global::ReleaseObject(*it);
        m_uRegisteredSpotLights--;

        auto oReorderFunc = std::remove_if(m_lstSpotLights.begin(), m_lstSpotLights.end(),
        [](render::lights::CLight* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
        std::fill(oReorderFunc, m_lstSpotLights.end(), nullptr); // Set nullptr
      }
      pLight_ = nullptr;
    }
  }
}


#include "LightManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Engine/Render/Lights/PointLight.h"
#include "Engine/Render/Lights/DirectionalLight.h"
#include "Engine/Render/Lights/SpotLight.h"
#include <algorithm>
#include <iostream>

namespace render
{
	namespace lights
	{
    // ------------------------------------
    CLightManager::CLightManager()
    {
      HRESULT hResult = m_oLightningBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);
      UNUSED_VARIABLE(hResult);
      assert(!FAILED(hResult));
    }
    // ------------------------------------
    CLightManager::~CLightManager()
    {
      Clean();
    }
    // ------------------------------------
    void CLightManager::Update()
    {
      // Get data
      auto& oGlobalLightningData = m_oLightningBuffer.GetData();

      // Update directional light
      if (m_pDirectionalLight)
      {
        oGlobalLightningData.DirectionalLight.Intensity = m_pDirectionalLight->GetIntensity();
        oGlobalLightningData.DirectionalLight.Color = m_pDirectionalLight->GetColor();
        oGlobalLightningData.DirectionalLight.Direction = m_pDirectionalLight->GetDirection();
      }

      // Update point lights
      for (uint32_t uIndex = 0; uIndex < m_uRegisteredPointLights; uIndex++)
      {
        render::lights::CPointLight* pPointLight = m_vctPointLights[uIndex];
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
        render::lights::CSpotLight* pSpotLight = m_vctSpotLights[uIndex];
        oGlobalLightningData.SpotLights[uIndex].Position = pSpotLight->GetPosition();
        oGlobalLightningData.SpotLights[uIndex].Direction = pSpotLight->GetDirection();
        oGlobalLightningData.SpotLights[uIndex].Color = pSpotLight->GetColor();
        oGlobalLightningData.SpotLights[uIndex].Range = pSpotLight->GetRange();
        oGlobalLightningData.SpotLights[uIndex].CutOffAngle = pSpotLight->GetCutOffAngle();
        oGlobalLightningData.SpotLights[uIndex].Intensity = pSpotLight->GetIntensity();
      }
      // Set the number of registered spot lights
      oGlobalLightningData.RegisteredSpotLights = static_cast<int>(m_uRegisteredSpotLights);

      // Write buffer
      bool bOk = m_oLightningBuffer.WriteBuffer();
      UNUSED_VARIABLE(bOk);
      assert(bOk);

      // Push buffer
      ID3D11Buffer* pConstantBuffer = m_oLightningBuffer.GetBuffer();
      global::dx11::s_pDeviceContext->PSSetConstantBuffers(1, 1, &pConstantBuffer);
    }
    // ------------------------------------
    void CLightManager::DestroyLight(render::lights::CLight*& pLight_)
    {
      switch (pLight_->GetLightType())
      {
      case render::lights::ELightType::DIRECTIONAL_LIGHT:
      {
        if (m_pDirectionalLight)
        {
          delete m_pDirectionalLight;
          m_pDirectionalLight = nullptr;
          pLight_ = nullptr;
        }
      }
      break;
      case render::lights::ELightType::POINT_LIGHT:
      {
        DestroyPointLight(pLight_);
      }
      break;
      case render::lights::ELightType::SPOT_LIGHT:
      {
        DestroySpotLight(pLight_);
      }
      break;
      default:
        break;
      }
    }
    // ------------------------------------
    render::lights::CDirectionalLight* CLightManager::CreateDirectionalLight()
    {
      if (m_pDirectionalLight)
      {
        std::cout << "There is a directional light in the current scene" << std::endl;
        return m_pDirectionalLight;
      }
      m_pDirectionalLight = new render::lights::CDirectionalLight();
      return m_pDirectionalLight;
    }
    // ------------------------------------
    render::lights::CPointLight* CLightManager::CreatePointLight()
    {
      if (m_uRegisteredPointLights >= s_iMaxSpotLights)
      {
        std::cout << "You have reached maximum point lights in the current scene" << std::endl;
        return nullptr;
      }
      render::lights::CPointLight*& pPointLight = m_vctPointLights[m_uRegisteredPointLights++];
      pPointLight = new render::lights::CPointLight();
      return pPointLight;
    }
    // ------------------------------------
    render::lights::CSpotLight* CLightManager::CreateSpotLight()
    {
      if (m_uRegisteredSpotLights >= s_iMaxSpotLights)
      {
        std::cout << "You have reached maximum spot lights in the current scene" << std::endl;
        return nullptr;
      }
      render::lights::CSpotLight*& pSpotLight = m_vctSpotLights[m_uRegisteredSpotLights++];
      pSpotLight = new render::lights::CSpotLight();
      return pSpotLight;
    }
    // ------------------------------------
    void CLightManager::Clean()
    {
      // Destroy directional light
      if (m_pDirectionalLight)
      {
        delete m_pDirectionalLight;
        m_pDirectionalLight = nullptr;
      }
      // Destroy point lights
      std::for_each(m_vctPointLights.begin(), m_vctPointLights.end(), [](render::lights::CPointLight*& _pLight)
      {
        if (_pLight)
        {
          delete _pLight;
          _pLight = nullptr;
        }
      });
      m_uRegisteredPointLights = 0;
      // Destroy spot lights
      std::for_each(m_vctSpotLights.begin(), m_vctSpotLights.end(), [](render::lights::CSpotLight*& _pLight)
      {
        if (_pLight)
        {
          delete _pLight;
          _pLight = nullptr;
        }
      });
      m_uRegisteredSpotLights = 0;
    }
    // ------------------------------------
    void CLightManager::DestroyPointLight(render::lights::CLight*& pLight_)
    {
      auto it = std::find(m_vctPointLights.begin(), m_vctPointLights.end(), pLight_);
      if (it != m_vctPointLights.end())
      {
        delete* it;
        *it = nullptr;
        m_uRegisteredPointLights--;

        auto oReorderFunc = std::remove_if(m_vctPointLights.begin(), m_vctPointLights.end(),
        [](render::lights::CLight* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
        std::fill(oReorderFunc, m_vctPointLights.end(), nullptr); // Set nullptr
      }
      pLight_ = nullptr;
    }
    // ------------------------------------
    void CLightManager::DestroySpotLight(render::lights::CLight*& pLight_)
    {
      auto it = std::find(m_vctSpotLights.begin(), m_vctSpotLights.end(), pLight_);
      if (it != m_vctSpotLights.end())
      {
        delete* it;
        *it = nullptr;
        m_uRegisteredSpotLights--;

        auto oReorderFunc = std::remove_if(m_vctSpotLights.begin(), m_vctSpotLights.end(),
        [](render::lights::CLight* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
        std::fill(oReorderFunc, m_vctSpotLights.end(), nullptr); // Set nullptr
      }
      pLight_ = nullptr;
    }
  }
}


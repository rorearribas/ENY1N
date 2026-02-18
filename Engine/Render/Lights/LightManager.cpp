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
    void CLightManager::ApplyLighting()
    {
      // Get data
      TLightingData rLightingData = TLightingData();

      // Update directional light
      if (m_pDirectionalLight)
      {
        rLightingData.DirectionalLight.Intensity = m_pDirectionalLight->GetIntensity();
        rLightingData.DirectionalLight.Color = m_pDirectionalLight->GetColor();
        rLightingData.DirectionalLight.Direction = m_pDirectionalLight->GetDir();
      }

      // Update point lights
      for (uint32_t uIndex = 0; uIndex < m_lstPointLights.GetMaxSize(); uIndex++)
      {
        if (render::lights::CPointLight* pPointLight = m_lstPointLights[uIndex])
        {
          rLightingData.PointLights[uIndex].Position = pPointLight->GetPos();
          rLightingData.PointLights[uIndex].Color = pPointLight->GetColor();
          rLightingData.PointLights[uIndex].Intensity = pPointLight->GetIntensity();
          rLightingData.PointLights[uIndex].Range = pPointLight->GetRange();
        }
      }
      // Set the number of registered point lights
      rLightingData.RegisteredPointLights = static_cast<int>(m_lstPointLights.GetSize());

      // Update spot lights
      for (uint32_t uIndex = 0; uIndex < m_lstSpotLights.GetMaxSize(); uIndex++)
      {
        if (render::lights::CSpotLight* pSpotLight = m_lstSpotLights[uIndex])
        {
          rLightingData.SpotLights[uIndex].Position = pSpotLight->GetPos();
          rLightingData.SpotLights[uIndex].Direction = pSpotLight->GetDir();
          rLightingData.SpotLights[uIndex].Color = pSpotLight->GetColor();
          rLightingData.SpotLights[uIndex].Range = pSpotLight->GetRange();
          rLightingData.SpotLights[uIndex].Intensity = pSpotLight->GetIntensity();
        }
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
    void CLightManager::ComputeShadows()
    {
      if (m_lstShadowMaps.GetSize() == 0)
      {
        return;
      }
    }
    // ------------------------------------
    render::lights::CDirectionalLight* const CLightManager::CreateDirectionalLight()
    {
      if (m_pDirectionalLight)
      {
        WARNING_LOG("There is a directional light in the current scene!");
        return m_pDirectionalLight;
      }

      // Create directional light
      m_pDirectionalLight = new render::lights::CDirectionalLight();
      m_pDirectionalLight->SetCastShadows(true);

      // Create depth stencil texture
      TShadowMap& rShadowMap = *m_lstShadowMaps.Create();

      D3D11_TEXTURE2D_DESC rTextureDesc = D3D11_TEXTURE2D_DESC();
      rTextureDesc.Width = 1024;
      rTextureDesc.Height = 1024;
      rTextureDesc.MipLevels = 1;
      rTextureDesc.ArraySize = 1;
      rTextureDesc.SampleDesc.Count = 1;
      rTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // Format
      rTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; // Depth stencil

      texture::TDepthStencil& rShadowDepth = rShadowMap.ShadowDepth;
      HRESULT hResult = rShadowDepth.CreateTexture(rTextureDesc);
      if (FAILED(hResult))
      {
        ERROR_LOG("Error creating depth stencil texture!");
      }

      // Set depth stencil view desc
      D3D11_DEPTH_STENCIL_VIEW_DESC rDepthStencilViewDesc = D3D11_DEPTH_STENCIL_VIEW_DESC();
      rDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
      rDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

      // Create the depth stencil view
      hResult = rShadowDepth.CreateView(rDepthStencilViewDesc);
      if (FAILED(hResult))
      {
        ERROR_LOG("Error creating stencil view!");
      }

      // Creating view from texture
      D3D11_SHADER_RESOURCE_VIEW_DESC rSRVDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
      rSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
      rSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      rSRVDesc.Texture2D.MipLevels = 1;
      texture::TShaderResource& rShadowTexture = rShadowMap.ShadowTexture;
      rShadowTexture.CreateViewFromTexture(rShadowDepth, rSRVDesc);

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


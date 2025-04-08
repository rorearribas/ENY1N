#include "Scene.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Base/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/Lights/Light.h"
#include "Engine/Render/Lights/DirectionalLight.h"
#include "Engine/Render/Lights/PointLight.h"
#include "Engine/Render/Lights/SpotLight.h"
#include <algorithm>
#include <random>
#include <cassert>
#include <iostream>

namespace scene
{
  CScene::CScene(const UINT32& _uIndex) : m_uSceneIdx(_uIndex)
  {
    HRESULT hResult = m_oLightningBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);
    UNUSED_VARIABLE(hResult);
    assert(!FAILED(hResult));
  }
  // ------------------------------------
  CScene::~CScene()
  {
    DestroyAllPrimitives();
    DestroyAllModels();
    DestroyAllLights();
    m_oLightningBuffer.CleanBuffer();
  }
  // ------------------------------------
  void CScene::DrawPrimitives()
  {
    for (uint32_t uIndex = 0; uIndex < m_vctPrimitiveItems.CurrentSize(); uIndex++)
    {
      render::graphics::CPrimitive* pPrimitiveItem = m_vctPrimitiveItems[uIndex];
      pPrimitiveItem->DrawPrimitive();
    }
  }
  // ------------------------------------
  void CScene::DrawModels()
  {
    for (uint32_t uIndex = 0; uIndex < m_vctModels.CurrentSize(); uIndex++)
    {
      render::graphics::CModel* pModel = m_vctModels[uIndex];
      pModel->DrawModel();
    }
  }
  // ------------------------------------
  void CScene::UpdateLights()
  {
    // Fill data
    auto& oGlobalLightningData = m_oLightningBuffer.GetData();

    // Directional light
    if (m_pDirectionalLight)
    {
      oGlobalLightningData.DirectionalLight.Intensity = m_pDirectionalLight->GetIntensity();
      oGlobalLightningData.DirectionalLight.Color = m_pDirectionalLight->GetColor();
      oGlobalLightningData.DirectionalLight.Direction = m_pDirectionalLight->GetDirection();
    }

    // Point lights
    for (uint32_t uIndex = 0; uIndex < m_vctPointLights.CurrentSize(); uIndex++)
    {
      render::lights::CPointLight* pPointLight = m_vctPointLights[uIndex];
      oGlobalLightningData.PointLights[uIndex].Position = pPointLight->GetPosition();
      oGlobalLightningData.PointLights[uIndex].Color = pPointLight->GetColor();
      oGlobalLightningData.PointLights[uIndex].Intensity = pPointLight->GetIntensity();
      oGlobalLightningData.PointLights[uIndex].Range = pPointLight->GetRange();
    }
    // Set the number of registered point lights
    oGlobalLightningData.RegisteredPointLights = static_cast<int>(m_vctPointLights.CurrentSize());

    // Spot lights
    for (uint32_t uIndex = 0; uIndex < m_vctSpotLights.CurrentSize(); uIndex++)
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
    oGlobalLightningData.RegisteredSpotLights = static_cast<int>(m_vctSpotLights.CurrentSize());

    // Update buffer
    m_oLightningBuffer.UpdateBuffer();

    // Apply constant buffer
    ID3D11Buffer* pConstantBuffer = m_oLightningBuffer.GetBuffer();
    global::dx11::s_pDeviceContext->PSSetConstantBuffers(1, 1, &pConstantBuffer);
  }
  // ------------------------------------
  void CScene::DestroyAllPrimitives()
  {
    m_vctPrimitiveItems.ClearAll();
  }
  // ------------------------------------
  void CScene::DestroyAllModels()
  {
    m_vctModels.ClearAll();
  }
  // ------------------------------------
  void CScene::DestroyAllLights()
  {
    // Destroy all lights
    if (m_pDirectionalLight) { delete m_pDirectionalLight; m_pDirectionalLight = nullptr; }
    m_vctPointLights.ClearAll();
    m_vctSpotLights.ClearAll();
  }
  // ------------------------------------
  render::graphics::CPrimitive* const CScene::CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType, render::ERenderMode _eRenderMode)
  {
    if (m_vctPrimitiveItems.CurrentSize() >= m_vctPrimitiveItems.GetMaxSize())
    {
      std::cout << "You have reached maximum primitives in the current scene" << std::endl;
      return nullptr;
    }
    return m_vctPrimitiveItems.CreateItem(_ePrimitiveType, _eRenderMode);
  }
  // ------------------------------------
  render::graphics::CModel* const CScene::CreateModel(const char* _sModelPath, const char* _sBaseMltDir)
  {
    if (m_vctModels.CurrentSize() >= m_vctModels.GetMaxSize())
    {
      std::cout << "You have reached maximum models in the current scene" << std::endl;
      return nullptr;
    }
    return m_vctModels.CreateItem(_sModelPath, _sBaseMltDir);
  }
  // ------------------------------------
  render::lights::CDirectionalLight* const CScene::CreateDirectionalLight()
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
  render::lights::CPointLight* const CScene::CreatePointLight()
  {
    if (m_vctPointLights.CurrentSize() >= s_iMaxPointLights)
    {
      std::cout << "You have reached maximum point lights in the current scene" << std::endl;
      return nullptr;
    }
    return m_vctPointLights.CreateItem();
  }
  // ------------------------------------
  render::lights::CSpotLight* const CScene::CreateSpotLight()
  {
    if (m_vctSpotLights.CurrentSize() >= s_iMaxSpotLights)
    {
      std::cout << "You have reached maximum spot lights in the current scene" << std::endl;
      return nullptr;
    }
    return m_vctSpotLights.CreateItem();
  }
  // ------------------------------------
  void CScene::DestroyPrimitive(render::graphics::CPrimitive*& _pPrimitive_)
  {
    bool bOk = m_vctPrimitiveItems.RemoveItem(_pPrimitive_);
    UNUSED_VARIABLE(bOk);
    assert(bOk); // Sanity check
    _pPrimitive_ = nullptr; // Set as nullptr
  }
  // ------------------------------------
  void CScene::DestroyModel(render::graphics::CModel*& _pModel_)
  {
    bool bOk = m_vctModels.RemoveItem(_pModel_);
    UNUSED_VARIABLE(bOk);
    assert(bOk); // Sanity check
    _pModel_ = nullptr; // Set as nullptr
  }
  // ------------------------------------
  void CScene::DestroyLight(render::lights::CLight*& _pLight_)
  {
    bool bOk = false;
    switch (_pLight_->GetLightType())
    {
    case render::lights::ELightType::DIRECTIONAL_LIGHT:
    {
      if (m_pDirectionalLight)
      {
        delete m_pDirectionalLight;
        m_pDirectionalLight = nullptr;
        bOk = true;
      }
    }
    break;
    case render::lights::ELightType::POINT_LIGHT:
    {
      bOk = DestroyPointLight(static_cast<render::lights::CPointLight*>(_pLight_));
    }
    break;
    case render::lights::ELightType::SPOT_LIGHT:
    {
      bOk = DestroySpotLight(static_cast<render::lights::CSpotLight*>(_pLight_));
    }
    break;
    default:
      break;
    }

    assert(bOk); // Sanity check
    _pLight_ = nullptr; // Assign to nullptr
  }
  // ------------------------------------
  bool CScene::DestroyPointLight(render::lights::CPointLight* _pPointLight_)
  {
    return m_vctPointLights.RemoveItem(_pPointLight_);
  }
  // ------------------------------------
  bool CScene::DestroySpotLight(render::lights::CSpotLight* _pLight_)
  {
    return m_vctSpotLights.RemoveItem(_pLight_);
  }
}
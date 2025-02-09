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
    HRESULT hr = m_oLightningBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);
    UNUSED_VARIABLE(hr);
    assert(!FAILED(hr));
  }
  // ------------------------------------
  CScene::~CScene()
  {
    DestroyAllPrimitives();
    DestroyAllModels();
    DestroyAllLights();
  }
  // ------------------------------------
  void CScene::DrawPrimitives()
  {
    for (uint32_t uIndex = 0; uIndex < m_uRegisteredPrimitives; uIndex++)
    {
      render::graphics::CPrimitive* pPrimitiveItem = m_vctPrimitiveItems[uIndex];
      pPrimitiveItem->DrawPrimitive();
    }
  }
  // ------------------------------------
  void CScene::DrawModels()
  {
    for (uint32_t uIndex = 0; uIndex < m_uRegisteredModels; uIndex++)
    {
      render::graphics::CModel* pModel = m_vctModels[uIndex];
      pModel->DrawModel();
    }
  }
  // ------------------------------------
  void CScene::UpdateLights()
  {
    auto& oGlobalLightningData = m_oLightningBuffer.GetData();

    // Directional light
    if (m_pDirectionalLight)
    {
      oGlobalLightningData.DirectionalLight.Intensity = m_pDirectionalLight->GetIntensity();
      oGlobalLightningData.DirectionalLight.Color = m_pDirectionalLight->GetColor();
      oGlobalLightningData.DirectionalLight.Direction = m_pDirectionalLight->GetDirection();
    }

    // Point lights
    for (uint32_t uIndex = 0; uIndex < m_uRegisteredPointLights; uIndex++)
    {
      render::lights::CPointLight* pPointLight = m_vctPointLights[uIndex];
      oGlobalLightningData.PointLights[uIndex].Position = pPointLight->GetTransform().GetPosition();
      oGlobalLightningData.PointLights[uIndex].Color = pPointLight->GetColor();
      oGlobalLightningData.PointLights[uIndex].Intensity = pPointLight->GetIntensity();
      oGlobalLightningData.PointLights[uIndex].Range = pPointLight->GetRange();
    }

    // Spot lights
    for (uint32_t uIndex = 0; uIndex < m_uRegisteredSpotLights; uIndex++)
    {
      render::lights::CSpotLight* pSpotLight = m_vctSpotLights[uIndex];
      oGlobalLightningData.SpotLights[uIndex].Color = pSpotLight->GetTransform().GetPosition();
      oGlobalLightningData.SpotLights[uIndex].CutOffAngle = pSpotLight->GetCutOffAngle();
      oGlobalLightningData.SpotLights[uIndex].Direction = pSpotLight->GetDirection();
      oGlobalLightningData.SpotLights[uIndex].Intensity = pSpotLight->GetIntensity();
      oGlobalLightningData.SpotLights[uIndex].Position = pSpotLight->GetTransform().GetPosition();
    }

    // Update buffer
    m_oLightningBuffer.UpdateBuffer();

    // Apply constant buffer
    ID3D11Buffer* pConstantBuffer = m_oLightningBuffer.GetBuffer();
    global::dx11::s_pDeviceContext->PSSetConstantBuffers(1, 1, &pConstantBuffer);
  }
  // ------------------------------------
  void CScene::DestroyAllPrimitives()
  {
    std::for_each(m_vctPrimitiveItems.begin(), m_vctPrimitiveItems.end(), [](render::graphics::CPrimitive*& _pPrimitive)
      {
        if (_pPrimitive)
        {
          delete _pPrimitive;
          _pPrimitive = nullptr;
        }
      });
    m_uRegisteredPrimitives = 0;
  }
  // ------------------------------------
  void CScene::DestroyAllModels()
  {
    std::for_each(m_vctModels.begin(), m_vctModels.end(), [](render::graphics::CModel*& _pModel)
      {
        if (_pModel)
        {
          delete _pModel;
          _pModel = nullptr;
        }
      });
    m_uRegisteredModels = 0;
  }
  // ------------------------------------
  void CScene::DestroyAllLights()
  {
    /*std::for_each(m_vctLights.begin(), m_vctLights.end(), [](render::lights::CLight*& _pLight)
      {
        if (_pLight)
        {
          delete _pLight;
          _pLight = nullptr;
        }
      });
    m_uRegisteredLights = 0;*/
  }
  // ------------------------------------
  render::graphics::CPrimitive* CScene::CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData)
  {
    if (m_uRegisteredPrimitives >= s_iMaxPrimitives) return nullptr;
    render::graphics::CPrimitive*& pPrimitiveItem = m_vctPrimitiveItems[m_uRegisteredPrimitives++];
    pPrimitiveItem = new render::graphics::CPrimitive(_vctVertexData);
    return pPrimitiveItem;
  }
  // ------------------------------------
  render::graphics::CPrimitive* CScene::CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType)
  {
    if (m_uRegisteredPrimitives >= s_iMaxPrimitives) return nullptr;
    render::graphics::CPrimitive*& pPrimitiveItem = m_vctPrimitiveItems[m_uRegisteredPrimitives++];
    pPrimitiveItem = new render::graphics::CPrimitive(_ePrimitiveType);
    return pPrimitiveItem;
  }
  // ------------------------------------
  render::graphics::CModel* CScene::CreateModel(const char* _sModelPath, const char* _sBaseMltDir)
  {
    if (m_uRegisteredPrimitives >= s_iMaxPrimitives) return nullptr;
    render::graphics::CModel*& pModel = m_vctModels[m_uRegisteredModels++];
    pModel = new render::graphics::CModel(_sModelPath, _sBaseMltDir);
    return pModel;
  }
  // ------------------------------------
  render::lights::CDirectionalLight* CScene::CreateDirectionalLight()
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
  render::lights::CPointLight* CScene::CreatePointLight()
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
  render::lights::CSpotLight* CScene::CreateSpotLight()
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
  void CScene::DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_)
  {
    assert(pPrimitive_);
    auto it = std::find(m_vctPrimitiveItems.begin(), m_vctPrimitiveItems.end(), pPrimitive_);
    if (it != m_vctPrimitiveItems.end())
    {
      delete* it;
      *it = nullptr;
      m_uRegisteredPrimitives--;

      auto oReorderFunc = std::remove_if(m_vctPrimitiveItems.begin(), m_vctPrimitiveItems.end(),
        [](render::graphics::CPrimitive* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_vctPrimitiveItems.end(), nullptr); // Set nullptr
    }
    pPrimitive_ = nullptr;
  }
  // ------------------------------------
  void CScene::DestroyModel(render::graphics::CModel*& pModel_)
  {
    assert(pModel_);
    auto it = std::find(m_vctModels.begin(), m_vctModels.end(), pModel_);
    if (it != m_vctModels.end())
    {
      delete* it;
      *it = nullptr;
      m_uRegisteredModels--;

      auto oReorderFunc = std::remove_if(m_vctModels.begin(), m_vctModels.end(),
        [](render::graphics::CModel* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_vctModels.end(), nullptr); // Set nullptr
    }
    pModel_ = nullptr;
  }
  // ------------------------------------
  void CScene::DestroyLight(render::lights::CLight*& _pLight_)
  {
    assert(_pLight_);

    switch (_pLight_->GetLightType())
    {
    case render::lights::ELightType::DIRECTIONAL_LIGHT:
      delete _pLight_;
      _pLight_ = nullptr;
      break;
    case render::lights::ELightType::POINT_LIGHT:
    {
      auto it = std::find(m_vctPointLights.begin(), m_vctPointLights.end(), _pLight_);
      if (it != m_vctPointLights.end())
      {
        delete* it;
        *it = nullptr;
        m_uRegisteredPointLights--;

        auto oReorderFunc = std::remove_if(m_vctPointLights.begin(), m_vctPointLights.end(),
          [](render::lights::CLight* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
        std::fill(oReorderFunc, m_vctPointLights.end(), nullptr); // Set nullptr
      }
      _pLight_ = nullptr;
    }
    break;
    case render::lights::ELightType::SPOT_LIGHT:
    {
      auto it = std::find(m_vctSpotLights.begin(), m_vctSpotLights.end(), _pLight_);
      if (it != m_vctSpotLights.end())
      {
        delete* it;
        *it = nullptr;
        m_uRegisteredSpotLights--;

        auto oReorderFunc = std::remove_if(m_vctSpotLights.begin(), m_vctSpotLights.end(),
        [](render::lights::CLight* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
        std::fill(oReorderFunc, m_vctSpotLights.end(), nullptr); // Set nullptr
      }
      _pLight_ = nullptr;
    }
    break;
    default:
      break;
    }
  }
}
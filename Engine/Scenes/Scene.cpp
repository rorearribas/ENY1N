#include "Scene.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Base/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/Lights/Light.h"
#include "Engine/Render/Lights/DirectionalLight.h"
#include "Engine/Render/Graphics/PrimitiveUtils.h"
#include "Libs/Macros/GlobalMacros.h"

#include <algorithm>
#include <random>
#include <cassert>
#include <iostream>

namespace scene
{
  CScene::CScene(uint32_t _uIndex) : m_uSceneIdx(_uIndex)
  {
    HRESULT hResult = m_oLightingBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);
    UNUSED_VARIABLE(hResult);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif
  }
  // ------------------------------------
  CScene::~CScene()
  {
    m_oLightingBuffer.CleanBuffer();
    DestroyAllPrimitives();
    DestroyAllModels();
    DestroyAllLights();
  }
  // ------------------------------------
  void CScene::DrawPrimitives()
  {
    // Draw primitives
    for (uint32_t uIndex = 0; uIndex < m_vctPrimitiveItems.CurrentSize(); uIndex++)
    {
      render::graphics::CPrimitive* pPrimitiveItem = m_vctPrimitiveItems[uIndex];
      pPrimitiveItem->DrawPrimitive();
    }

    // Draw temporal primitives
    uint32_t uTempSize = m_vctTemporalItems.CurrentSize();
    for (uint32_t uIndex = 0; uIndex < uTempSize; uIndex++)
    {
      render::graphics::CPrimitive* pPrimitiveItem = m_vctTemporalItems[uIndex];
      if (pPrimitiveItem)
      {
        pPrimitiveItem->DrawPrimitive();
      }
    }
    
    // Clean after draw
    if (uTempSize > 0)
    {
      m_vctTemporalItems.ClearAll();
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
  void CScene::UpdateLighting()
  {
    // Fill data
    auto& oGlobalLightingData = m_oLightingBuffer.GetData();

    // Directional light
    if (m_pDirectionalLight)
    {
      oGlobalLightingData.DirectionalLight.Intensity = m_pDirectionalLight->GetIntensity();
      oGlobalLightingData.DirectionalLight.Color = m_pDirectionalLight->GetColor();
      oGlobalLightingData.DirectionalLight.Direction = m_pDirectionalLight->GetDirection();
    }

    // Point lights
    for (uint32_t uIndex = 0; uIndex < m_vctPointLights.CurrentSize(); uIndex++)
    {
      const render::lights::CPointLight* pPointLight = m_vctPointLights[uIndex];
      oGlobalLightingData.PointLights[uIndex].Position = pPointLight->GetPosition();
      oGlobalLightingData.PointLights[uIndex].Color = pPointLight->GetColor();
      oGlobalLightingData.PointLights[uIndex].Intensity = pPointLight->GetIntensity();
      oGlobalLightingData.PointLights[uIndex].Range = pPointLight->GetRange();
    }
    // Set the number of registered point lights
    oGlobalLightingData.RegisteredPointLights = static_cast<int>(m_vctPointLights.CurrentSize());

    // Spot lights
    for (uint32_t uIndex = 0; uIndex < m_vctSpotLights.CurrentSize(); uIndex++)
    {
      const render::lights::CSpotLight* pSpotLight = m_vctSpotLights[uIndex];
      oGlobalLightingData.SpotLights[uIndex].Position = pSpotLight->GetPosition();
      oGlobalLightingData.SpotLights[uIndex].Direction = pSpotLight->GetDirection();
      oGlobalLightingData.SpotLights[uIndex].Color = pSpotLight->GetColor();
      oGlobalLightingData.SpotLights[uIndex].Range = pSpotLight->GetRange();
      oGlobalLightingData.SpotLights[uIndex].Intensity = pSpotLight->GetIntensity();
    }
    // Set the number of registered spot lights
    oGlobalLightingData.RegisteredSpotLights = static_cast<int>(m_vctSpotLights.CurrentSize());

    // Write buffer
    bool bOk = m_oLightingBuffer.WriteBuffer();
    UNUSED_VARIABLE(bOk);
#ifdef _DEBUG
    assert(bOk); // Sanity check
#endif

    // Apply constant buffer
    ID3D11Buffer* pConstantBuffer = m_oLightingBuffer.GetBuffer();
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
    global::ReleaseObject(m_pDirectionalLight);
    m_vctPointLights.ClearAll();
    m_vctSpotLights.ClearAll();
  }
  // ------------------------------------
  void CScene::DrawLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest, const math::CVector3& _v3Color)
  {
    if (m_vctTemporalItems.CurrentSize() >= m_vctTemporalItems.GetMaxSize())
    {
      std::cout << "You have reached maximum temporal items in the current scene" << std::endl;
      return;
    }

    // Create line
    render::graphics::CPrimitive::SCustomPrimitive oCustomData = render::graphics::CPrimitive::SCustomPrimitive();
    render::graphics::CPrimitiveUtils::CreateLine(_v3Origin, _v3Dest, oCustomData);

    // Create temporal item
    render::graphics::CPrimitive* pPrimitive = m_vctTemporalItems.CreateItem(oCustomData, render::ERenderMode::WIREFRAME);
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set values
    pPrimitive->UseGlobalLighting(false);
    pPrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CScene::DrawCube(const math::CVector3& _v3Pos, float _fSize, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_vctTemporalItems.CurrentSize() >= m_vctTemporalItems.GetMaxSize())
    {
      std::cout << "You have reached maximum temporal items in the current scene" << std::endl;
      return;
    }

    // Create vertex data
    render::graphics::CPrimitive::SCustomPrimitive oVertexData = render::graphics::CPrimitive::SCustomPrimitive();

    // Set primitive data + update pos
    oVertexData.m_vctVertexData = render::graphics::CPrimitiveUtils::s_oCubePrimitive;
    for (auto& oPrimitiveData : oVertexData.m_vctVertexData)
    {
      oPrimitiveData.Position *= _fSize;
    }

    // Set indices
    oVertexData.m_vctIndices = render::graphics::CPrimitiveUtils::s_oCubeIndices;

    // Create item + set pos
    render::graphics::CPrimitive* pPrimitive = m_vctTemporalItems.CreateItem(oVertexData, _eRenderMode);
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set values
    pPrimitive->SetPosition(_v3Pos);
    pPrimitive->SetColor(_v3Color);
    pPrimitive->UseGlobalLighting(false);
  }
  // ------------------------------------
  void CScene::DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iStacks, int _iSlices, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_vctTemporalItems.CurrentSize() >= m_vctTemporalItems.GetMaxSize())
    {
      std::cout << "You have reached maximum temporal items in the current scene" << std::endl;
      return;
    }

    // Fill primitive data
    render::graphics::CPrimitive::SCustomPrimitive oVertexData = render::graphics::CPrimitive::SCustomPrimitive();
    render::graphics::CPrimitiveUtils::CreateSphere(_fRadius, _iStacks, _iSlices, oVertexData.m_vctVertexData);

    // Fill indices
    oVertexData.m_vctIndices = _eRenderMode == render::ERenderMode::SOLID ? render::graphics::CPrimitiveUtils::GetSphereIndices(_iStacks, _iSlices) :
      render::graphics::CPrimitiveUtils::GetWireframeSphereIndices(_iStacks, _iSlices);

    // Compute normals
    render::graphics::CPrimitiveUtils::ComputeNormals(oVertexData.m_vctVertexData, oVertexData.m_vctIndices);

    // Create temporal item + set pos
    render::graphics::CPrimitive* pPrimitive = m_vctTemporalItems.CreateItem(oVertexData, _eRenderMode);
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set values
    pPrimitive->SetPosition(_v3Pos);
    pPrimitive->SetColor(_v3Color);
    pPrimitive->UseGlobalLighting(false);
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
#ifdef _DEBUG
    assert(bOk); // Sanity check
#endif
    _pPrimitive_ = nullptr; // Set as nullptr
  }
  // ------------------------------------
  void CScene::DestroyModel(render::graphics::CModel*& _pModel_)
  {
    bool bOk = m_vctModels.RemoveItem(_pModel_);
    UNUSED_VARIABLE(bOk);
#ifdef _DEBUG
    assert(bOk); // Sanity check
#endif
    _pModel_ = nullptr; // Set as nullptr
  }
  // ------------------------------------
  void CScene::DestroyLight(render::lights::CBaseLight*& _pLight_)
  {
    bool bOk = false;
    switch (_pLight_->GetLightType())
    {
    case render::lights::ELightType::DIRECTIONAL_LIGHT:
    {
      bOk = global::ReleaseObject(m_pDirectionalLight);
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

#ifdef _DEBUG
    assert(bOk); // Sanity check
#endif
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
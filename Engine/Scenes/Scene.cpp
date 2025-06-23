#include "Scene.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Base/Engine.h"
#include "Engine/Utils/Plane.h"
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
    HRESULT hResult = m_oGlobalLightingBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);
    UNUSED_VAR(hResult);
#ifdef _DEBUG
    assert(!FAILED(hResult));
#endif
  }
  // ------------------------------------
  CScene::~CScene()
  {
    m_oGlobalLightingBuffer.CleanBuffer();
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
    uint32_t uTempSize = m_vctDebugItems.CurrentSize();
    for (uint32_t uIndex = 0; uIndex < uTempSize; uIndex++)
    {
      render::graphics::CPrimitive* pPrimitiveItem = m_vctDebugItems[uIndex];
      pPrimitiveItem->DrawPrimitive();
    }
    
    // Clean after draw
    if (uTempSize > 0)
    {
      m_vctDebugItems.ClearAll();
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
    auto& oGlobalLightingData = m_oGlobalLightingBuffer.GetData();

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
    bool bOk = m_oGlobalLightingBuffer.WriteBuffer();
    UNUSED_VAR(bOk);
#ifdef _DEBUG
    assert(bOk); // Sanity check
#endif

    // Apply constant buffer
    ID3D11Buffer* pConstantBuffer = m_oGlobalLightingBuffer.GetBuffer();
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
  void CScene::DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color,
  float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode)
  {
    if (m_vctDebugItems.CurrentSize() >= m_vctDebugItems.GetMaxSize())
    {
      std::cout << "You have reached maximum temporal items in the current scene" << std::endl;
      return;
    }

    // Fill primitive data
    using namespace render::graphics;
    CPrimitive::SCustomPrimitive oPrimitiveData = CPrimitive::SCustomPrimitive();
    CPrimitiveUtils::CreateCapsule(_fRadius, _fHeight, _iSubvH, _iSubvV, oPrimitiveData, _eRenderMode);

    // Compute normals
    CPrimitiveUtils::ComputeNormals(oPrimitiveData.m_vctVertexData, oPrimitiveData.m_vctIndices);

    // Create temporal item + set pos
    CPrimitive* pCapsulePrimitive = m_vctDebugItems.CreateItem(oPrimitiveData, _eRenderMode);

#ifdef _DEBUG
    assert(pCapsulePrimitive); // Sanity check
#endif

    // Set values
    pCapsulePrimitive->SetPosition(_v3Pos);
    pCapsulePrimitive->SetRotation(_v3Rot);
    pCapsulePrimitive->SetColor(_v3Color);
    pCapsulePrimitive->UseGlobalLighting(false);
  }
  // ------------------------------------
  void CScene::DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Size, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_vctDebugItems.CurrentSize() >= m_vctDebugItems.GetMaxSize())
    {
      std::cout << "You have reached maximum temporal items in the current scene" << std::endl;
      return;
    }

    // Create cube
    using namespace render::graphics;
    CPrimitive* pCubePrimitive = m_vctDebugItems.CreateItem(CPrimitive::EPrimitiveType::E3D_CUBE, _eRenderMode);
#ifdef _DEBUG
    assert(pCubePrimitive); // Sanity check
#endif

    // Set values
    pCubePrimitive->SetPosition(_v3Pos);
    pCubePrimitive->SetRotation(_v3Rot);
    pCubePrimitive->SetScale(_v3Size);
    pCubePrimitive->SetColor(_v3Color);
    pCubePrimitive->UseGlobalLighting(false);
  }
  // ------------------------------------
  void CScene::DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_vctDebugItems.CurrentSize() >= m_vctDebugItems.GetMaxSize())
    {
      std::cout << "You have reached maximum temporal items in the current scene" << std::endl;
      return;
    }

    // Fill primitive data
    using namespace render::graphics;
    CPrimitive::SCustomPrimitive oPrimitiveData = CPrimitive::SCustomPrimitive();
    CPrimitiveUtils::CreateSphere(_fRadius, _iSubvH, _iSubvV, oPrimitiveData.m_vctVertexData);

    // Fill indices
    oPrimitiveData.m_vctIndices = _eRenderMode == render::ERenderMode::SOLID ? 
    CPrimitiveUtils::GetSphereIndices(_iSubvH, _iSubvV) :
    CPrimitiveUtils::GetWireframeSphereIndices(_iSubvH, _iSubvV);

    // Compute normals
    CPrimitiveUtils::ComputeNormals(oPrimitiveData.m_vctVertexData, oPrimitiveData.m_vctIndices);

    // Create temporal item + set pos
    CPrimitive* pSpherePrimitive = m_vctDebugItems.CreateItem(oPrimitiveData, _eRenderMode);
#ifdef _DEBUG
    assert(pSpherePrimitive); // Sanity check
#endif

    // Set values
    pSpherePrimitive->SetPosition(_v3Pos);
    pSpherePrimitive->SetColor(_v3Color);
    pSpherePrimitive->UseGlobalLighting(false);
  }
  // ------------------------------------
  void CScene::DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_vctDebugItems.CurrentSize() >= m_vctDebugItems.GetMaxSize())
    {
      std::cout << "You have reached maximum temporal items in the current scene" << std::endl;
      return;
    }

    // Create plane
    using namespace render::graphics;
    CPrimitive::SCustomPrimitive oPrimitiveData = CPrimitive::SCustomPrimitive();
    CPrimitiveUtils::CreatePlane(_oPlane, oPrimitiveData, _eRenderMode);

    // Create primitive
    CPrimitive* pPlanePrimitive = m_vctDebugItems.CreateItem(oPrimitiveData, _eRenderMode);
#ifdef _DEBUG
    assert(pPlanePrimitive); // Sanity check
#endif

    // Set values
    pPlanePrimitive->SetPosition(_oPlane.GetPos());
    pPlanePrimitive->SetScale(_v3Size);
    pPlanePrimitive->SetColor(_v3Color);
    pPlanePrimitive->UseGlobalLighting(false);
  }
  // ------------------------------------
  void CScene::DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color)
  {
    if (m_vctDebugItems.CurrentSize() >= m_vctDebugItems.GetMaxSize())
    {
      std::cout << "You have reached maximum temporal items in the current scene" << std::endl;
      return;
    }

    // Create line
    render::graphics::CPrimitive::SCustomPrimitive oCustomData = render::graphics::CPrimitive::SCustomPrimitive();
    render::graphics::CPrimitiveUtils::CreateLine(_v3Start, _v3Dest, oCustomData);

    // Create temporal item
    render::graphics::CPrimitive* pLinePrimitive = m_vctDebugItems.CreateItem(oCustomData, render::ERenderMode::WIREFRAME);
#ifdef _DEBUG
    assert(pLinePrimitive); // Sanity check
#endif

    // Set values
    pLinePrimitive->SetColor(_v3Color);
    pLinePrimitive->UseGlobalLighting(false);
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
    UNUSED_VAR(bOk);
#ifdef _DEBUG
    assert(bOk); // Sanity check
#endif
    _pPrimitive_ = nullptr; // Set as nullptr
  }
  // ------------------------------------
  void CScene::DestroyModel(render::graphics::CModel*& _pModel_)
  {
    bool bOk = m_vctModels.RemoveItem(_pModel_);
    UNUSED_VAR(bOk);
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
        render::lights::CPointLight* pPointLight = static_cast<render::lights::CPointLight*>(_pLight_);
        bOk = m_vctPointLights.RemoveItem(pPointLight);
      }
      break;
      case render::lights::ELightType::SPOT_LIGHT:
      {
        render::lights::CSpotLight* pSpotLight = static_cast<render::lights::CSpotLight*>(_pLight_);
        bOk = m_vctSpotLights.RemoveItem(pSpotLight);
      }
      break;
      default:
        break;
    }

#ifdef _DEBUG
    assert(bOk); // Sanity check
#endif
    _pLight_ = nullptr; // Set as nullptr
  }
}
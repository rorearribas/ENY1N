#include "Scene.h"
#include "Engine/Engine.h"
#include "Engine/Utils/Plane.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/Lights/Light.h"
#include "Engine/Render/Lights/DirectionalLight.h"
#include "Engine/Render/Utils/PrimitiveUtils.h"
#include "Engine/Managers/ResourceManager.h"
#include "Engine/Global/GlobalResources.h"

#include "Libs/Macros/GlobalMacros.h"
#include <algorithm>
#include <random>
#include <cassert>
#include <iostream>

namespace scene
{
  CScene::CScene(uint32_t _uIndex) : m_uSceneIdx(_uIndex)
  {
    HRESULT hResult = m_oGlobalLightingBuffer.Init(global::dx::s_pDevice, global::dx::s_pDeviceContext);
    assert(!FAILED(hResult));
    hResult = m_oHandleInstancingBuffer.Init(global::dx::s_pDevice, global::dx::s_pDeviceContext);
    assert(!FAILED(hResult));
  }
  // ------------------------------------
  CScene::~CScene()
  {
    Clear();
  }
  // ------------------------------------
  render::gfx::CPrimitive* const CScene::CreatePrimitive(render::EPrimitiveType _eType, render::ERenderMode _eRenderMode)
  {
    if (m_lstPrimitives.GetCurrentSize() >= m_lstPrimitives.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum primitives in the current scene!");
      return nullptr;
    }
    return m_lstPrimitives.Create(_eType, _eRenderMode);
  }
  // ------------------------------------
  render::gfx::CModel* const CScene::LoadModel(const char* _sModelPath)
  {
    if (m_lstModels.GetCurrentSize() >= m_lstModels.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum models in the current scene!");
      return nullptr;
    }

    // Load model
    bool bCached = false;
    CResourceManager* pResourceManager = CResourceManager::GetInstance();
    render::gfx::CModel* pModel = pResourceManager->LoadModel(_sModelPath, bCached);
    assert(pModel);

    if (bCached)
    {
      pModel->CreateInstance();
    }
    else
    {
      m_lstModels.Insert(pModel);
    }

    return pModel;
  }
  // ------------------------------------
  render::lights::CDirectionalLight* const CScene::CreateDirectionalLight()
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
  render::lights::CPointLight* const CScene::CreatePointLight()
  {
    if (m_lstPointLights.GetCurrentSize() >= s_uMaxPointLights)
    {
      WARNING_LOG("You have reached maximum point lights in the current scene!");
      return nullptr;
    }
    return m_lstPointLights.Create();
  }
  // ------------------------------------
  render::lights::CSpotLight* const CScene::CreateSpotLight()
  {
    if (m_lstSpotLights.GetCurrentSize() >= s_uMaxSpotLights)
    {
      WARNING_LOG("You have reached maximum spot lights in the current scene!");
      return nullptr;
    }
    return m_lstSpotLights.Create();
  }
  // ------------------------------------
  void CScene::DestroyModel(render::gfx::CModel*& _pModel_)
  {
    bool bOk = m_lstModels.Remove(_pModel_, false);
    UNUSED_VAR(bOk);
#ifdef _DEBUG
    assert(bOk); // Sanity check
#endif
  }
  // ------------------------------------
  void CScene::DestroyPrimitive(render::gfx::CPrimitive*& _pPrimitive_)
  {
    bool bOk = m_lstPrimitives.Remove(_pPrimitive_);
    UNUSED_VAR(bOk);
#ifdef _DEBUG
    assert(bOk); // Sanity check
#endif
  }
  // ------------------------------------
  void CScene::DestroyLight(render::lights::CLight*& _pLight_)
  {
    bool bOk = false;
    switch (_pLight_->GetLightType())
    {
      case render::ELightType::DIRECTIONAL_LIGHT:
      {
        bOk = global::ReleaseObject(m_pDirectionalLight);
      }
      break;
      case render::ELightType::POINT_LIGHT:
      {
        render::lights::CPointLight* pPointLight = static_cast<render::lights::CPointLight*>(_pLight_);
        bOk = m_lstPointLights.Remove(pPointLight);
      }
      break;
      case render::ELightType::SPOT_LIGHT:
      {
        render::lights::CSpotLight* pSpotLight = static_cast<render::lights::CSpotLight*>(_pLight_);
        bOk = m_lstSpotLights.Remove(pSpotLight);
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
  // ------------------------------------
  void CScene::DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color,
    float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode)
  {
    if (m_lstDebugItems.GetCurrentSize() >= m_lstDebugItems.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum temporal items in the current scene");
      return;
    }

    // Fill primitive data
    auto oPrimitiveData = render::gfx::CPrimitiveUtils::CreateCapsule(_fRadius, _fHeight, _iSubvH, _iSubvV, _eRenderMode);

    // Compute normals
    render::gfx::CPrimitiveUtils::ComputeNormals(oPrimitiveData.m_lstVertexData, oPrimitiveData.m_lstIndices);

    // Create temporal item + set pos
    render::gfx::CPrimitive* pPrimitive = m_lstDebugItems.Create(oPrimitiveData, _eRenderMode);

#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set values
    pPrimitive->SetPosition(_v3Pos);
    pPrimitive->SetRotation(_v3Rot);
    pPrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CScene::DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Size, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_lstDebugItems.GetCurrentSize() >= m_lstDebugItems.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Create cube
    using namespace render::gfx;
    CPrimitive* pPrimitive = m_lstDebugItems.Create(render::EPrimitiveType::E3D_CUBE, _eRenderMode);
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set values
    pPrimitive->SetPosition(_v3Pos);
    pPrimitive->SetRotation(_v3Rot);
    pPrimitive->SetScale(_v3Size);
    pPrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CScene::DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_lstDebugItems.GetCurrentSize() >= m_lstDebugItems.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Fill primitive data
    using namespace render::gfx;
    SCustomPrimitive oPrimitiveData = SCustomPrimitive();
    CPrimitiveUtils::CreateSphere(_fRadius, _iSubvH, _iSubvV, oPrimitiveData.m_lstVertexData);

    // Fill indices
    oPrimitiveData.m_lstIndices = (_eRenderMode == render::ERenderMode::SOLID) ? CPrimitiveUtils::GetSphereIndices(_iSubvH, _iSubvV) :
      CPrimitiveUtils::GetWireframeSphereIndices(_iSubvH, _iSubvV);

    // Compute normals
    CPrimitiveUtils::ComputeNormals(oPrimitiveData.m_lstVertexData, oPrimitiveData.m_lstIndices);

    // Create temporal item + set pos
    CPrimitive* pSpherePrimitive = m_lstDebugItems.Create(oPrimitiveData, _eRenderMode);
#ifdef _DEBUG
    assert(pSpherePrimitive); // Sanity check
#endif

    // Set values
    pSpherePrimitive->SetPosition(_v3Pos);
    pSpherePrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CScene::DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_lstDebugItems.GetCurrentSize() >= m_lstDebugItems.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Create plane
    auto oData = render::gfx::CPrimitiveUtils::CreatePlane(_oPlane, _eRenderMode);

    // Create primitive
    render::gfx::CPrimitive* pPlanePrimitive = m_lstDebugItems.Create(oData, _eRenderMode);
#ifdef _DEBUG
    assert(pPlanePrimitive); // Sanity check
#endif

    // Set values
    pPlanePrimitive->SetPosition(_oPlane.GetPos());
    pPlanePrimitive->SetScale(_v3Size);
    pPlanePrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CScene::DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color)
  {
    if (m_lstDebugItems.GetCurrentSize() >= m_lstDebugItems.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Create data
    using namespace render::gfx;
    SCustomPrimitive oData = CPrimitiveUtils::CreateLine(_v3Start, _v3Dest);
    // Create temporal item
    CPrimitive* pPrimitive = m_lstDebugItems.Create(oData, render::ERenderMode::WIREFRAME);
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set value
    pPrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CScene::DrawModels()
  {
    // Check frustum
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    render::CCamera* pCamera = pEngine->GetCamera();

    // I have to change this!!!
    ID3D11Buffer* pConstantBuffer = m_oHandleInstancingBuffer.GetBuffer();
    global::dx::s_pDeviceContext->VSSetConstantBuffers(1, 1, &pConstantBuffer);

    // Draw
    for (uint32_t uIndex = 0; uIndex < m_lstModels.GetCurrentSize(); uIndex++)
    {
      render::gfx::CModel* pModel = m_lstModels[uIndex];
      if (!pModel->IsVisible() && !pModel->HasInstances())
      {
        continue;
      }

      // Draw model
      if (pModel->IsVisible())
      {
        bool bDrawModel = true;
        if (pModel->IsCullingEnabled()) // Check culling
        {
          bDrawModel = pCamera->IsOnFrustum(pModel->GetBoundingBox());
        }
        if (bDrawModel)
        {
          pModel->Draw();
        }
      }

      // Handle instances
      std::vector<uint32_t> lstInstancesIds = std::vector<uint32_t>();
      for (const render::gfx::CRenderInstance& rInstance : pModel->GetInstances())
      {
        if (!rInstance.IsVisible())
        {
          continue;
        }

        bool bDrawInstance = true;
        if (rInstance.IsCullingEnabled()) // Check culling
        {
          bDrawInstance = pCamera->IsOnFrustum(rInstance.GetBoundingBox());
        }
        if (bDrawInstance)
        {
          lstInstancesIds.emplace_back(rInstance.GetInstanceID());
        }
      }

      if (lstInstancesIds.size() > 0)
      {
        // Write
        m_oHandleInstancingBuffer.GetData().IsInstantiated = true;
        bool bOk = m_oHandleInstancingBuffer.WriteBuffer();
        UNUSED_VAR(bOk);
        assert(bOk);

        // Draw instances
        pModel->DrawInstances(lstInstancesIds);
      }
    }

    // Write
    m_oHandleInstancingBuffer.GetData().IsInstantiated = false;
    bool bOk = m_oHandleInstancingBuffer.WriteBuffer();
    UNUSED_VAR(bOk);
    assert(bOk);

    ID3D11Buffer* pInvalidBuffer = nullptr;
    global::dx::s_pDeviceContext->VSSetConstantBuffers(1, 1, &pInvalidBuffer);
  }
  // ------------------------------------
  void CScene::DrawPrimitives()
  {
    // Check frustum
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    render::CCamera* pCamera = pEngine->GetCamera();

    // Draw primitives
    for (uint32_t uIndex = 0; uIndex < m_lstPrimitives.GetCurrentSize(); uIndex++)
    {
      render::gfx::CPrimitive* pPrimitive = m_lstPrimitives[uIndex];
      if (!pPrimitive->IsVisible())
      {
        continue;
      }

      bool bDrawPrimitive = true;
      if (pPrimitive->IsCullingEnabled()) // Check culling
      {
        bDrawPrimitive = pCamera->IsOnFrustum(pPrimitive->GetBoundingBox());
      }
      if (bDrawPrimitive)
      {
        pPrimitive->Draw();
      }
    }
  }
  // ------------------------------------
  void CScene::DrawDebug()
  {
    // Check frustum
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    render::CCamera* pCamera = pEngine->GetCamera();

    // Draw debug primitives
    uint32_t uTempSize = m_lstDebugItems.GetCurrentSize();
    for (uint32_t uIndex = 0; uIndex < uTempSize; uIndex++)
    {
      render::gfx::CPrimitive* pDebug = m_lstDebugItems[uIndex];
      if (!pDebug->IsVisible())
      {
        // This is strange because normally nobody wants to hide a debugging object - just in case!
        continue;
      }

      bool bDrawDebug = true;
      if (pDebug->IsCullingEnabled()) // Check culling
      {
        bDrawDebug = pCamera->IsOnFrustum(pDebug->GetBoundingBox());
      }
      if (bDrawDebug)
      {
        pDebug->Draw();
      }
    }

    // Clean after draw
    if (uTempSize > 0)
    {
      m_lstDebugItems.Clear();
    }
  }
  // ------------------------------------
  void CScene::ApplyLightning()
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
    for (uint32_t uIndex = 0; uIndex < m_lstPointLights.GetCurrentSize(); uIndex++)
    {
      const render::lights::CPointLight* pPointLight = m_lstPointLights[uIndex];
      oGlobalLightingData.PointLights[uIndex].Position = pPointLight->GetPosition();
      oGlobalLightingData.PointLights[uIndex].Color = pPointLight->GetColor();
      oGlobalLightingData.PointLights[uIndex].Intensity = pPointLight->GetIntensity();
      oGlobalLightingData.PointLights[uIndex].Range = pPointLight->GetRange();
    }
    // Set the number of registered point lights
    oGlobalLightingData.RegisteredPointLights = static_cast<int>(m_lstPointLights.GetCurrentSize());

    // Spot lights
    for (uint32_t uIndex = 0; uIndex < m_lstSpotLights.GetCurrentSize(); uIndex++)
    {
      const render::lights::CSpotLight* pSpotLight = m_lstSpotLights[uIndex];
      oGlobalLightingData.SpotLights[uIndex].Position = pSpotLight->GetPosition();
      oGlobalLightingData.SpotLights[uIndex].Direction = pSpotLight->GetDirection();
      oGlobalLightingData.SpotLights[uIndex].Color = pSpotLight->GetColor();
      oGlobalLightingData.SpotLights[uIndex].Range = pSpotLight->GetRange();
      oGlobalLightingData.SpotLights[uIndex].Intensity = pSpotLight->GetIntensity();
    }
    // Set the number of registered spot lights
    oGlobalLightingData.RegisteredSpotLights = static_cast<int>(m_lstSpotLights.GetCurrentSize());

    // Write buffer
    bool bOk = m_oGlobalLightingBuffer.WriteBuffer();
    UNUSED_VAR(bOk);
#ifdef _DEBUG
    assert(bOk); // Sanity check
#endif

    // Apply constant buffer
    ID3D11Buffer* pConstantBuffer = m_oGlobalLightingBuffer.GetBuffer();
    global::dx::s_pDeviceContext->PSSetConstantBuffers(1, 1, &pConstantBuffer);
  }
  // ------------------------------------
  void CScene::Clear()
  {
    // Models + primitives
    m_lstPrimitives.Clear();
    m_lstModels.Clear();

    // Lighting
    global::ReleaseObject(m_pDirectionalLight);
    m_lstPointLights.Clear();
    m_lstSpotLights.Clear();

    // Constant buffer
    m_oGlobalLightingBuffer.Clear();
  }
}
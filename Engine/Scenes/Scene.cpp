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
  // ------------------------------------
  CScene::~CScene()
  {
    Clear();
  }
  // ------------------------------------
  render::gfx::CPrimitive* const CScene::CreatePrimitive(render::EPrimitive _eType, render::ERenderMode _eRenderMode)
  {
    if (m_lstPrimitives.GetSize() >= m_lstPrimitives.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum primitives in the current scene!");
      return nullptr;
    }
    return m_lstPrimitives.Create(_eType, _eRenderMode);
  }
  // ------------------------------------
  bool CScene::DestroyPrimitive(render::gfx::CPrimitive*& _pPrimitive_)
  {
    return m_lstPrimitives.Remove(_pPrimitive_);
  }
  // ------------------------------------
  utils::CWeakPtr<render::gfx::CModel> const CScene::LoadModel(const char* _sModelPath)
  {
    // Check preload model
    utils::CWeakPtr<render::gfx::CModel> wpModel;
    for (uint32_t uIndex = 0; uIndex < m_lstModels.GetSize(); uIndex++)
    {
      utils::CWeakPtr<render::gfx::CModel> wpCurrentModel = m_lstModels[uIndex];
      bool bPreloaded = wpCurrentModel->AllowInstancing() && (wpCurrentModel->GetAssetPath() == _sModelPath);
      if (bPreloaded)
      {
        wpModel = wpCurrentModel;
        break;
      }
    }

    if (m_lstModels.GetSize() >= m_lstModels.GetMaxSize() && !wpModel.IsValid())
    {
      WARNING_LOG("You have reached maximum models in the current scene!");
      return utils::CWeakPtr<render::gfx::CModel>();
    }

    // Create instance
    if (wpModel.IsValid() && wpModel->CreateInstance())
    {      
      LOG("Created Instance! -> " << _sModelPath);
    }
    else
    { 
      // Load model
      CResourceManager* pResourceManager = CResourceManager::GetInstance();
      std::unique_ptr<render::gfx::CModel> pLoadedModel = pResourceManager->LoadModel(_sModelPath);
      wpModel = m_lstModels.Insert(std::move(pLoadedModel));
    }

    return wpModel;
  }
  // ------------------------------------
  bool CScene::DestroyModel(utils::CWeakPtr<render::gfx::CModel> _wpModel_)
  {
    return m_lstModels.Remove(_wpModel_);
  }
  // ------------------------------------
  render::lights::CDirectionalLight* const CScene::CreateDirectionalLight()
  {
    return m_oLightManager.CreateDirectionalLight();
  }
  // ------------------------------------
  render::lights::CPointLight* const CScene::CreatePointLight()
  {
    return m_oLightManager.CreatePointLight();
  }
  // ------------------------------------
  render::lights::CSpotLight* const CScene::CreateSpotLight()
  {
    return m_oLightManager.CreateSpotLight();
  }
  // ------------------------------------
  bool CScene::DestroyLight(render::lights::CLight*& _pLight_)
  {
    return m_oLightManager.DestroyLight(_pLight_);
  }
  // ------------------------------------
  void CScene::DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color,
    float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode)
  {
    if (m_lstDebugItems.GetSize() >= m_lstDebugItems.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum temporal items in the current scene");
      return;
    }

    // Fill primitive data
    auto oPrimitiveData = render::gfx::CPrimitiveUtils::CreateCapsule(_fRadius, _fHeight, _iSubvH, _iSubvV, _eRenderMode);

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
    if (m_lstDebugItems.GetSize() >= m_lstDebugItems.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Create cube
    using namespace render::gfx;
    CPrimitive* pPrimitive = m_lstDebugItems.Create(render::EPrimitive::E3D_CUBE, _eRenderMode);
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
    if (m_lstDebugItems.GetSize() >= m_lstDebugItems.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Fill primitive data
    using namespace render::gfx;
    TCustomPrimitive oPrimitiveData = TCustomPrimitive();
    CPrimitiveUtils::CreateSphere(_fRadius, _iSubvH, _iSubvV, oPrimitiveData.PrimitiveData);

    // Fill indices
    oPrimitiveData.Indices = (_eRenderMode == render::ERenderMode::SOLID) ? CPrimitiveUtils::GetSphereIndices(_iSubvH, _iSubvV) :
      CPrimitiveUtils::GetWireframeSphereIndices(_iSubvH, _iSubvV);

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
    if (m_lstDebugItems.GetSize() >= m_lstDebugItems.GetMaxSize())
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
    if (m_lstDebugItems.GetSize() >= m_lstDebugItems.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Create data
    using namespace render::gfx;
    TCustomPrimitive oData = CPrimitiveUtils::CreateLine(_v3Start, _v3Dest);
    // Create temporal item
    CPrimitive* pPrimitive = m_lstDebugItems.Create(oData, render::ERenderMode::WIREFRAME);
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set values
    pPrimitive->SetCullingEnabled(false); // @Note: Special case that we cannot use culling!
    pPrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CScene::CacheModels(const render::CCamera* _pCamera)
  {
    // Draw
    for (uint32_t uIndex = 0; uIndex < m_lstModels.GetMaxSize(); uIndex++)
    {
      utils::CWeakPtr<render::gfx::CModel> pModel = m_lstModels[uIndex];
      if (!pModel.IsValid())
      {
        continue;
      }

      // Handle model
      TCachedModel& rCachedModel = m_lstCachedModels[uIndex];
      rCachedModel.Visible = pModel->IsVisible();
      if (rCachedModel.Visible)
      {
        bool bOnFrustum = true;
        if (pModel->IsCullingEnabled()) // Check culling
        {
          bOnFrustum = _pCamera->IsOnFrustum(pModel->GetWorldAABB());
        }
        rCachedModel.Visible = bOnFrustum;
      }

      // Handle instances
      rCachedModel.InstanceCount = 0;
      render::gfx::TInstances& lstInstances = pModel->GetInstances();
      for (uint32_t uI = 0; uI < lstInstances.GetMaxSize(); uI++)
      {
        render::gfx::CRenderInstance* pInstance = lstInstances[uI];
        if (!pInstance || !pInstance->IsVisible())
        {
          continue;
        }

        bool bOnFrustum = true;
        if (pInstance->IsCullingEnabled()) // Check culling
        {
          bOnFrustum = _pCamera->IsOnFrustum(pInstance->GetWorldAABB());
        }
        if (bOnFrustum)
        {
          rCachedModel.DrawableInstances[rCachedModel.InstanceCount++] = pInstance->GetInstanceID();
        }
      }
    }
  }
  // ------------------------------------
  void CScene::ApplyLighting()
  {
    m_oLightManager.Apply();
  }
  // ------------------------------------
  void CScene::DrawModels()
  {
    // Draw
    for (uint32_t uIndex = 0; uIndex < m_lstModels.GetMaxSize(); uIndex++)
    {
      utils::CWeakPtr<render::gfx::CModel> pModel = m_lstModels[uIndex];
      if (!pModel.IsValid())
      {
        continue;
      }

      // Handle model
      const TCachedModel& rCachedModel = m_lstCachedModels[uIndex];
      if (rCachedModel.Visible)
      {
        pModel->Draw();
      }

      // Handle instances
      if (rCachedModel.InstanceCount > 0)
      {
        engine::CEngine* pEngine = engine::CEngine::GetInstance();
        render::CRender* pRender = pEngine->GetRender();
        // Push mode
        pRender->SetInstancingMode(true);
        // Draw instances
        pModel->DrawInstances
        (
          rCachedModel.DrawableInstances,
          rCachedModel.InstanceCount
        );
        // Disabled mode
        pRender->SetInstancingMode(false);
      }
    }
  }
  // ------------------------------------
  void CScene::DrawPrimitives(const render::CCamera* _pCamera)
  {
    // Draw primitives
    for (uint32_t uIndex = 0; uIndex < m_lstPrimitives.GetSize(); uIndex++)
    {
      render::gfx::CPrimitive* pPrimitive = m_lstPrimitives[uIndex];
      if (!pPrimitive->IsVisible())
      {
        continue;
      }

      bool bDrawPrimitive = true;
      if (pPrimitive->IsCullingEnabled()) // Check culling
      {
        bDrawPrimitive = _pCamera->IsOnFrustum(pPrimitive->GetWorldAABB());
      }
      if (bDrawPrimitive)
      {
        pPrimitive->Draw();
      }
    }

    // Draw debug primitives
    size_t tDebugCount = m_lstDebugItems.GetSize();
    for (uint32_t uIndex = 0; uIndex < tDebugCount; uIndex++)
    {
      render::gfx::CPrimitive* pDebugPrimitive = m_lstDebugItems[uIndex];
      if (!pDebugPrimitive->IsVisible())
      {
        // This is strange because normally nobody wants to hide a debugging object - just in case!
        continue;
      }

      bool bDrawDebug = true;
      if (pDebugPrimitive->IsCullingEnabled()) // Check culling
      {
        bDrawDebug = _pCamera->IsOnFrustum(pDebugPrimitive->GetWorldAABB());
      }
      if (bDrawDebug)
      {
        pDebugPrimitive->Draw();
      }
    }

    // Clean after draw
    if (tDebugCount > 0)
    {
      m_lstDebugItems.Clear();
    }
  }
  // ------------------------------------
  void CScene::Clear()
  {
    // Models + primitives
    m_lstPrimitives.Clear();
    m_lstModels.Clear();
  }
}
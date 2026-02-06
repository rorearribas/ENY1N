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
    for (uint32_t uI = 0; uI < m_lstModels.GetSize(); uI++)
    {
      utils::CWeakPtr<render::gfx::CModel> wpCurrentModel = m_lstModels[uI];
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
    using namespace render::gfx;
    TCustomPrimitive rData = render::gfx::CPrimitiveUtils::CreateCapsule(_fRadius, _fHeight, _iSubvH, _iSubvV, _eRenderMode);

    // Create temporal item + set pos
    render::gfx::CPrimitive* pPrimitive = m_lstDebugItems.Create(rData, _eRenderMode);

#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set values
    pPrimitive->SetPos(_v3Pos);
    pPrimitive->SetRot(_v3Rot);
    pPrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CScene::DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
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
    pPrimitive->SetPos(_v3Pos);
    pPrimitive->SetRot(_v3Rot);
    pPrimitive->SetScl(_v3Size);
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
    TCustomPrimitive rData = TCustomPrimitive();
    CPrimitiveUtils::CreateSphere(_fRadius, _iSubvH, _iSubvV, rData.PrimitiveData);

    // Fill indices
    rData.Indices = (_eRenderMode == render::ERenderMode::SOLID) ? CPrimitiveUtils::GetSphereIndices(_iSubvH, _iSubvV) :
      CPrimitiveUtils::GetWireframeSphereIndices(_iSubvH, _iSubvV);

    // Create temporal item + set pos
    CPrimitive* pSpherePrimitive = m_lstDebugItems.Create(rData, _eRenderMode);
#ifdef _DEBUG
    assert(pSpherePrimitive); // Sanity check
#endif

    // Set values
    pSpherePrimitive->SetPos(_v3Pos);
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
    using namespace render::gfx;
    TCustomPrimitive rData = render::gfx::CPrimitiveUtils::CreatePlane(_oPlane, _eRenderMode);

    // Create primitive
    CPrimitive* pPlanePrimitive = m_lstDebugItems.Create(rData, _eRenderMode);
#ifdef _DEBUG
    assert(pPlanePrimitive); // Sanity check
#endif

    // Set values
    pPlanePrimitive->SetPos(_oPlane.GetPos());
    pPlanePrimitive->SetScl(_v3Size);
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
    TCustomPrimitive rData = CPrimitiveUtils::CreateLine(_v3Start, _v3Dest);
    // Create temporal item
    CPrimitive* pPrimitive = m_lstDebugItems.Create(rData, render::ERenderMode::WIREFRAME);
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set values
    pPrimitive->SetPos(math::CVector3::Zero);
    pPrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CScene::CacheModels(render::CCamera* _pCamera)
  {
    // Reset value
    m_uDrawableModels = 0;

    for (uint16_t uI = 0; uI < m_lstModels.GetMaxSize(); uI++)
    {
      utils::CWeakPtr<render::gfx::CModel> pModel = m_lstModels[uI];
      if (!pModel.IsValid())
      {
        continue;
      }

      // Handle model
      TCachedModel& rCachedModel = m_lstCachedModels[m_uDrawableModels];
      rCachedModel.Visible = pModel->IsVisible();
      if (rCachedModel.Visible)
      {
        bool bOnFrustum = true;
        if (pModel->IsCullEnabled()) // Check culling
        {
          bOnFrustum = _pCamera->IsOnFrustum(pModel->GetWorldAABB());
        }
        rCachedModel.Visible = bOnFrustum;
      }

      // Handle instances
      rCachedModel.InstanceCount = 0;
      render::gfx::TInstances& lstInstances = pModel->GetInstances();
      for (uint32_t uJ = 0; uJ < lstInstances.GetMaxSize(); uJ++)
      {
        render::gfx::CRenderInstance* pInstance = lstInstances[uJ];
        if (!pInstance || !pInstance->IsVisible())
        {
          continue;
        }

        bool bOnFrustum = true;
        if (pInstance->IsCullEnabled()) // Check culling
        {
          bOnFrustum = _pCamera->IsOnFrustum(pInstance->GetWorldAABB());
        }
        if (bOnFrustum)
        {
          rCachedModel.DrawableInstances[rCachedModel.InstanceCount++] = pInstance->GetInstanceID();
        }
      }

      // Register drawable model
      if (rCachedModel.Visible || rCachedModel.InstanceCount > 0)
      {
        rCachedModel.Index = uI;
        m_uDrawableModels++;
      }
    }
  }
  // ------------------------------------
  void CScene::DrawModels(render::CRender* _pRender)
  {
    // Draw models
    for (uint16_t uI = 0; uI < m_uDrawableModels; uI++)
    {
      // Handle model
      const TCachedModel& rCachedModel = m_lstCachedModels[uI];
      utils::CWeakPtr<render::gfx::CModel> pModel = m_lstModels[rCachedModel.Index];

      // Push buffers
      pModel->PushBuffers();

      // Handle model
      if (rCachedModel.Visible)
      {
        pModel->Draw();
      }

      // Handle instances
      if (rCachedModel.InstanceCount > 0)
      {
        // Push mode
        _pRender->SetInstancingMode(true);

        // Draw instances
        pModel->PushInstances(rCachedModel.DrawableInstances, rCachedModel.InstanceCount);
        pModel->Draw(rCachedModel.InstanceCount);

        // Disabled mode
        _pRender->SetInstancingMode(false);
      }
    }
  }
  // ------------------------------------
  void CScene::DrawPrimitives(render::CCamera* _pCamera)
  {
    // Draw primitives
    for (uint32_t uI = 0; uI < m_lstPrimitives.GetSize(); uI++)
    {
      render::gfx::CPrimitive* pPrimitive = m_lstPrimitives[uI];
      if (!pPrimitive->IsVisible())
      {
        continue;
      }

      bool bDrawPrimitive = true;
      if (pPrimitive->IsCullEnabled()) // Check culling
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

      bool bDrawDebug = true;
      if (pDebugPrimitive->IsCullEnabled()) // Check culling
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
  void CScene::ApplyLighting()
  {
    m_oLightManager.Apply();
  }
  // ------------------------------------
  void CScene::Clear()
  {
    // Models + primitives
    m_lstPrimitives.Clear();
    m_lstModels.Clear();
  }
}
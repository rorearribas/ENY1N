#include "RenderScene.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Render/Lighting/Light.h"
#include "Engine/Render/Lighting/DirectionalLight.h"
#include "Engine/Render/Utils/PrimitiveUtils.h"
#include "Engine/Managers/ResourceManager.h"
#include "Engine/Global/GlobalResources.h"

#include "Libs/Macros/GlobalMacros.h"
#include <cassert>

namespace scene
{
  // ------------------------------------
  HRESULT CRenderScene::Init()
  {
    // Create model buffers
    D3D11_BUFFER_DESC rVertexBufferDesc = D3D11_BUFFER_DESC();
    rVertexBufferDesc.ByteWidth = MAX_MODELS_VB_SIZE;
    rVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    rVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    rVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hResult = global::api::Device->CreateBuffer(&rVertexBufferDesc, nullptr, &m_pModelsVB);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating vertex buffer.");
      return hResult;
    }

    D3D11_BUFFER_DESC rIndexBufferDesc = D3D11_BUFFER_DESC();
    rIndexBufferDesc.ByteWidth = MAX_MODELS_IB_SIZE;
    rIndexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    rIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    rIndexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hResult = global::api::Device->CreateBuffer(&rIndexBufferDesc, nullptr, &m_pModelsIB);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating index buffer.");
      m_pModelsVB->Release();
      m_pModelsVB = nullptr;
      return hResult;
    }

    return hResult;
  }
  // ------------------------------------
  CRenderScene::~CRenderScene()
  {
    Clear();
  }
  // ------------------------------------
  const TCachedModels& CRenderScene::GetCacheModels(uint16_t& _uDrawableCount_) const
  {
    _uDrawableCount_ = m_uDrawableModels;
    return m_lstCachedModels;
  }
  // ------------------------------------
  const scene::TCachedPrimitives& CRenderScene::GetCachedPrimitives(uint16_t& _uDrawableCount_) const
  {
    _uDrawableCount_ = m_uDrawablePrimitives;
    return m_lstCachedPrimitives;
  }
  // ------------------------------------
  const scene::TCachedDebugPrimitives& CRenderScene::GetCachedDebugPrimitives(uint16_t& _uDrawableCount_) const
  {
    _uDrawableCount_ = m_uDrawableDebugPrimitives;
    return m_lstCachedDebugPrimitives;
  }
  // ------------------------------------
  render::gfx::CPrimitive* const CRenderScene::CreatePrimitive(render::EPrimitive _eType, render::ERenderMode _eRenderMode)
  {
    if (m_lstPrimitives.GetSize() >= m_lstPrimitives.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum primitives in the current scene!");
      return nullptr;
    }
    return m_lstPrimitives.Create(_eType, _eRenderMode);
  }
  // ------------------------------------
  bool CRenderScene::DestroyPrimitive(render::gfx::CPrimitive*& _pPrimitive_)
  {
    return m_lstPrimitives.Remove(_pPrimitive_);
  }
  // ------------------------------------
  utils::CWeakPtr<render::gfx::CModel> const CRenderScene::LoadModel(const char* _sModelPath)
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
      render::gfx::TModelData rModelData = pResourceManager->LoadModel(_sModelPath);

      #pragma region VERTEX BUFFER

      uint32_t uVertexCount = static_cast<uint32_t>(rModelData.VertexData.size());
      uint32_t uTargetSize = uVertexCount * sizeof(render::gfx::TVertexData);
      uint32_t uNextOffset = m_uModelsVertexOffset * sizeof(render::gfx::TVertexData) + uTargetSize;

      if (uNextOffset > MAX_MODELS_VB_SIZE)
      {
        ERROR_LOG("There isn't enough memory to create a new model!");
        return utils::CWeakPtr<render::gfx::CModel>();
      }

      // Map buffer
      D3D11_MAPPED_SUBRESOURCE rMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
      HRESULT hResult = global::api::DeviceContext->Map(m_pModelsVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &rMappedSubresource);
      if (FAILED(hResult))
      {
        ERROR_LOG("Error mapping buffer!");
        return utils::CWeakPtr<render::gfx::CModel>();
      }

      uint32_t uStartVertexOffset = m_uModelsVertexOffset;
      render::gfx::TVertexData * pVertexData = static_cast<render::gfx::TVertexData*>(rMappedSubresource.pData);
      render::gfx::TVertexData* pWritePtr = pVertexData + uStartVertexOffset;
      memcpy(pWritePtr, rModelData.VertexData.data(), uTargetSize);

      // Add offset
      m_uModelsVertexOffset += uVertexCount;

      // Unmap
      global::api::DeviceContext->Unmap(m_pModelsVB, 0);

      #pragma endregion

      #pragma region INDEX BUFFER

      for (std::unique_ptr<render::gfx::CMesh>& pMesh : rModelData.Meshes)
      {
        uint32_t uIndices = static_cast<uint32_t>(pMesh->GetIndexCount());
        uTargetSize = uIndices * sizeof(uint32_t);
        uNextOffset = m_uModelsIndexOffset * sizeof(uint32_t) + uTargetSize;

        if (uNextOffset > MAX_MODELS_IB_SIZE)
        {
          ERROR_LOG("There isn't enough memory to create a new model!");
          return utils::CWeakPtr<render::gfx::CModel>();
        }

        // Map buffer
        rMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
        hResult = global::api::DeviceContext->Map(m_pModelsIB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &rMappedSubresource);
        if (FAILED(hResult))
        {
          ERROR_LOG("Error mapping buffer!");
          return utils::CWeakPtr<render::gfx::CModel>();
        }

        uint32_t uStartIndicesOffset = m_uModelsIndexOffset;
        uint32_t* pIndexData = static_cast<uint32_t*>(rMappedSubresource.pData);

        uint32_t* pPtr = pIndexData + uStartIndicesOffset;
        std::vector<uint32_t>& lstIndices = pMesh->GetIndices();
        memcpy(pPtr, lstIndices.data(), uTargetSize);
        lstIndices.clear();

        // Unmap
        global::api::DeviceContext->Unmap(m_pModelsIB, 0);

        // Add offset
        m_uModelsIndexOffset += uIndices;
        pMesh->SetIndexOffset(m_uModelsIndexOffset - uIndices);
      }

      #pragma endregion

      // Create model + set vertex offset
      std::unique_ptr<render::gfx::CModel> pLoadedModel = std::make_unique<render::gfx::CModel>(rModelData);
      pLoadedModel->SetVertexOffset(m_uModelsVertexOffset - uVertexCount);

      SUCCESS_LOG("Created model! -> " << _sModelPath);
      wpModel = m_lstModels.Insert(std::move(pLoadedModel));
    }

    return wpModel;
  }
  // ------------------------------------
  bool CRenderScene::DestroyModel(utils::CWeakPtr<render::gfx::CModel> _wpModel_)
  {
    return m_lstModels.Remove(_wpModel_);
  }
  // ------------------------------------
  render::lights::CDirectionalLight* const CRenderScene::CreateDirectionalLight()
  {
    return m_oLightManager.CreateDirectionalLight();
  }
  // ------------------------------------
  render::lights::CPointLight* const CRenderScene::CreatePointLight()
  {
    return m_oLightManager.CreatePointLight();
  }
  // ------------------------------------
  render::lights::CSpotLight* const CRenderScene::CreateSpotLight()
  {
    return m_oLightManager.CreateSpotLight();
  }
  // ------------------------------------
  bool CRenderScene::DestroyLight(render::lights::CLight*& _pLight_)
  {
    return m_oLightManager.DestroyLight(_pLight_);
  }
  // ------------------------------------
  void CRenderScene::DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color,
    float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode)
  {
    if (m_lstDebugPrimitives.GetSize() >= m_lstDebugPrimitives.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum temporal items in the current scene");
      return;
    }

    // Fill primitive data
    using namespace render::gfx;
    TCustomPrimitive rData = render::gfx::CPrimitiveUtils::CreateCapsule(_fRadius, _fHeight, _iSubvH, _iSubvV, _eRenderMode);

    // Create temporal item + set pos
    render::gfx::CPrimitive* pPrimitive = m_lstDebugPrimitives.Create(rData, _eRenderMode);

#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set values
    pPrimitive->SetPos(_v3Pos);
    pPrimitive->SetRot(_v3Rot);
    pPrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CRenderScene::DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_lstDebugPrimitives.GetSize() >= m_lstDebugPrimitives.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Create cube
    using namespace render::gfx;
    CPrimitive* pPrimitive = m_lstDebugPrimitives.Create(render::EPrimitive::E3D_CUBE, _eRenderMode);
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
  void CRenderScene::DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_lstDebugPrimitives.GetSize() >= m_lstDebugPrimitives.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Fill primitive data
    using namespace render::gfx;
    TCustomPrimitive rData = TCustomPrimitive();
    CPrimitiveUtils::CreateSphere(_fRadius, _iSubvH, _iSubvV, rData.Vertices);

    // Fill indices
    rData.Indices = (_eRenderMode == render::ERenderMode::SOLID) ? CPrimitiveUtils::GetSphereIndices(_iSubvH, _iSubvV) :
      CPrimitiveUtils::GetWireframeSphereIndices(_iSubvH, _iSubvV);

    // Create temporal item + set pos
    CPrimitive* pSpherePrimitive = m_lstDebugPrimitives.Create(rData, _eRenderMode);
#ifdef _DEBUG
    assert(pSpherePrimitive); // Sanity check
#endif

    // Set values
    pSpherePrimitive->SetPos(_v3Pos);
    pSpherePrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CRenderScene::DrawPlane(const math::CPlane& _rPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_lstDebugPrimitives.GetSize() >= m_lstDebugPrimitives.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Create plane
    using namespace render::gfx;
    TCustomPrimitive rData = render::gfx::CPrimitiveUtils::CreatePlane(_rPlane, _eRenderMode);

    // Create primitive
    CPrimitive* pPlanePrimitive = m_lstDebugPrimitives.Create(rData, _eRenderMode);
#ifdef _DEBUG
    assert(pPlanePrimitive); // Sanity check
#endif

    // Set values
    pPlanePrimitive->SetPos(_rPlane.GetPos());
    pPlanePrimitive->SetScl(_v3Size);
    pPlanePrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CRenderScene::DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color)
  {
    if (m_lstDebugPrimitives.GetSize() >= m_lstDebugPrimitives.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Create data
    using namespace render::gfx;
    TCustomPrimitive rData = CPrimitiveUtils::CreateLine(_v3Start, _v3Dest);
    // Create temporal item
    CPrimitive* pPrimitive = m_lstDebugPrimitives.Create(rData, render::ERenderMode::WIREFRAME);
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set values
    pPrimitive->SetPos(math::CVector3::Zero);
    pPrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CRenderScene::CacheModels(render::CCamera* _pCamera)
  {
    // Reset value
    m_uDrawableModels = 0;

    for (uint16_t uI = 0; uI < m_lstModels.GetSize(); uI++)
    {
      utils::CWeakPtr<render::gfx::CModel> pModel = m_lstModels[uI];
      if (!pModel.IsValid())
      {
        continue;
      }

      // Handle model
      TCachedModel& rCachedModel = m_lstCachedModels[m_uDrawableModels];
      rCachedModel.Visible = pModel->IsVisible();

      // Check culling
      if (rCachedModel.Visible && pModel->IsCullEnabled())
      {
        rCachedModel.Visible = _pCamera->IsOnFrustum(pModel->GetWorldAABB());
      }

      // Handle instances
      rCachedModel.InstanceCount = 0;
      render::gfx::TInstances& lstInstances = pModel->GetInstances();
      for (uint32_t uJ = 0; uJ < lstInstances.GetSize(); uJ++)
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
  void CRenderScene::CachePrimitives(render::CCamera* _pCamera)
  {
    // Reset value
    m_uDrawablePrimitives = 0;

    for (uint16_t uI = 0; uI < m_lstPrimitives.GetSize(); uI++)
    {
      // Handle debug primitive
      const render::gfx::CPrimitive* pPrimitive = m_lstPrimitives[uI];
      uint16_t& uCachedIdx = m_lstCachedPrimitives[m_uDrawablePrimitives];

      bool bOnFrustum = true;
      if (pPrimitive->IsCullEnabled()) // Check culling
      {
        bOnFrustum = _pCamera->IsOnFrustum(pPrimitive->GetWorldAABB());
      }

      if (bOnFrustum)
      {
        uCachedIdx = uI;
        m_uDrawablePrimitives++;
      }
    }
  }
  // ------------------------------------
  void CRenderScene::CacheDebugPrimitives(render::CCamera* _pCamera)
  {
    // Reset value
    m_uDrawableDebugPrimitives = 0;

    for (uint16_t uI = 0; uI < m_lstDebugPrimitives.GetSize(); uI++)
    {
      // Handle debug primitive
      const render::gfx::CPrimitive* pDebugPrimitive = m_lstDebugPrimitives[uI];
      uint16_t& uCachedIdx = m_lstCachedDebugPrimitives[m_uDrawableDebugPrimitives];

      bool bOnFrustum = true;
      if (pDebugPrimitive->IsCullEnabled()) // Check culling
      {
        bOnFrustum = _pCamera->IsOnFrustum(pDebugPrimitive->GetWorldAABB());
      }

      if (bOnFrustum)
      {
        uCachedIdx = uI;
        m_uDrawableDebugPrimitives++;
      }
    }
  }
  // ------------------------------------
  void CRenderScene::ApplyLighting()
  {
    m_oLightManager.ApplyLighting();
  }
// ------------------------------------
  void CRenderScene::Clear()
  {
    // Clear buffers
    global::api::SafeRelease(m_pModelsVB);
    global::api::SafeRelease(m_pModelsIB);

    // Flush items
    m_lstModels.Clear();
    m_lstPrimitives.Clear();
    m_lstDebugPrimitives.Clear();
  }
}
#include "RenderScene.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Utils/PrimitiveUtils.h"
#include "Engine/Render/Lighting/Light.h"
#include "Engine/Render/Lighting/DirectionalLight.h"
#include "Engine/Managers/ResourceManager.h"
#include "Engine/Global/GlobalResources.h"

#include "Libs/Macros/GlobalMacros.h"
#include <cassert>

namespace scene
{
  // ------------------------------------
  HRESULT CRenderScene::SetupBuffers()
  {
    // Create vertex buffer by models
    D3D11_BUFFER_DESC rVertexBufferDesc = D3D11_BUFFER_DESC();
    rVertexBufferDesc.ByteWidth = MAX_MODELS_VB_SIZE;
    rVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    rVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    HRESULT hResult = m_oModelsVB.Init(rVertexBufferDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating vertex buffer!");
      return hResult;
    }

    // Create index buffer by models
    D3D11_BUFFER_DESC rIndexBufferDesc = D3D11_BUFFER_DESC();
    rIndexBufferDesc.ByteWidth = MAX_MODELS_IB_SIZE;
    rIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    rIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    hResult = m_oModelsIB.Init(rIndexBufferDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating index buffer!");
      return hResult;
    }

    // Create vertex buffer by primitives
    rVertexBufferDesc.ByteWidth = MAX_PRIMITIVES_VB_SIZE;
    m_oPrimitivesVB.Init(rVertexBufferDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating vertex buffer!");
      return hResult;
    }

    // Create index buffer by primitives
    rIndexBufferDesc.ByteWidth = MAX_PRIMITIVES_IB_SIZE;
    m_oPrimitivesIB.Init(rIndexBufferDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating index buffer!");
      return hResult;
    }

#ifdef _DEBUG
    // Create vertex buffer by debug primitives
    rVertexBufferDesc.ByteWidth = MAX_PRIMITIVES_VB_SIZE;
    m_oDebugPrimitivesVB.Init(rVertexBufferDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating vertex buffer!");
      return hResult;
    }

    // Create index buffer by debug primitives
    rIndexBufferDesc.ByteWidth = MAX_PRIMITIVES_IB_SIZE;
    m_oDebugPrimitivesIB.Init(rIndexBufferDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating index buffer!");
      return hResult;
    }
#endif

    return hResult;
  }
  // ------------------------------------
  CRenderScene::~CRenderScene()
  {
    Clear();
  }
  // ------------------------------------
  void CRenderScene::CacheModels(render::CCamera* _pCamera)
  {
    // Reset value
    m_uDrawableModels = 0;

    for (uint16_t uI = 0; uI < m_lstModels.GetSize(); uI++)
    {
      // Handle model
      utils::CWeakPtr<render::gfx::CModel> pModel = m_lstModels[uI];
      TCachedModel& rCachedModel = m_lstCachedModels[m_uDrawableModels];
      rCachedModel.Visible = pModel->IsVisible();

      // Check culling
      if (rCachedModel.Visible && pModel->IsCullEnabled())
      {
        rCachedModel.Visible = _pCamera->IsOnFrustum(pModel->GetWorldAABB());
      }

      // Handle instances
      rCachedModel.InstanceCount = 0;
      const render::gfx::TInstances& lstInstances = pModel->GetInstances();
      for (uint32_t uJ = 0; uJ < lstInstances.GetSize(); uJ++)
      {
        utils::CWeakPtr<render::gfx::CRenderInstance> pInstance = lstInstances[uJ];
        if (!pInstance->IsVisible())
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
  const TCachedModels& CRenderScene::GetCachedModels(uint16_t& _uDrawableCount_) const
  {
    _uDrawableCount_ = m_uDrawableModels;
    return m_lstCachedModels;
  }
  // ------------------------------------
  void CRenderScene::CachePrimitives(render::CCamera* _pCamera)
  {
    // Reset value
    m_uDrawablePrimitives = 0;

    for (uint16_t uI = 0; uI < m_lstPrimitives.GetSize(); uI++)
    {
      // Handle debug primitive
      utils::CWeakPtr<render::gfx::CPrimitive> pPrimitive = m_lstPrimitives[uI];
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
  const scene::TCachedPrimitives& CRenderScene::GetCachedPrimitives(uint16_t& _uDrawableCount_) const
  {
    _uDrawableCount_ = m_uDrawablePrimitives;
    return m_lstCachedPrimitives;
  }
  // ------------------------------------
  utils::CWeakPtr<render::gfx::CPrimitive> const CRenderScene::CreatePrimitive(render::EPrimitive _eType, render::ERenderMode _eRenderMode)
  {
    if (m_lstPrimitives.GetSize() >= m_lstPrimitives.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum primitives in the current scene!");
      return utils::CWeakPtr<render::gfx::CPrimitive>();
    }

    // Create primitive data
    render::gfx::TPrimitiveData rPrimitiveData = render::gfx::CPrimitiveUtils::CreatePrimitive(_eType, _eRenderMode);

    // Vertex buffer
    CBufferHandler rVtxBufferHandler = CBufferHandler();
    uint32_t uVertexCount = static_cast<uint32_t>(rPrimitiveData.Vertices.size());
    if (!m_oPrimitivesVB.Alloc(rPrimitiveData.Vertices.data(), uVertexCount, rVtxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return utils::CWeakPtr<render::gfx::CPrimitive>();
    }

    // Index buffer
    CBufferHandler rIdxBufferHandler = CBufferHandler();
    uint32_t uIndices = static_cast<uint32_t>(rPrimitiveData.Indices.size());
    if (!m_oPrimitivesIB.Alloc(rPrimitiveData.Indices.data(), uIndices, rIdxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return utils::CWeakPtr<render::gfx::CPrimitive>();
    }

    // Create primitive
    utils::CWeakPtr<render::gfx::CPrimitive> pPrimitive = m_lstPrimitives.Create();
#ifdef _DEBUG
    assert(pPrimitive.IsValid()); // Sanity check
#endif

    // Set AABB
    collision::CAABB rAABB = collision::CAABB();
    collision::ComputeLocalAABB(rPrimitiveData.Vertices, rAABB);
    pPrimitive->SetLocalAABB(rAABB);

    // Setup
    pPrimitive->SetRenderMode(_eRenderMode);
    pPrimitive->SetVtxBufferHandler(rVtxBufferHandler);
    pPrimitive->SetIdxBufferHandler(rIdxBufferHandler);

    return pPrimitive;
  }
  // ------------------------------------
  bool CRenderScene::DestroyPrimitive(utils::CWeakPtr<render::gfx::CPrimitive> _pPrimitive_)
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
    if (wpModel.IsValid() && wpModel->CreateInstance().IsValid())
    {
      LOG("Created Instance! -> " << _sModelPath);
    }
    else
    {
      // Load model
      CResourceManager* pResourceManager = CResourceManager::GetInstance();
      render::gfx::TModelData rModelData = pResourceManager->LoadModel(_sModelPath);

      // Vertex buffer
      CBufferHandler rVtxBufferHandler = CBufferHandler();
      uint32_t uVertexCount = static_cast<uint32_t>(rModelData.VertexData.size());
      render::gfx::TVertexData* pVertexData = rModelData.VertexData.data();
      if (!m_oModelsVB.Alloc(pVertexData, uVertexCount, rVtxBufferHandler))
      {
        return utils::CWeakPtr<render::gfx::CModel>();
      }

      // Index buffer
      for (uint32_t uIdx = 0; uIdx < rModelData.MeshesCount; uIdx++)
      {
        // Allocate
        CBufferHandler rIdxBufferHandler = CBufferHandler();
        uint32_t uIdxCount = static_cast<uint32_t>(rModelData.Indices[uIdx].size());
        uint32_t* pIndices = rModelData.Indices[uIdx].data();
        if (!m_oModelsIB.Alloc(pIndices, uIdxCount, rIdxBufferHandler))
        {
          return utils::CWeakPtr<render::gfx::CModel>();
        }
        // Set idx buffer handler
        render::gfx::CMesh& rMesh = rModelData.Meshes[uIdx];
        rMesh.SetIdxBufferHandler(rIdxBufferHandler);
      }

      // Create model
      wpModel = m_lstModels.Create(rModelData);
      wpModel->SetVtxBufferHandler(rVtxBufferHandler);
      SUCCESS_LOG("Created model! -> " << _sModelPath);
    }

    return wpModel;
  }
  // ------------------------------------
  bool CRenderScene::DestroyModel(utils::CWeakPtr<render::gfx::CModel> _wpModel_)
  {
    if (!_wpModel_.IsValid())
    {
      return false;
    }

    // Get info
    uint32_t uModelIdx = static_cast<uint32_t>(m_lstModels.FindIndex(_wpModel_));
    uint16_t uCurrentSize = static_cast<uint16_t>(m_lstModels.GetSize());

    const CBufferHandler& rVtxBufferHandler = _wpModel_->GetVtxBufferHandler();
    uint32_t uVtxDisplacement = 0;
    m_oModelsVB.Free(rVtxBufferHandler, uVtxDisplacement);

    if (uVtxDisplacement > 0)
    {
      uint32_t uFirstOffset = rVtxBufferHandler.BeginOffset;
      for (uint32_t uI = (uModelIdx + 1); uI < uCurrentSize; uI++)
      {
        utils::CWeakPtr<render::gfx::CModel> pModel = m_lstModels[uI];
        CBufferHandler rBufferHandler = pModel->GetVtxBufferHandler();

        uint32_t uOffsetSize = rBufferHandler.GetOffset();
        rBufferHandler.BeginOffset = uFirstOffset;
        rBufferHandler.EndOffset = rBufferHandler.BeginOffset + uOffsetSize;

        pModel->SetVtxBufferHandler(rBufferHandler);
        uFirstOffset = rBufferHandler.EndOffset;
      }
    }

    // Get meshes
    uint16_t uCount = 0;
    render::gfx::TMeshes& lstMeshes = _wpModel_->GetMeshes(uCount);

    uint32_t uIdxDisplacement = 0;
    size_t tIndex = (uCount - 1);
    render::gfx::TMeshes::reverse_iterator it = lstMeshes.rbegin();
    for (; it != lstMeshes.rend(); ++it, --tIndex)
    {
      const CBufferHandler& rBufferHandler = it->GetIdxBufferHandler();
      if (rBufferHandler.GetOffset() <= 0) continue;

      uint32_t uDisplacement = 0;
      m_oModelsIB.Free(rBufferHandler, uDisplacement);
      uIdxDisplacement += uDisplacement;
    }

    if (uIdxDisplacement > 0)
    {
      for (uint32_t uIdx = (uModelIdx + 1); uIdx < uCurrentSize; uIdx++)
      {
        uCount = 0;
        render::gfx::TMeshes& tmpMeshes = m_lstModels[uIdx]->GetMeshes(uCount);

        tIndex = (uCount - 1);
        it = tmpMeshes.rbegin();

        for (; it != tmpMeshes.rend(); ++it, --tIndex)
        {
          CBufferHandler rIdxBufferHandler = it->GetIdxBufferHandler();
          if (rIdxBufferHandler.GetOffset() <= 0) continue;

          // Apply displacement
          rIdxBufferHandler.BeginOffset -= uIdxDisplacement;
          rIdxBufferHandler.EndOffset -= uIdxDisplacement;
          it->SetIdxBufferHandler(rIdxBufferHandler);
        }
      }
    }

    return m_lstModels.RemoveAt(uModelIdx);
  }
  // ------------------------------------
  utils::CWeakPtr<render::lights::CDirectionalLight> const CRenderScene::CreateDirectionalLight()
  {
    return m_oLightManager.CreateDirectionalLight();
  }
  // ------------------------------------
  utils::CWeakPtr<render::lights::CPointLight> const CRenderScene::CreatePointLight()
  {
    return m_oLightManager.CreatePointLight();
  }
  // ------------------------------------
  utils::CWeakPtr<render::lights::CSpotLight> const CRenderScene::CreateSpotLight()
  {
    return m_oLightManager.CreateSpotLight();
  }
  // ------------------------------------
  bool CRenderScene::DestroyLight(utils::CWeakPtr<render::lights::CLight> _wpLight)
  {
    return m_oLightManager.DestroyLight(_wpLight);
  }
  // ------------------------------------
#ifdef _DEBUG
  void CRenderScene::CacheDebugPrimitives(render::CCamera* _pCamera)
  {
    // Reset value
    m_uDrawableDebugPrimitives = 0;

    for (uint16_t uI = 0; uI < m_lstDebugPrimitives.GetSize(); uI++)
    {
      // Handle debug primitive
      render::gfx::CPrimitive* pDebugPrimitive = m_lstDebugPrimitives[uI];
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
  const scene::TCachedDebugPrimitives& CRenderScene::GetCachedDebugPrimitives(uint16_t& _uDrawableCount_) const
  {
    _uDrawableCount_ = m_uDrawableDebugPrimitives;
    return m_lstCachedDebugPrimitives;
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

    // Create primitive data
    render::gfx::TPrimitiveData rPrimitiveData = render::gfx::CPrimitiveUtils::CreateCapsule(_fRadius, _fHeight, _iSubvH, _iSubvV, _eRenderMode);

    // Vertex buffer
    CBufferHandler rVtxBufferHandler = CBufferHandler();
    uint32_t uVertexCount = static_cast<uint32_t>(rPrimitiveData.Vertices.size());
    if (!m_oDebugPrimitivesVB.Alloc(rPrimitiveData.Vertices.data(), uVertexCount, rVtxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return;
    }

    // Index buffer
    CBufferHandler rIdxBufferHandler = CBufferHandler();
    uint32_t uIndices = static_cast<uint32_t>(rPrimitiveData.Indices.size());
    if (!m_oDebugPrimitivesIB.Alloc(rPrimitiveData.Indices.data(), uIndices, rIdxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return;
    }

    // Create temporal primitive
    render::gfx::CPrimitive* pPrimitive = m_lstDebugPrimitives.Alloc();
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set AABB
    collision::CAABB rAABB = collision::CAABB();
    collision::ComputeLocalAABB(rPrimitiveData.Vertices, rAABB);
    pPrimitive->SetLocalAABB(rAABB);

    // Setup
    pPrimitive->SetRenderMode(_eRenderMode);
    pPrimitive->SetVtxBufferHandler(rVtxBufferHandler);
    pPrimitive->SetIdxBufferHandler(rIdxBufferHandler);

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

    // Create primitive data
    render::gfx::TPrimitiveData rPrimitiveData = render::gfx::CPrimitiveUtils::CreatePrimitive(render::EPrimitive::E3D_CUBE, _eRenderMode);

    // Vertex buffer
    CBufferHandler rVtxBufferHandler = CBufferHandler();
    uint32_t uVertexCount = static_cast<uint32_t>(rPrimitiveData.Vertices.size());
    if (!m_oDebugPrimitivesVB.Alloc(rPrimitiveData.Vertices.data(), uVertexCount, rVtxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return;
    }

    // Index buffer
    CBufferHandler rIdxBufferHandler = CBufferHandler();
    uint32_t uIndices = static_cast<uint32_t>(rPrimitiveData.Indices.size());
    if (!m_oDebugPrimitivesIB.Alloc(rPrimitiveData.Indices.data(), uIndices, rIdxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return;
    }

    // Create temporal primitive
    render::gfx::CPrimitive* pPrimitive = m_lstDebugPrimitives.Alloc();
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set AABB
    collision::CAABB rAABB = collision::CAABB();
    collision::ComputeLocalAABB(rPrimitiveData.Vertices, rAABB);
    pPrimitive->SetLocalAABB(rAABB);

    // Setup
    pPrimitive->SetRenderMode(_eRenderMode);
    pPrimitive->SetVtxBufferHandler(rVtxBufferHandler);
    pPrimitive->SetIdxBufferHandler(rIdxBufferHandler);

    // Set values
    pPrimitive->SetColor(_v3Color);
    pPrimitive->SetPos(_v3Pos);
    pPrimitive->SetRot(_v3Rot);
    pPrimitive->SetScl(_v3Size);
  }
  // ------------------------------------
  void CRenderScene::DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_lstDebugPrimitives.GetSize() >= m_lstDebugPrimitives.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Create primitive data
    render::gfx::TPrimitiveData rPrimitiveData = render::gfx::TPrimitiveData();
    render::gfx::CPrimitiveUtils::CreateSphere(_fRadius, _iSubvH, _iSubvV, rPrimitiveData.Vertices);

    // Fill indices
    rPrimitiveData.Indices = (_eRenderMode == render::ERenderMode::SOLID) ?
      render::gfx::CPrimitiveUtils::GetSphereIndices(_iSubvH, _iSubvV) : render::gfx::CPrimitiveUtils::GetWireframeSphereIndices(_iSubvH, _iSubvV);

    // Vertex buffer
    CBufferHandler rVtxBufferHandler = CBufferHandler();
    uint32_t uVertexCount = static_cast<uint32_t>(rPrimitiveData.Vertices.size());
    if (!m_oDebugPrimitivesVB.Alloc(rPrimitiveData.Vertices.data(), uVertexCount, rVtxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return;
    }

    // Index buffer
    CBufferHandler rIdxBufferHandler = CBufferHandler();
    uint32_t uIndices = static_cast<uint32_t>(rPrimitiveData.Indices.size());
    if (!m_oDebugPrimitivesIB.Alloc(rPrimitiveData.Indices.data(), uIndices, rIdxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return;
    }

    // Create temporal primitive
    render::gfx::CPrimitive* pPrimitive = m_lstDebugPrimitives.Alloc();
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set AABB
    collision::CAABB rAABB = collision::CAABB();
    collision::ComputeLocalAABB(rPrimitiveData.Vertices, rAABB);
    pPrimitive->SetLocalAABB(rAABB);

    // Setup
    pPrimitive->SetRenderMode(_eRenderMode);
    pPrimitive->SetVtxBufferHandler(rVtxBufferHandler);
    pPrimitive->SetIdxBufferHandler(rIdxBufferHandler);

    // Set values
    pPrimitive->SetPos(_v3Pos);
    pPrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CRenderScene::DrawPlane(const math::CPlane& _rPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_lstDebugPrimitives.GetSize() >= m_lstDebugPrimitives.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Create primitive data
    render::gfx::TPrimitiveData rPrimitiveData = render::gfx::CPrimitiveUtils::CreatePlane(_rPlane, _eRenderMode);

    // Vertex buffer
    CBufferHandler rVtxBufferHandler = CBufferHandler();
    uint32_t uVertexCount = static_cast<uint32_t>(rPrimitiveData.Vertices.size());
    if (!m_oDebugPrimitivesVB.Alloc(rPrimitiveData.Vertices.data(), uVertexCount, rVtxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return;
    }

    // Index buffer
    CBufferHandler rIdxBufferHandler = CBufferHandler();
    uint32_t uIndices = static_cast<uint32_t>(rPrimitiveData.Indices.size());
    if (!m_oDebugPrimitivesIB.Alloc(rPrimitiveData.Indices.data(), uIndices, rIdxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return;
    }

    // Create temporal primitive
    render::gfx::CPrimitive* pPrimitive = m_lstDebugPrimitives.Alloc();
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set AABB
    collision::CAABB rAABB = collision::CAABB();
    collision::ComputeLocalAABB(rPrimitiveData.Vertices, rAABB);
    pPrimitive->SetLocalAABB(rAABB);

    // Setup
    pPrimitive->SetRenderMode(_eRenderMode);
    pPrimitive->SetVtxBufferHandler(rVtxBufferHandler);
    pPrimitive->SetIdxBufferHandler(rIdxBufferHandler);

    // Set values
    pPrimitive->SetPos(_rPlane.GetPos());
    pPrimitive->SetScl(_v3Size);
    pPrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CRenderScene::DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color)
  {
    if (m_lstDebugPrimitives.GetSize() >= m_lstDebugPrimitives.GetMaxSize())
    {
      WARNING_LOG("You have reached maximum debug items in the current scene!");
      return;
    }

    // Create primitive data
    render::gfx::TPrimitiveData rPrimitiveData = render::gfx::CPrimitiveUtils::CreateLine(_v3Start, _v3Dest);

    // Vertex buffer
    CBufferHandler rVtxBufferHandler = CBufferHandler();
    uint32_t uVertexCount = static_cast<uint32_t>(rPrimitiveData.Vertices.size());
    if (!m_oDebugPrimitivesVB.Alloc(rPrimitiveData.Vertices.data(), uVertexCount, rVtxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return;
    }

    // Index buffer
    CBufferHandler rIdxBufferHandler = CBufferHandler();
    uint32_t uIndices = static_cast<uint32_t>(rPrimitiveData.Indices.size());
    if (!m_oDebugPrimitivesIB.Alloc(rPrimitiveData.Indices.data(), uIndices, rIdxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return;
    }

    // Create temporal item
    render::gfx::CPrimitive* pPrimitive = m_lstDebugPrimitives.Alloc();
#ifdef _DEBUG
    assert(pPrimitive); // Sanity check
#endif

    // Set AABB
    collision::CAABB rAABB = collision::CAABB();
    collision::ComputeLocalAABB(rPrimitiveData.Vertices, rAABB);
    pPrimitive->SetLocalAABB(rAABB);

    // Setup
    pPrimitive->SetRenderMode(render::ERenderMode::WIREFRAME);
    pPrimitive->SetVtxBufferHandler(rVtxBufferHandler);
    pPrimitive->SetIdxBufferHandler(rIdxBufferHandler);

    // Set values
    pPrimitive->SetPos(math::CVector3::Zero);
    pPrimitive->SetColor(_v3Color);
  }
  // ------------------------------------
  void CRenderScene::ClearDebugItems()
  {
    m_oDebugPrimitivesVB.ResetOffset();
    m_oDebugPrimitivesIB.ResetOffset();
    m_lstDebugPrimitives.Clear();
  }
#endif
  // ------------------------------------
  void CRenderScene::Clear()
  {
    // Clear vertex buffers
    m_oModelsVB.Release();
    m_oPrimitivesVB.Release();
#ifdef _DEBUG
    m_oDebugPrimitivesVB.Release();
#endif

    // Clear index buffers
    m_oModelsIB.Release();
    m_oPrimitivesIB.Release();
#ifdef _DEBUG
    m_oDebugPrimitivesIB.Release();
#endif

    // Flush items
    m_lstModels.Clear();
    m_lstPrimitives.Clear();
#ifdef _DEBUG
    m_lstDebugPrimitives.Clear();
#endif
  }
}
#include "RenderScene.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Render/Lighting/Light.h"
#include "Engine/Render/Lighting/DirectionalLight.h"
#include "Engine/Render/Utils/PrimitiveUtils.h"
#include "Engine/Managers/ResourceManager.h"
#include "Engine/Global/GlobalResources.h"

#include "Libs/Macros/GlobalMacros.h"
#include <cassert>
#include "../Render/Graphics/Primitive.h"

namespace scene
{
  // ------------------------------------
  HRESULT CRenderScene::Init()
  {
    // Create vertex buffer by models
    D3D11_BUFFER_DESC rVertexBufferDesc = D3D11_BUFFER_DESC();
    rVertexBufferDesc.ByteWidth = MAX_MODELS_VB_SIZE;
    rVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    rVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    rVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hResult = m_oModelsVB.Init(rVertexBufferDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating vertex buffer.");
      return hResult;
    }

    // Create index buffer by models
    D3D11_BUFFER_DESC rIndexBufferDesc = D3D11_BUFFER_DESC();
    rIndexBufferDesc.ByteWidth = MAX_MODELS_IB_SIZE;
    rIndexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    rIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    rIndexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hResult = m_oModelsIB.Init(rIndexBufferDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating index buffer.");
      return hResult;
    }

    // Create vertex buffer by primitives
    rVertexBufferDesc.ByteWidth = MAX_PRIMITIVES_VB_SIZE;
    m_oPrimitivesVB.Init(rVertexBufferDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating vertex buffer.");
      return hResult;
    }

    // Create index buffer by primitives
    rIndexBufferDesc.ByteWidth = MAX_PRIMITIVES_IB_SIZE;
    m_oPrimitivesIB.Init(rVertexBufferDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating vertex buffer.");
      return hResult;
    }

    // Create vertex buffer by debug primitives
    rVertexBufferDesc.ByteWidth = MAX_PRIMITIVES_VB_SIZE;
    m_oDebugPrimitivesVB.Init(rVertexBufferDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating vertex buffer.");
      return hResult;
    }

    // Create index buffer by debug primitives
    rIndexBufferDesc.ByteWidth = MAX_PRIMITIVES_IB_SIZE;
    m_oDebugPrimitivesIB.Init(rVertexBufferDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating vertex buffer.");
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

    // Create primitive data
    using namespace render::gfx;
    TPrimitiveData rPrimitiveData = CPrimitiveUtils::CreatePrimitive(_eType, _eRenderMode);

    // Vertex buffer
    CBufferHandler rVtxBufferHandler = CBufferHandler();
    uint32_t uVertexCount = static_cast<uint32_t>(rPrimitiveData.Vertices.size());
    if (!m_oPrimitivesVB.Alloc(rPrimitiveData.Vertices.data(), uVertexCount, rVtxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return nullptr;
    }

    // Index buffer
    CBufferHandler rIdxBufferHandler = CBufferHandler();
    uint32_t uIndices = static_cast<uint32_t>(rPrimitiveData.Indices.size());
    if (!m_oPrimitivesIB.Alloc(rPrimitiveData.Indices.data(), uIndices, rIdxBufferHandler))
    {
      ERROR_LOG("Error allocating memory!");
      return nullptr;
    }

    // Create primitive
    render::gfx::CPrimitive* pPrimitive = m_lstPrimitives.Create();

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
  bool CRenderScene::DestroyPrimitive(render::gfx::CPrimitive*& _pPrimitive_)
  {
    return m_lstPrimitives.Remove(_pPrimitive_);
  }
  // ------------------------------------
  void CRenderScene::ClearDebugItems()
  {
    // @Hack
    m_oDebugPrimitivesVB.Dealloc();
    m_oDebugPrimitivesIB.Dealloc();
    m_lstDebugPrimitives.Clear();
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

      // Vertex buffer
      CBufferHandler rVtxBufferHandler = CBufferHandler();
      uint32_t uVertexCount = static_cast<uint32_t>(rModelData.VertexData.size());
      render::gfx::TVertexData* pVertexData = rModelData.VertexData.data();
      if (!m_oModelsVB.Alloc(pVertexData, uVertexCount, rVtxBufferHandler))
      {
        return utils::CWeakPtr<render::gfx::CModel>();
      }

      // Index buffer
      for (uint32_t uIdx = 0; uIdx < rModelData.Meshes.size(); uIdx++)
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
        std::unique_ptr<render::gfx::CMesh>& pMesh = rModelData.Meshes[uIdx];
        pMesh->SetIdxBufferHandler(rIdxBufferHandler);
      }

      // Create model + set vertex offset
      std::unique_ptr<render::gfx::CModel> pLoadedModel = std::make_unique<render::gfx::CModel>(rModelData);
      pLoadedModel->SetVtxBufferHandler(rVtxBufferHandler);

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

    // Create primitive data
    using namespace render::gfx;
    TPrimitiveData rPrimitiveData = CPrimitiveUtils::CreateCapsule(_fRadius, _fHeight, _iSubvH, _iSubvV, _eRenderMode);

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

    // Create item + configure
    render::gfx::CPrimitive* pPrimitive = m_lstDebugPrimitives.Create();
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
    using namespace render::gfx;
    TPrimitiveData rPrimitiveData = CPrimitiveUtils::CreatePrimitive(render::EPrimitive::E3D_CUBE, _eRenderMode);

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

    // Create cube
    using namespace render::gfx;
    CPrimitive* pPrimitive = m_lstDebugPrimitives.Create();
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

    // Fill primitive data
    using namespace render::gfx;
    TPrimitiveData rPrimitiveData = TPrimitiveData();
    CPrimitiveUtils::CreateSphere(_fRadius, _iSubvH, _iSubvV, rPrimitiveData.Vertices);

    // Fill indices
    rPrimitiveData.Indices = (_eRenderMode == render::ERenderMode::SOLID) ?
      CPrimitiveUtils::GetSphereIndices(_iSubvH, _iSubvV) : CPrimitiveUtils::GetWireframeSphereIndices(_iSubvH, _iSubvV);

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

    // Create temporal item + set pos
    CPrimitive* pPrimitive = m_lstDebugPrimitives.Create();
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

    // Create plane
    using namespace render::gfx;
    TPrimitiveData rPrimitiveData = render::gfx::CPrimitiveUtils::CreatePlane(_rPlane, _eRenderMode);

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

    // Create primitive
    CPrimitive* pPrimitive = m_lstDebugPrimitives.Create();
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

    // Create data
    using namespace render::gfx;
    TPrimitiveData rPrimitiveData = CPrimitiveUtils::CreateLine(_v3Start, _v3Dest);

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
    CPrimitive* pPrimitive = m_lstDebugPrimitives.Create();
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
    // Clear vertex buffers
    m_oModelsVB.Release();
    m_oPrimitivesVB.Release();
    m_oDebugPrimitivesVB.Release();

    // Clear index buffers
    m_oModelsIB.Release();
    m_oPrimitivesIB.Release();
    m_oDebugPrimitivesIB.Release();

    // Flush items
    m_lstModels.Clear();
    m_lstPrimitives.Clear();
    m_lstDebugPrimitives.Clear();
  }
}
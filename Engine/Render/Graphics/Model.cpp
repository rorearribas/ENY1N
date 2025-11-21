#include "Model.h"
#include "Engine/Shaders/Forward/SimpleVS.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Managers/ResourceManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/Math/Math.h"
#include <cassert>

namespace render
{
  namespace gfx
  {
    // ------------------------------------
    CModel::CModel(TModelData& _rModelData)
    {
      HRESULT hResult = InitModel(_rModelData);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));
    }
    // ------------------------------------
    CModel::~CModel()
    {
      Clear();
    }
    // ------------------------------------
    void CModel::Draw()
    {
      // Set buffers
      static const uint32_t uBuffersCount(2);
      ID3D11Buffer* pBuffers[uBuffersCount] = { m_pVertexBuffer, m_pInstanceBuffer };

      uint32_t lstStrides[uBuffersCount] = { sizeof(TVertexData), sizeof(TInstanceData) };
      uint32_t lstOffsets[uBuffersCount] = { 0, 0 };

      global::dx::s_pDeviceContext->IASetVertexBuffers(0, uBuffersCount, pBuffers, lstStrides, lstOffsets);
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Set model matrix
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      pEngine->GetRender()->SetModelMatrix(m_oTransform.GetMatrix());

      // Draw meshes
      for (std::shared_ptr<CMesh>& rMesh : m_lstMeshes)
      {
        rMesh->Draw();
      }

      // Unbind buffers
      ID3D11Buffer* pRemoveBuffers[uBuffersCount] = { nullptr, nullptr };
      global::dx::s_pDeviceContext->IASetVertexBuffers(0, uBuffersCount, pRemoveBuffers, lstStrides, lstOffsets);
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED);
    }
    // ------------------------------------
    void CModel::DrawInstances(const TDrawableInstances& _lstDrawableInstances, uint32_t _uSize)
    {
      D3D11_MAPPED_SUBRESOURCE oMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
      HRESULT hResult = global::dx::s_pDeviceContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedSubresource);
      if (FAILED(hResult))
      {
        ERROR_LOG("Error mapping buffer!");
        return;
      }

      // Set values
      TInstanceData* pInstanceData = static_cast<TInstanceData*>(oMappedSubresource.pData);
      for (uint32_t uIndex = 0; uIndex < _uSize; ++uIndex)
      {
        uint32_t uTargetID = _lstDrawableInstances[uIndex];
        if (render::gfx::CRenderInstance* pInstance = m_lstInstances[uTargetID])
        {
          pInstanceData[uIndex].Transform = pInstance->GetMatrix();
          pInstance->GetWorldBoundingBox().DrawDebug();
        }
      }

      // Unmap
      global::dx::s_pDeviceContext->Unmap(m_pInstanceBuffer, 0);

      // Set buffers
      static const uint32_t uBuffersCount(2);
      ID3D11Buffer* pBuffers[uBuffersCount] = { m_pVertexBuffer, m_pInstanceBuffer };

      uint32_t lstStrides[uBuffersCount] = { sizeof(TVertexData), sizeof(TInstanceData) };
      uint32_t lstOffsets[uBuffersCount] = { 0, 0 };

      // Bind buffers
      global::dx::s_pDeviceContext->IASetVertexBuffers(0, uBuffersCount, pBuffers, lstStrides, lstOffsets);
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Set this model matrix
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      pEngine->GetRender()->SetModelMatrix(m_oTransform.GetMatrix());

      // Draw instancing
      for (std::shared_ptr<CMesh>& rMesh : m_lstMeshes)
      {
        rMesh->Draw(_uSize);
      }

      // Unbind buffers
      ID3D11Buffer* pRemoveBuffers[uBuffersCount] = { nullptr, nullptr };
      global::dx::s_pDeviceContext->IASetVertexBuffers(0, uBuffersCount, pRemoveBuffers, lstStrides, lstOffsets);
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED);
    }
    // ------------------------------------
    void CModel::SetCullingEnabled(bool _bCull)
    {
      // Set state
      if (m_bCullEnabled != _bCull)
      {
        m_bCullEnabled = _bCull;
      }

      // Update bounding box
      if (m_bCullEnabled)
      {
        ComputeWorldAABB(m_oLocalAABB, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CModel::SetPosition(const math::CVector3& _v3Pos)
    {
      // Set pos
      m_oTransform.SetPosition(_v3Pos);

      // Update bounding box
      if (m_bCullEnabled)
      {
        ComputeWorldAABB(m_oLocalAABB, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CModel::SetRotation(const math::CVector3& _v3Rot)
    {
      // Set rot
      m_oTransform.SetRotation(_v3Rot);

      // Update bounding box
      if (m_bCullEnabled)
      {
        ComputeWorldAABB(m_oLocalAABB, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CModel::SetScale(const math::CVector3& _v3Scl)
    {
      // Set scale
      m_oTransform.SetScale(_v3Scl);

      // Update bounding box
      if (m_bCullEnabled)
      {
        ComputeWorldAABB(m_oLocalAABB, m_oWorldAABB);
      }
    }
    // ------------------------------------
    CRenderInstance* CModel::CreateInstance()
    {
      if (!AllowInstancing())
      {
        WARNING_LOG("You have reached maximum instances in this model!");
        return nullptr;
      }

      // Create instance
      uint32_t uInstanceID = m_lstInstances.GetSize();
      return m_lstInstances.Create(this, uInstanceID);
    }
    // ------------------------------------
    bool CModel::RemoveInstance(uint32_t _uID)
    {
      if (render::gfx::CRenderInstance* pInstance = m_lstInstances[_uID])
      {
        return m_lstInstances.Remove(pInstance);
      }
      return false;
    }
    // ------------------------------------
    HRESULT CModel::InitModel(TModelData& _rModelData)
    {
      if (_rModelData.Meshes.empty())
      {
        return E_FAIL;
      }

      // Flush model data
      Clear();

      // Set data
      m_lstMeshes = std::move(_rModelData.Meshes); 
      memcpy(m_sAssetPath, _rModelData.AssetPath, strlen(_rModelData.AssetPath));

      // We create here the vertex buffer
      D3D11_BUFFER_DESC rVertexBufferDesc = D3D11_BUFFER_DESC();
      rVertexBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(TVertexData) * _rModelData.Vertices.size()));
      rVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
      rVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      rVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = D3D11_SUBRESOURCE_DATA();
      oSubresourceData.pSysMem = _rModelData.Vertices.data();
      HRESULT hResult = global::dx::s_pDevice->CreateBuffer(&rVertexBufferDesc, &oSubresourceData, &m_pVertexBuffer);
      if (FAILED(hResult))
      {
        return hResult;
      }

      // We create here the instance buffer
      rVertexBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(TInstanceData) * s_uMaxModelInstances));
      rVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
      rVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      rVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      oSubresourceData.pSysMem = new TInstanceData[s_uMaxModelInstances];
      hResult = global::dx::s_pDevice->CreateBuffer(&rVertexBufferDesc, &oSubresourceData, &m_pInstanceBuffer);
      if (FAILED(hResult))
      {
        return hResult;
      }

      // Calculate local AABB
      ComputeLocalAABB(_rModelData.Vertices, m_oLocalAABB);

      return hResult;
    }
    // ------------------------------------
    void CModel::ComputeWorldAABB(const collision::CBoundingBox& _rLocalAABB, collision::CBoundingBox& _rWorldAABB_)
    {
      math::CVector3 lstExtents[8];
      _rLocalAABB.GetExtents(lstExtents);

      // Compute world AABB
      math::CVector3 v3Min(FLT_MAX, FLT_MAX, FLT_MAX);
      math::CVector3 v3Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

      for (uint32_t uIndex = 0; uIndex < 8; uIndex++)
      {
        // Calculate extent pos
        math::CVector3 v3Extent = m_oTransform.GetMatrix() * lstExtents[uIndex];

        // Calculate Min
        v3Min.x = math::Min(v3Min.x, v3Extent.x);
        v3Min.y = math::Min(v3Min.y, v3Extent.y);
        v3Min.z = math::Min(v3Min.z, v3Extent.z);

        // Calculate Max
        v3Max.x = math::Max(v3Max.x, v3Extent.x);
        v3Max.y = math::Max(v3Max.y, v3Extent.y);
        v3Max.z = math::Max(v3Max.z, v3Extent.z);
      }

      // Set world bounding
      _rWorldAABB_ = collision::CBoundingBox(v3Min, v3Max);
    }
    // ------------------------------------
    void CModel::ComputeLocalAABB(const std::vector<render::gfx::TVertexData>& _lstVertexData, collision::CBoundingBox& _rLocalAABB_)
    {
      if (_lstVertexData.empty())
      {
        return;
      }

      // Compute AABB
      math::CVector3 v3Min(FLT_MAX, FLT_MAX, FLT_MAX);
      math::CVector3 v3Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

      for (auto& rVertexData : _lstVertexData)
      {
        // Calculate Min
        v3Min.x = math::Min(v3Min.x, rVertexData.VertexPos.x);
        v3Min.y = math::Min(v3Min.y, rVertexData.VertexPos.y);
        v3Min.z = math::Min(v3Min.z, rVertexData.VertexPos.z);

        // Calculate Max
        v3Max.x = math::Max(v3Max.x, rVertexData.VertexPos.x);
        v3Max.y = math::Max(v3Max.y, rVertexData.VertexPos.y);
        v3Max.z = math::Max(v3Max.z, rVertexData.VertexPos.z);
      }

      // Set local bounding
      _rLocalAABB_ = collision::CBoundingBox(v3Min, v3Max);
    }
    // ------------------------------------
    void CModel::Clear()
    {
      // Clear buffers
      global::dx::SafeRelease(m_pVertexBuffer);
      global::dx::SafeRelease(m_pInstanceBuffer);

      // Clear model data
      for (auto& pMesh : m_lstMeshes)
      {
        pMesh.reset();
      }
      m_lstMeshes.clear();

      // Clear instances
      m_lstInstances.Clear();
    }
}
}
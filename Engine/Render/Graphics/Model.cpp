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
#ifdef _DEBUG
      assert(!FAILED(hResult));
#endif // DEBUG
    }
    // ------------------------------------
    CModel::~CModel()
    {
      Clear();
    }
    // ------------------------------------
    void CModel::Draw(render::CRender* _pRender, bool _bDrawModel, uint16_t _uInstanceCount)
    {
      // Set values
      uint16_t uInstanceCount = _bDrawModel ? (_uInstanceCount + 1) : _uInstanceCount;
      uint16_t uStartOffset = _bDrawModel ? 0 : 1;

      // Draw meshes
      for (std::unique_ptr<CMesh>& pMesh : m_lstMeshes)
      {
        // Set material info (global buffer)
        _pRender->PushMaterialInfo(pMesh->GetMaterial());

        // Draw process
        pMesh->Draw(uInstanceCount, uStartOffset);
      }
    }
    // ------------------------------------
    void CModel::PushBuffers(const TDrawableInstances& _lstDrawableInstances, uint16_t _uInstanceCount)
    {
      D3D11_MAPPED_SUBRESOURCE rMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
      HRESULT hResult = global::dx::s_pDeviceContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &rMappedSubresource);
      if (FAILED(hResult))
      {
        ERROR_LOG("Error mapping buffer!");
        return;
      }

      // Mapped instance data
      TInstanceData* pInstanceData = static_cast<TInstanceData*>(rMappedSubresource.pData);

      // Set model matrix
      uint16_t uInstance = 0;
      pInstanceData[uInstance++].Transform = GetMatrix();

      // Set instances
      for (uint16_t uJ = 0; uJ < _uInstanceCount; ++uJ)
      {
        uint16_t uID = _lstDrawableInstances[uJ];
        pInstanceData[uInstance++].Transform = m_lstInstances[uID]->GetMatrix();
      }

      // Unmap
      global::dx::s_pDeviceContext->Unmap(m_pInstanceBuffer, 0);

      // Set vertex buffers
      static const uint32_t uBuffersCount(2);
      ID3D11Buffer* pBuffers[uBuffersCount] = { m_pVertexBuffer, m_pInstanceBuffer };

      uint32_t lstStrides[uBuffersCount] = { sizeof(render::gfx::TVertexData), sizeof(TInstanceData) };
      uint32_t lstOffsets[uBuffersCount] = { 0, 0 };
      global::dx::s_pDeviceContext->IASetVertexBuffers(0, uBuffersCount, pBuffers, lstStrides, lstOffsets);

      // Set topology
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
// ------------------------------------
    void CModel::SetPos(const math::CVector3& _v3Pos)
    {
      // Set pos
      m_oTransform.SetPos(_v3Pos);

      // Update bounding box
      if (m_bCullEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CModel::SetRot(const math::CVector3& _v3Rot)
    {
      // Set rot
      m_oTransform.SetRot(_v3Rot);

      // Update bounding box
      if (m_bCullEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CModel::SetScl(const math::CVector3& _v3Scl)
    {
      // Set scale
      m_oTransform.SetScl(_v3Scl);

      // Update bounding box
      if (m_bCullEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CModel::SetCullEnabled(bool _bCull)
    {
      // Set state
      if (m_bCullEnabled != _bCull)
      {
        m_bCullEnabled = _bCull;
      }

      // Update bounding box
      if (m_bCullEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
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
      uint16_t uInstanceID = static_cast<uint16_t>(m_lstInstances.GetSize());
      return m_lstInstances.Create(this, uInstanceID);
    }
    // ------------------------------------
    bool CModel::RemoveInstance(uint16_t _uID)
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

      D3D11_SUBRESOURCE_DATA rSubresourceData = D3D11_SUBRESOURCE_DATA();
      rSubresourceData.pSysMem = _rModelData.Vertices.data();
      HRESULT hResult = global::dx::s_pDevice->CreateBuffer(&rVertexBufferDesc, &rSubresourceData, &m_pVertexBuffer);
      if (FAILED(hResult))
      {
        return hResult;
      }

      // We create the instance buffer (we use an extra slot for the model)
      uint16_t uInstances = (s_uMaxInstancesPerObject + 1);

      rVertexBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(TInstanceData) * uInstances));
      rVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
      rVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      rVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      rSubresourceData.pSysMem = new TInstanceData[uInstances];
      hResult = global::dx::s_pDevice->CreateBuffer(&rVertexBufferDesc, &rSubresourceData, &m_pInstanceBuffer);
      if (FAILED(hResult))
      {
        return hResult;
      }

      // Calculate local AABB
      collision::ComputeLocalAABB(_rModelData.Vertices, m_oLocalAABB);

      return S_OK;
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
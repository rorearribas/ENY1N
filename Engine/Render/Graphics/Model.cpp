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
      bool bOk = false;
      if (render::gfx::CRenderInstance* pInstance = m_lstInstances[_uID])
      {
        bOk = m_lstInstances.Remove(pInstance);
      }
#ifdef _DEBUG
      assert(bOk);
#endif
      // Sort ids
      uint16_t uSize = static_cast<uint16_t>(m_lstInstances.GetSize());
      for (uint16_t uIndex = 0; uIndex < uSize; uIndex++)
      {
        render::gfx::CRenderInstance* pInstance = m_lstInstances[uIndex];
        pInstance->SetInstanceID(uIndex);
      }
      return bOk;
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

      // Calculate local AABB
      collision::ComputeLocalAABB(_rModelData.Vertices, m_oLocalAABB);

      return S_OK;
    }
    // ------------------------------------
    void CModel::Clear()
    {
      // Clear buffers
      global::dx::SafeRelease(m_pVertexBuffer);

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
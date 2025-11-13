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
    bool TVertexData::operator==(const TVertexData& _other) const
    {
      return VertexPos == _other.VertexPos && Normal == _other.Normal && TexCoord == _other.TexCoord;
    }
    // ------------------------------------
    bool TVertexData::operator!=(const TVertexData& _other) const
    {
      return !(*this == _other);
    }
    // ------------------------------------
    CModel::CModel(const TModelData& _rModelData)
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
      for (auto& pMesh : m_oModelData.Meshes)
      {
        pMesh->Draw();
      }

      // Unbind buffers
      ID3D11Buffer* pRemoveBuffers[uBuffersCount] = { nullptr, nullptr };
      global::dx::s_pDeviceContext->IASetVertexBuffers(0, uBuffersCount, pRemoveBuffers, lstStrides, lstOffsets);
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED);
    }
    // ------------------------------------
    void CModel::DrawInstances(const std::vector<uint32_t>& _vctDrawableIds)
    {
      D3D11_MAPPED_SUBRESOURCE oMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
      HRESULT hResult = global::dx::s_pDeviceContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedSubresource);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));

      // Update instance data
      TInstanceData* pInstanceData = static_cast<TInstanceData*>(oMappedSubresource.pData);
      assert(pInstanceData);

      // Set values
      uint32_t uInstanceCount = static_cast<uint32_t>(_vctDrawableIds.size());
      for (uint32_t uIndex = 0; uIndex < uInstanceCount; ++uIndex)
      {
        uint32_t uTargetID = _vctDrawableIds[uIndex];
        pInstanceData[uIndex].Transform = m_lstInstances[uTargetID]->GetMatrix();
      }

      // Unmap
      global::dx::s_pDeviceContext->Unmap(m_pInstanceBuffer, 0);

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

      // Draw instancing
      for (auto& pMesh : m_oModelData.Meshes)
      {
        pMesh->Draw(uInstanceCount);
      }

      // Unbind buffers
      ID3D11Buffer* pRemoveBuffers[uBuffersCount] = { nullptr, nullptr };
      global::dx::s_pDeviceContext->IASetVertexBuffers(0, uBuffersCount, pRemoveBuffers, lstStrides, lstOffsets);
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED);
    }
    // ------------------------------------
    void CModel::SetPosition(const math::CVector3& _v3Pos)
    {
      // Set pos
      m_oTransform.SetPosition(_v3Pos);

      // Update bounding box
      if (m_bCullEnabled)
      {
        ComputeBoundingBox(m_oTransform.GetMatrix(), m_oBoundingBox);
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
        ComputeBoundingBox(m_oTransform.GetMatrix(), m_oBoundingBox);
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
        ComputeBoundingBox(m_oTransform.GetMatrix(), m_oBoundingBox);
      }
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
        ComputeBoundingBox(m_oTransform.GetMatrix(), m_oBoundingBox);
      }
    }
    // ------------------------------------
    HRESULT CModel::InitModel(const TModelData& _rModelData)
    {
      if (_rModelData.Meshes.empty())
      {
        return E_FAIL;
      }

      // Flush model data
      Clear();

      // Set model data
      m_oModelData = _rModelData;

      // We create here the vertex buffer
      D3D11_BUFFER_DESC oVertexBufferDescriptor = D3D11_BUFFER_DESC();
      oVertexBufferDescriptor.ByteWidth = static_cast<uint32_t>((sizeof(TVertexData) * m_oModelData.Vertices.size()));
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = D3D11_SUBRESOURCE_DATA();
      oSubresourceData.pSysMem = m_oModelData.Vertices.data();
      HRESULT hResult = global::dx::s_pDevice->CreateBuffer(&oVertexBufferDescriptor, &oSubresourceData, &m_pVertexBuffer);
      if (FAILED(hResult))
      {
        return hResult;
      }

      // We create here the instance buffer
      oVertexBufferDescriptor.ByteWidth = static_cast<uint32_t>((sizeof(TInstanceData) * s_uMaxInstances));
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      oSubresourceData.pSysMem = new TInstanceData[s_uMaxInstances];
      hResult = global::dx::s_pDevice->CreateBuffer(&oVertexBufferDescriptor, &oSubresourceData, &m_pInstanceBuffer);
      if (FAILED(hResult))
      {
        return hResult;
      }

      // Update bounding box
      if (m_bCullEnabled)
      {
        ComputeBoundingBox(m_oTransform.GetMatrix(), m_oBoundingBox);
      }

      return hResult;
    }
    // ------------------------------------
    void CModel::Clear()
    {
      // Clear buffers
      global::dx::SafeRelease(m_pVertexBuffer);
      global::dx::SafeRelease(m_pInstanceBuffer);

      // Clear meshes data
      for (auto& pMesh : m_oModelData.Meshes)
      {
        pMesh.reset();
      }
      m_oModelData.Meshes.clear();
      m_oModelData.Vertices.clear();
    }
    // ------------------------------------
    void CModel::ComputeBoundingBox(const math::CMatrix4x4& _mTransform, collision::CBoundingBox& _rBoundingBox_) const
    {
      if (m_oModelData.Vertices.empty())
      {
        return;
      }

      // Compute bounding box
      math::CVector3 v3Min(FLT_MAX, FLT_MAX, FLT_MAX);
      math::CVector3 v3Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

      for (auto& oVertexData : m_oModelData.Vertices)
      {
        // Get vertex pos
        math::CVector3 v3VertexPos = _mTransform * oVertexData.VertexPos;

        // Calculate Min
        v3Min.x = math::Min<float>(v3Min.x, v3VertexPos.x);
        v3Min.y = math::Min<float>(v3Min.y, v3VertexPos.y);
        v3Min.z = math::Min<float>(v3Min.z, v3VertexPos.z);

        // Calculate Max
        v3Max.x = math::Max<float>(v3Max.x, v3VertexPos.x);
        v3Max.y = math::Max<float>(v3Max.y, v3VertexPos.y);
        v3Max.z = math::Max<float>(v3Max.z, v3VertexPos.z);
      }

      // Get bounding
      _rBoundingBox_ = collision::CBoundingBox(v3Min, v3Max);
    }
    // ------------------------------------
    CRenderInstance* CModel::CreateInstance()
    {
      if (m_lstInstances.GetCurrentSize() >= m_lstInstances.GetMaxSize())
      {
        WARNING_LOG("You have reached maximum instances in this model!");
        return nullptr;
      }

      // Create instance
      uint32_t uInstanceID = m_lstInstances.GetCurrentSize();
      return m_lstInstances.Create(this, uInstanceID);
    }
    // ------------------------------------
    bool CModel::RemoveInstance(CRenderInstance*& _pRenderInstance_)
    {
      return m_lstInstances.Remove(_pRenderInstance_);
    }
  }
}
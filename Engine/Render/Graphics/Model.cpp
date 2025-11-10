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
    CModel::CModel(const SModelData& _rModelData)
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
      // Set vertex buffer
      uint32_t uVertexStride = sizeof(render::gfx::SVertexData);
      uint32_t uVertexOffset = 0;
      global::dx::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Set model matrix
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      pEngine->GetRender()->SetModelMatrix(m_oTransform.CreateTransform());

      // Draw meshes
      for (auto& pMesh : m_oModelData.Meshes)
      {
        pMesh->Draw();
      }

      m_oBoundingBox.DrawDebug();
    }
    // ------------------------------------
    void CModel::SetPosition(const math::CVector3& _v3Pos)
    {
      // Set pos
      m_oTransform.SetPosition(_v3Pos);

      // Update bounding box
      if (m_bCullEnabled)
      {
        ComputeBoundingBox(m_oTransform.CreateTransform(), m_oBoundingBox);
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
        ComputeBoundingBox(m_oTransform.CreateTransform(), m_oBoundingBox);
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
        ComputeBoundingBox(m_oTransform.CreateTransform(), m_oBoundingBox);
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
        ComputeBoundingBox(m_oTransform.CreateTransform(), m_oBoundingBox);
      }
    }
    // ------------------------------------
    HRESULT CModel::InitModel(const SModelData& _rModelData)
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
      oVertexBufferDescriptor.ByteWidth = static_cast<uint32_t>((sizeof(render::gfx::SVertexData) * m_oModelData.Vertices.size()));
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

      // Update bounding box
      if (m_bCullEnabled)
      {
        ComputeBoundingBox(m_oTransform.CreateTransform(), m_oBoundingBox);
      }

      return hResult;
    }
    // ------------------------------------
    CRenderInstance* CModel::CreateInstance()
    {
      if (m_lstInstances.GetCurrentSize() >= m_lstInstances.GetMaxSize())
      {
        WARNING_LOG("You have reached maximum instances in this model!");
        return nullptr;
      }
      return m_lstInstances.Create(this);
    }
    // ------------------------------------
    void CModel::Clear()
    {
      // Clear vertex buffer
      global::dx::SafeRelease(m_pVertexBuffer);

      // Clear meshes
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
        math::CVector3 v3VertexPos = _mTransform * oVertexData.Position;

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
  }
}
#include "Primitive.h"
#include "Engine/Engine.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Shaders/Forward/SimpleVS.h"
#include "Engine/Render/Utils/PrimitiveUtils.h"

#include "Libs/Macros/GlobalMacros.h"
#include "Libs/Math/Math.h"

#include <cassert>
#include <iostream>

namespace render
{
  namespace gfx
  {
    static constexpr float s_fStandardRadius = 0.5f;
    static constexpr float s_fCapsuleHeight = 2.0f;

    static constexpr uint32_t s_uCircleSegments = 12;
    static constexpr uint32_t s_uSubvH = 12;
    static constexpr uint32_t s_uSubvV = 12;

    CPrimitive::CPrimitive(TCustomPrimitive& _oData, render::ERenderMode _eRenderMode)
    {
      // Create from custom data
      HRESULT hResult = CreateBuffer(_oData.PrimitiveData, _oData.Indices);
      UNUSED_VAR(hResult);
#ifdef _DEBUG
      assert(!FAILED(hResult));
#endif // DEBUG

      // Set values
      m_eRenderMode = _eRenderMode;
      m_ePrimitiveType = EPrimitive::CUSTOM;
    }
    // ------------------------------------
    CPrimitive::CPrimitive(EPrimitive _ePrimitiveType, ERenderMode _eRenderMode)
    {
      // Create buffer from presets
      HRESULT hResult = CreatePrimitive(_ePrimitiveType, _eRenderMode);
      UNUSED_VAR(hResult);
#ifdef _DEBUG
      assert(!FAILED(hResult));
#endif // DEBUG

      // Set values
      m_eRenderMode = _eRenderMode;
      m_ePrimitiveType = _ePrimitiveType;
    }
    // ------------------------------------
    CPrimitive::~CPrimitive()
    {
      Clear();
    }
    // ------------------------------------
    void CPrimitive::Draw()
    {
      // Set vertex buffer
      uint32_t uVertexStride = sizeof(render::gfx::TPrimitiveData);
      uint32_t uVertexOffset = 0;
      global::dx::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);

      // Set topology
      bool bSolid = m_eRenderMode == ERenderMode::SOLID;
      D3D_PRIMITIVE_TOPOLOGY eTopology = bSolid ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(eTopology);

      // Set model matrix
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      pEngine->GetRender()->SetModelMatrix(m_oTransform.GetMatrix());

      // Draw
      global::dx::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
      global::dx::s_pDeviceContext->DrawIndexed(m_uIndices, 0, 0);
    }
    // ------------------------------------
    void CPrimitive::SetPos(const math::CVector3& _v3Pos)
    {
      // Set pos
      m_oTransform.SetPos(_v3Pos);

      // Update bounding box
      if (m_bCullingEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CPrimitive::SetRot(const math::CVector3& _v3Rot)
    {
      // Set rot
      m_oTransform.SetRot(_v3Rot);

      // Update bounding box
      if (m_bCullingEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CPrimitive::SetScl(const math::CVector3& _v3Scl)
    {
      // Set scale
      m_oTransform.SetScl(_v3Scl);

      // Update bounding box
      if (m_bCullingEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CPrimitive::SetCullEnabled(bool _bCull)
    {
      // Set state
      if (m_bCullingEnabled != _bCull)
      {
        m_bCullingEnabled = _bCull;
      }

      // Update bounding box
      if (m_bCullingEnabled)
      {
        collision::ComputeWorldAABB(m_oLocalAABB, m_oTransform, m_oWorldAABB);
      }
    }
    // ------------------------------------
    void CPrimitive::SetRenderMode(render::ERenderMode _eRenderMode)
    {
      if (m_eRenderMode != _eRenderMode)
      {
        m_eRenderMode = _eRenderMode;
        CreatePrimitive(m_ePrimitiveType, m_eRenderMode);
      }
    }
    // ------------------------------------
    void CPrimitive::SetColor(const math::CVector3& _v3Color)
    {
      // Map
      D3D11_MAPPED_SUBRESOURCE rMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
      HRESULT hResult = global::dx::s_pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &rMappedSubresource);
      UNUSED_VAR(hResult);
#ifdef _DEBUG
      assert(!FAILED(hResult));
#endif // DEBUG

      // Get data
      render::gfx::TPrimitiveData* pPrimitiveData = static_cast<render::gfx::TPrimitiveData*>(rMappedSubresource.pData);
#ifdef _DEBUG
      assert(pPrimitiveData);
#endif // DEBUG

      // Update color
      for (uint32_t uIndex = 0; uIndex < m_uVertices; ++uIndex)
      {
        pPrimitiveData[uIndex].Color = _v3Color;
      }

      // Unmap
      global::dx::s_pDeviceContext->Unmap(m_pVertexBuffer, 0);

      // Save color
      m_v3Color = _v3Color;
    }
    // ------------------------------------
    HRESULT CPrimitive::CreatePrimitive(EPrimitive _ePrimitiveType, render::ERenderMode _eRenderMode)
    {
      switch (_ePrimitiveType)
      {
        // 3D Implementation
        case EPrimitive::E3D_CUBE:
        {
          // Create 3D Cube
          return CreateBuffer
          (
            CPrimitiveUtils::s_oCubePrimitive,
            _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::s_oCubeIndices : CPrimitiveUtils::s_oWireframeCubeIndices
          );
        }
        case EPrimitive::E3D_SPHERE:
        {
          // Create sphere
          std::vector<render::gfx::TPrimitiveData> lstPrimitiveData = std::vector<render::gfx::TPrimitiveData>();
          CPrimitiveUtils::CreateSphere(s_fStandardRadius, s_uSubvH, s_uSubvV, lstPrimitiveData);

          // Get indices
          const auto& lstIndices = _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::GetSphereIndices(s_uSubvH, s_uSubvV) :
          CPrimitiveUtils::GetWireframeSphereIndices(s_uSubvH, s_uSubvV);

          // Create buffer
          return CreateBuffer(lstPrimitiveData, lstIndices);
        }
        case EPrimitive::E3D_CAPSULE:
        {
          // Create capsule
          TCustomPrimitive oPrimitiveData = CPrimitiveUtils::CreateCapsule
          (
            s_fStandardRadius,
            s_fCapsuleHeight,
            s_uSubvH,
            s_uSubvV,
            _eRenderMode
          );
          return CreateBuffer(oPrimitiveData.PrimitiveData, oPrimitiveData.Indices);
        }
        case EPrimitive::E3D_PLANE:
        {
          // Create 3D Plane
          return CreateBuffer
          (
            CPrimitiveUtils::s_oPlanePrimitive,
            _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::s_oPlaneIndices : CPrimitiveUtils::s_oWireframePlaneIndices
          );
        }
        // 2D Implementation
        case EPrimitive::E2D_SQUARE:
        {
          // Create 2D Square
          return CreateBuffer
          (
            CPrimitiveUtils::s_oSquarePrimitive,
            _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::s_oSquareIndices : CPrimitiveUtils::s_oSquareWireframeIndices
          );
        }
        case EPrimitive::E2D_CIRCLE:
        {
          // Create 2D Circle
          TCustomPrimitive oPrimitiveData = CPrimitiveUtils::CreateCircle(s_fStandardRadius, s_uCircleSegments, _eRenderMode);
          return CreateBuffer(oPrimitiveData.PrimitiveData, oPrimitiveData.Indices);
        }
        break;
        case EPrimitive::E2D_TRIANGLE:
        {
          // Create 2D Triangle
          return CreateBuffer
          (
            CPrimitiveUtils::s_oTrianglePrimitive,
            _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::s_oTriangleIndices : CPrimitiveUtils::s_oWireframeTriangleIndices
          );
        }
        break;
        break;
      }
      return S_FALSE;
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateBuffer(const std::vector<render::gfx::TPrimitiveData>& _lstPrimitiveData, const std::vector<uint32_t>& _lstIndices)
    {
      if (_lstPrimitiveData.empty() || _lstIndices.empty())
      {
        return E_FAIL;
      }

      // Set vertex data
      m_uVertices = static_cast<uint32_t>(_lstPrimitiveData.size());

      // Config vertex buffer
      D3D11_BUFFER_DESC rVertexBufferDesc = D3D11_BUFFER_DESC();
      rVertexBufferDesc.ByteWidth = static_cast<uint32_t>(sizeof(render::gfx::TPrimitiveData) * _lstPrimitiveData.size());
      rVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
      rVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      rVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      // Create vertex buffer
      D3D11_SUBRESOURCE_DATA oSubresourceData = D3D11_SUBRESOURCE_DATA();
      oSubresourceData.pSysMem = _lstPrimitiveData.data();
      HRESULT hResult = global::dx::s_pDevice->CreateBuffer(&rVertexBufferDesc, &oSubresourceData, &m_pVertexBuffer);
      if (FAILED(hResult))
      {
        return hResult;
      }

      // Config index buffer
      D3D11_BUFFER_DESC rIndexBufferDesc = D3D11_BUFFER_DESC();
      rIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      rIndexBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(uint32_t) * _lstIndices.size()));
      rIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      rIndexBufferDesc.CPUAccessFlags = 0;

      // Set indices size
      m_uIndices = static_cast<uint32_t>(_lstIndices.size());

      // Create index buffer
      D3D11_SUBRESOURCE_DATA rSubresourceData = D3D11_SUBRESOURCE_DATA();
      rSubresourceData.pSysMem = _lstIndices.data();
      hResult = global::dx::s_pDevice->CreateBuffer(&rIndexBufferDesc, &rSubresourceData, &m_pIndexBuffer);
      if (FAILED(hResult))
      {
        return hResult;
      }

      // Create local AABB
      collision::ComputeLocalAABB(_lstPrimitiveData, m_oLocalAABB);

      return S_OK;
    }
    // ------------------------------------
    void CPrimitive::Clear()
    {
      global::dx::SafeRelease(m_pVertexBuffer);
      global::dx::SafeRelease(m_pIndexBuffer);
    }
  }
}
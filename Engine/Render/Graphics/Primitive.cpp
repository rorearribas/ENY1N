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

    static constexpr int s_iSubvH = 12;
    static constexpr int s_iSubvV = 12;

    CPrimitive::CPrimitive(SCustomPrimitive& _oData, render::ERenderMode _eRenderMode)
    {
      // Create from custom data
      HRESULT hResult = CreateBuffer(_oData.m_vctVertexData, _oData.m_vctIndices);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));

      // Set values
      m_eRenderMode = _eRenderMode;
      m_ePrimitiveType = EPrimitiveType::CUSTOM;
    }
    // ------------------------------------
    CPrimitive::CPrimitive(EPrimitiveType _ePrimitiveType, ERenderMode _eRenderMode)
    {
      // Create buffer from presets
      HRESULT hResult = CreatePrimitive(_ePrimitiveType, _eRenderMode);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));

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
    void CPrimitive::Clear()
    {
      global::dx::SafeRelease(m_pVertexBuffer);
      global::dx::SafeRelease(m_pIndexBuffer);
    }
    // ------------------------------------
    void CPrimitive::Draw()
    {
      // Set general data
      uint32_t uVertexStride = sizeof(render::gfx::SVertexData);
      uint32_t uVertexOffset = 0;
      global::dx::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);

      // Set topology
      D3D_PRIMITIVE_TOPOLOGY eTopology = (m_eRenderMode == ERenderMode::SOLID) ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(eTopology);

      // Set model matrix
      math::CMatrix4x4 mModel = m_oTransform.CreateTransform();
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      pEngine->GetRender()->SetModelMatrix(mModel);

      // Draw
      global::dx::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
      global::dx::s_pDeviceContext->DrawIndexed(m_uIndices, 0, 0);
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
      D3D11_MAPPED_SUBRESOURCE oMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
      HRESULT hResult = global::dx::s_pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &oMappedSubresource);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));

      // Get data
      render::gfx::SVertexData* pPrimitiveData = (render::gfx::SVertexData*)(oMappedSubresource.pData);
      assert(pPrimitiveData);

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
    HRESULT CPrimitive::CreatePrimitive(EPrimitiveType _ePrimitiveType, render::ERenderMode _eRenderMode)
    {
      switch (_ePrimitiveType)
      {
        // 2D Implementation
        case EPrimitiveType::E2D_SQUARE:
        {
          // Create 2D Square
          return CreateBuffer
          (
            CPrimitiveUtils::s_oCubePrimitive,
            _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::s_oSquareIndices : CPrimitiveUtils::s_oSquareWireframeIndices
          );
        }
        case EPrimitiveType::E2D_CIRCLE:
        { 
          // Create 2D Circle
          SCustomPrimitive oPrimitiveData = CPrimitiveUtils::Create2DCircle(s_fStandardRadius, s_iSubvH, s_iSubvV, _eRenderMode);
          return CreateBuffer(oPrimitiveData.m_vctVertexData, oPrimitiveData.m_vctIndices);
        }
        break;
        case EPrimitiveType::E2D_TRIANGLE:
        {
          // Create 2D Triangle
          return CreateBuffer
          (
            CPrimitiveUtils::s_oTrianglePrimitive,
            _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::s_oTriangleIndices : CPrimitiveUtils::s_oWireframeTriangleIndices
          );
        }
        break;
        // 3D Implementation
        case EPrimitiveType::E3D_PLANE:
        {
          // Create 3D Plane
          return CreateBuffer
          (
            CPrimitiveUtils::s_oPlanePrimitive,
            _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::s_oPlaneIndices : CPrimitiveUtils::s_oWireframePlaneIndices
          );
        }
        case EPrimitiveType::E3D_CUBE:
        {
          // Create 3D Cube
          return CreateBuffer
          (
            CPrimitiveUtils::s_oCubePrimitive,
            _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::s_oCubeIndices : CPrimitiveUtils::s_oWireframeCubeIndices
          );
        }
        case EPrimitiveType::E3D_SPHERE:
        {
          // Create sphere
          std::vector<render::gfx::SVertexData> vctVertexData = std::vector<render::gfx::SVertexData>();
          CPrimitiveUtils::CreateSphere(s_fStandardRadius, s_iSubvH, s_iSubvV, vctVertexData);
          // Get indices
          const auto& vctIndices = _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::GetSphereIndices(s_iSubvH, s_iSubvV) :
          CPrimitiveUtils::GetWireframeSphereIndices(s_iSubvH, s_iSubvV);
          // Calculate normals
          CPrimitiveUtils::ComputeNormals(vctVertexData, vctIndices);
          // Create buffer
          return CreateBuffer(vctVertexData, vctIndices);
        }
        case EPrimitiveType::E3D_CAPSULE:
        {
          // Create capsule
          SCustomPrimitive oPrimitiveData = CPrimitiveUtils::CreateCapsule
          (
            s_fStandardRadius,
            s_fCapsuleHeight,
            s_iSubvH,
            s_iSubvV,
            _eRenderMode
          );
          CPrimitiveUtils::ComputeNormals(oPrimitiveData.m_vctVertexData, oPrimitiveData.m_vctIndices);
          return CreateBuffer(oPrimitiveData.m_vctVertexData, oPrimitiveData.m_vctIndices);
        }
        break;
      }
      return S_FALSE;
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateBuffer(const std::vector<render::gfx::SVertexData>& _vctVertexData, const std::vector<uint32_t>& _vctIndices)
    {
      if (_vctVertexData.empty() || _vctIndices.empty())
      {
        return E_FAIL;
      }

      // Set vertex data
      m_uVertices = static_cast<uint32_t>(_vctVertexData.size());

      // Config vertex buffer
      D3D11_BUFFER_DESC oVertexBufferDesc = D3D11_BUFFER_DESC();
      oVertexBufferDesc.ByteWidth = static_cast<uint32_t>(sizeof(render::gfx::SVertexData) * _vctVertexData.size());
      oVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      // Create vertex buffer
      D3D11_SUBRESOURCE_DATA oSubresourceData = D3D11_SUBRESOURCE_DATA();
      oSubresourceData.pSysMem = _vctVertexData.data();
      HRESULT hResult = global::dx::s_pDevice->CreateBuffer(&oVertexBufferDesc, &oSubresourceData, &m_pVertexBuffer);
      if (FAILED(hResult))
      {
        return hResult;
      }

      // Config index buffer
      D3D11_BUFFER_DESC oIndexBufferDesc = D3D11_BUFFER_DESC();
      oIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      oIndexBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(uint32_t) * _vctIndices.size()));
      oIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      oIndexBufferDesc.CPUAccessFlags = 0;

      // Set indices size
      m_uIndices = static_cast<uint32_t>(_vctIndices.size());

      // Create index buffer
      D3D11_SUBRESOURCE_DATA oSubresourceIndexesData = D3D11_SUBRESOURCE_DATA();
      oSubresourceIndexesData.pSysMem = _vctIndices.data();
      hResult = global::dx::s_pDevice->CreateBuffer(&oIndexBufferDesc, &oSubresourceIndexesData, &m_pIndexBuffer);
      if (FAILED(hResult))
      {
        return hResult;
      }

      // Update bounding box
      return CalculateBoundingBox();
    }
    // ------------------------------------
    HRESULT CPrimitive::CalculateBoundingBox()
    {
      if (m_uVertices == 0)
      {
        return E_FAIL;
      }

      D3D11_MAPPED_SUBRESOURCE oMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
      HRESULT hResult = global::dx::s_pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &oMappedSubresource);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));

      // Get data
      render::gfx::SVertexData* pPrimitiveData = (render::gfx::SVertexData*)(oMappedSubresource.pData);
      assert(pPrimitiveData);

      // Compute bounding box
      const math::CMatrix4x4 mTransform = m_oTransform.CreateTransform();
      math::CVector3 v3Min(FLT_MAX, FLT_MAX, FLT_MAX);
      math::CVector3 v3Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

      for (uint32_t uIndex = 0; uIndex < m_uVertices; ++uIndex)
      {
        // Get vertex pos
        math::CVector3 v3VertexPos = mTransform * pPrimitiveData[uIndex].Position;

        // Calculate Min
        v3Min.x = math::Min<float>(v3Min.x, v3VertexPos.x);
        v3Min.y = math::Min<float>(v3Min.y, v3VertexPos.y);
        v3Min.z = math::Min<float>(v3Min.z, v3VertexPos.z);

        // Calculate Max
        v3Max.x = math::Max<float>(v3Max.x, v3VertexPos.x);
        v3Max.y = math::Max<float>(v3Max.y, v3VertexPos.y);
        v3Max.z = math::Max<float>(v3Max.z, v3VertexPos.z);
      }

      // Apply min-max
      m_oBoundingBox.SetMin(v3Min);
      m_oBoundingBox.SetMax(v3Max);

      return S_OK;
    }
  }
}
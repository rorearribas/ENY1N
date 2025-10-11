#include "Primitive.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Shaders/Pipeline/StandardVS.h"
#include <cassert>
#include <iostream>
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/Math/Math.h"
#include "PrimitiveUtils.h"

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
      HRESULT hResult = CreateInputLayout();
      assert(!FAILED(hResult));

      // Create from custom data
      hResult = CreateBufferFromData(_oData.m_vctVertexData, _oData.m_vctIndices);
      assert(!FAILED(hResult));

      // Create constant model data buffer
      m_oConstantModelData.Init(global::dx::s_pDevice, global::dx::s_pDeviceContext);

      // Set values
      m_eRenderMode = _eRenderMode;
      m_eType = EPrimitiveType::CUSTOM;
    }
    // ------------------------------------
    CPrimitive::CPrimitive(EPrimitiveType _ePrimitiveType, ERenderMode _eRenderMode)
    {
      HRESULT hResult = CreateInputLayout();
      assert(!FAILED(hResult));

      // Create buffer from presets
      hResult = CreatePrimitive(_ePrimitiveType, _eRenderMode);
      assert(!FAILED(hResult));

      // Create constant model data buffer
      m_oConstantModelData.Init(global::dx::s_pDevice, global::dx::s_pDeviceContext);

      // Set values
      m_eRenderMode = _eRenderMode;
      m_eType = _ePrimitiveType;
    }
    // ------------------------------------
    CPrimitive::~CPrimitive()
    {
      // Release constant buffers
      m_oConstantBuffer.CleanBuffer();
      m_oConstantModelData.CleanBuffer();

      // Release dx
      global::dx::SafeRelease(m_pVertexBuffer);
      global::dx::SafeRelease(m_pIndexBuffer);
      global::dx::SafeRelease(m_pInputLayout);
    }
    // ------------------------------------
    void CPrimitive::Draw()
    {
      // Set general data
      uint32_t uVertexStride = sizeof(render::gfx::SVertexData);
      uint32_t uVertexOffset = 0;
      global::dx::s_pDeviceContext->IASetInputLayout(m_pInputLayout);
      global::dx::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);

      // Set topology
      D3D_PRIMITIVE_TOPOLOGY eTopology = (m_eRenderMode == ERenderMode::SOLID) ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
      global::dx::s_pDeviceContext->IASetPrimitiveTopology(eTopology);

      // Set model matrix
      m_oConstantBuffer.GetData().Matrix = m_oTransform.ComputeModelMatrix();
      bool bOk = m_oConstantBuffer.WriteBuffer();
      assert(bOk);

      // Apply constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
      global::dx::s_pDeviceContext->VSSetConstantBuffers(1, 1, &pConstantBuffer);

      // Set constants
      m_oConstantModelData.GetData().IgnoreGlobalLighting = m_bIgnoreGlobalLighting;
      m_oConstantModelData.GetData().HasTexture = false;
      bOk = m_oConstantModelData.WriteBuffer();
      assert(bOk);

      // Apply constant buffer
      pConstantBuffer = m_oConstantModelData.GetBuffer();
      global::dx::s_pDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);

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
        CreatePrimitive(m_eType, m_eRenderMode);
      }
    }
    // ------------------------------------
    void CPrimitive::IgnoreGlobalLighting(bool _bState)
    {
      m_bIgnoreGlobalLighting = _bState;
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
        return CreateBufferFromData
        (
          CPrimitiveUtils::s_oCubePrimitive,
          _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::s_oSquareIndices : CPrimitiveUtils::s_oSquareWireframeIndices
        );
      }
      case EPrimitiveType::E2D_CIRCLE:
      {
        SCustomPrimitive oPrimitiveData = CPrimitiveUtils::Create2DCircle(s_fStandardRadius, s_iSubvH, s_iSubvV, _eRenderMode);
        return CreateBufferFromData(oPrimitiveData.m_vctVertexData, oPrimitiveData.m_vctIndices);
      }
      break;
      case EPrimitiveType::E2D_TRIANGLE:
      {
        return CreateBufferFromData
        (
          CPrimitiveUtils::s_oTrianglePrimitive,
          _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::s_oTriangleIndices : CPrimitiveUtils::s_oWireframeTriangleIndices
        );
      }
      break;
      // 3D Implementation
      case EPrimitiveType::E3D_PLANE:
      {
        return CreateBufferFromData
        (
          CPrimitiveUtils::s_oPlanePrimitive,
          _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::s_oPlaneIndices : CPrimitiveUtils::s_oWireframePlaneIndices
        );
      }
      case EPrimitiveType::E3D_CUBE:
      {
        return CreateBufferFromData
        (
          CPrimitiveUtils::s_oCubePrimitive,
          _eRenderMode == (render::ERenderMode::SOLID) ? CPrimitiveUtils::s_oCubeIndices : CPrimitiveUtils::s_oWireframeCubeIndices
        );
      }
      case EPrimitiveType::E3D_SPHERE:
      {
        std::vector<render::gfx::SVertexData> vctVertexData = std::vector<render::gfx::SVertexData>();
        CPrimitiveUtils::CreateSphere(s_fStandardRadius, s_iSubvH, s_iSubvV, vctVertexData);

        const auto& vctIndices = _eRenderMode == (render::ERenderMode::SOLID) ?
          CPrimitiveUtils::GetSphereIndices(s_iSubvH, s_iSubvV) :
          CPrimitiveUtils::GetWireframeSphereIndices(s_iSubvH, s_iSubvV);
        CPrimitiveUtils::ComputeNormals(vctVertexData, vctIndices);

        return CreateBufferFromData(vctVertexData, vctIndices);
      }
      case EPrimitiveType::E3D_CAPSULE:
      {
        SCustomPrimitive oPrimitiveData = CPrimitiveUtils::CreateCapsule
        (
          s_fStandardRadius,
          s_fCapsuleHeight,
          s_iSubvH,
          s_iSubvV,
          _eRenderMode
        );
        CPrimitiveUtils::ComputeNormals(oPrimitiveData.m_vctVertexData, oPrimitiveData.m_vctIndices);
        return CreateBufferFromData(oPrimitiveData.m_vctVertexData, oPrimitiveData.m_vctIndices);
      }
      break;
      }

      return S_FALSE;
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateBufferFromData(const std::vector<render::gfx::SVertexData>& _vctVertexData, const std::vector<uint32_t>& _vctIndices)
    {
      if (_vctVertexData.empty() || _vctIndices.empty()) 
      {
        return E_FAIL;
      }

      // Clean buffer
      m_oConstantBuffer.CleanBuffer();

      // Create constant buffer
      m_oConstantBuffer.Init(global::dx::s_pDevice, global::dx::s_pDeviceContext);

      // Set number of vertices to draw
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
      return global::dx::s_pDevice->CreateBuffer(&oIndexBufferDesc, &oSubresourceIndexesData, &m_pIndexBuffer);
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateInputLayout()
    {
      return global::dx::s_pDevice->CreateInputLayout
      (
        render::gfx::SVertexData::s_vctInputElementDesc.data(),
        static_cast<uint32_t>(render::gfx::SVertexData::s_vctInputElementDesc.size()),
        g_StandardVS,
        sizeof(g_StandardVS),
        &m_pInputLayout
      );
    }
  }
}
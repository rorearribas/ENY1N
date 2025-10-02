#include "Primitive.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Shaders/Standard/ForwardVertexShader.h"
#include <cassert>
#include <iostream>
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/Math/Math.h"
#include "PrimitiveUtils.h"

namespace render
{
  namespace graphics
  {
    static constexpr float s_fStandardRadius = 0.5f;
    static constexpr float s_fCapsuleHeight = 2.0f;

    static constexpr int s_iSubvH = 12;
    static constexpr int s_iSubvV = 12;

    CPrimitive::CPrimitive(const SCustomPrimitive& _oVertexData, render::ERenderMode _eRenderMode)
    {
      HRESULT hResult = CreateInputLayout();
      assert(!FAILED(hResult));

      // Create from custom data
      hResult = CreateBufferFromPrimitiveData(_oVertexData.m_vctVertexData, _oVertexData.m_vctIndices);
      assert(!FAILED(hResult));

      // Create constant model data buffer
      m_oConstantModelData.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);

      // Set values
      m_eRenderMode = _eRenderMode;
      m_ePrimitiveType = EPrimitiveType::CUSTOM;
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
      m_oConstantModelData.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);

      // Set values
      m_eRenderMode = _eRenderMode;
      m_ePrimitiveType = _ePrimitiveType;
    }
    // ------------------------------------
    CPrimitive::~CPrimitive()
    {
      // Release constant buffers
      m_oConstantBuffer.CleanBuffer();
      m_oConstantModelData.CleanBuffer();

      // Release dx
      global::dx11::SafeRelease(m_pVertexBuffer);
      global::dx11::SafeRelease(m_pIndexBuffer);
      global::dx11::SafeRelease(m_pInputLayout);
    }
    // ------------------------------------
    HRESULT CPrimitive::CreatePrimitive(EPrimitiveType _ePrimitiveType, render::ERenderMode _eRenderMode)
    {
      switch (_ePrimitiveType)
      {
        // 2D Implementation
      case EPrimitiveType::E2D_SQUARE:
      {
        return CreateBufferFromPrimitiveData
        (
          CPrimitiveUtils::s_oCubePrimitive,
          _eRenderMode == SOLID ? CPrimitiveUtils::s_oSquareIndices : CPrimitiveUtils::s_oSquareWireframeIndices
        );
      }
      case EPrimitiveType::E2D_CIRCLE:
      {
        SCustomPrimitive oPrimitiveData = CPrimitiveUtils::Create2DCircle(s_fStandardRadius, s_iSubvH, s_iSubvV, _eRenderMode);
        return CreateBufferFromPrimitiveData(oPrimitiveData.m_vctVertexData, oPrimitiveData.m_vctIndices);
      }
      break;
      case EPrimitiveType::E2D_TRIANGLE:
      {
        return CreateBufferFromPrimitiveData
        (
          CPrimitiveUtils::s_oTrianglePrimitive,
          _eRenderMode == SOLID ? CPrimitiveUtils::s_oTriangleIndices : CPrimitiveUtils::s_oWireframeTriangleIndices
        );
      }
      break;
      // 3D Implementation
      case EPrimitiveType::E3D_PLANE:
      {
        return CreateBufferFromPrimitiveData
        (
          CPrimitiveUtils::s_oPlanePrimitive,
          _eRenderMode == SOLID ? CPrimitiveUtils::s_oPlaneIndices : CPrimitiveUtils::s_oWireframePlaneIndices
        );
      }
      case EPrimitiveType::E3D_CUBE:
      {
        return CreateBufferFromPrimitiveData
        (
          CPrimitiveUtils::s_oCubePrimitive,
          _eRenderMode == SOLID ? CPrimitiveUtils::s_oCubeIndices : CPrimitiveUtils::s_oWireframeCubeIndices
        );
      }
      case EPrimitiveType::E3D_SPHERE:
      {
        std::vector<render::graphics::SVertexData> vctVertexData = std::vector<render::graphics::SVertexData>();
        CPrimitiveUtils::CreateSphere(s_fStandardRadius, s_iSubvH, s_iSubvV, vctVertexData);

        const auto& vctIndices = _eRenderMode == SOLID ?
          CPrimitiveUtils::GetSphereIndices(s_iSubvH, s_iSubvV) :
          CPrimitiveUtils::GetWireframeSphereIndices(s_iSubvH, s_iSubvV);
        CPrimitiveUtils::ComputeNormals(vctVertexData, vctIndices);

        return CreateBufferFromPrimitiveData(vctVertexData, vctIndices);
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
        return CreateBufferFromPrimitiveData(oPrimitiveData.m_vctVertexData, oPrimitiveData.m_vctIndices);
      }
      break;
      }

      return S_FALSE;
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateInputLayout()
    {
      return global::dx11::s_pDevice->CreateInputLayout
      (
        render::graphics::SVertexData::s_vctInputElementDesc.data(),
        static_cast<uint32_t>(render::graphics::SVertexData::s_vctInputElementDesc.size()),
        g_ForwardVertexShader,
        sizeof(g_ForwardVertexShader),
        &m_pInputLayout
      );
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
    void CPrimitive::UseGlobalLighting(bool _bEnabled)
    {
      m_bUseGlobalLighting = _bEnabled;
    }
    // ------------------------------------
    void CPrimitive::SetColor(const math::CVector3& _v3Color)
    {
      // Map
      D3D11_MAPPED_SUBRESOURCE oMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
      HRESULT hResult = global::dx11::s_pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &oMappedSubresource);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));

      // Get data
      render::graphics::SVertexData* pPrimitiveData = (render::graphics::SVertexData*)(oMappedSubresource.pData);
      assert(pPrimitiveData);

      // Update color
      for (uint32_t uIndex = 0; uIndex < m_uVertices; ++uIndex)
      {
        pPrimitiveData[uIndex].Color = _v3Color;
      }

      // Unmap
      global::dx11::s_pDeviceContext->Unmap(m_pVertexBuffer, 0);

      // Save color
      m_v3Color = _v3Color;
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateBufferFromPrimitiveData(const std::vector<render::graphics::SVertexData>& _vctPrimitiveData, const std::vector<uint32_t>& _vctIndexes)
    {
      if (_vctPrimitiveData.empty()) return E_FAIL;

      // Clean buffer
      m_oConstantBuffer.CleanBuffer();

      // Create constant buffer
      m_oConstantBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);

      // Create vertex buffer
      D3D11_BUFFER_DESC oVertexBufferDescriptor = D3D11_BUFFER_DESC();
      oVertexBufferDescriptor.ByteWidth = static_cast<uint32_t>(sizeof(render::graphics::SVertexData) * _vctPrimitiveData.size());
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = { 0 };
      oSubresourceData.pSysMem = _vctPrimitiveData.data();
      m_uVertices = static_cast<uint32_t>(_vctPrimitiveData.size());

      HRESULT hResult = global::dx11::s_pDevice->CreateBuffer
      (
        &oVertexBufferDescriptor,
        &oSubresourceData,
        &m_pVertexBuffer
      );
      if (_vctIndexes.empty()) return hResult;

      // Config index buffer
      D3D11_BUFFER_DESC oIndexBufferDesc = D3D11_BUFFER_DESC();
      oIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      oIndexBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(uint32_t) * _vctIndexes.size()));
      oIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      oIndexBufferDesc.CPUAccessFlags = 0;
      m_uIndices = static_cast<uint32_t>(_vctIndexes.size());

      D3D11_SUBRESOURCE_DATA oSubresourceIndexesData = {};
      oSubresourceIndexesData.pSysMem = _vctIndexes.data();

      return global::dx11::s_pDevice->CreateBuffer(&oIndexBufferDesc, &oSubresourceIndexesData, &m_pIndexBuffer);
    }
    // ------------------------------------
    void CPrimitive::DrawPrimitive()
    {
      // Set general data
      uint32_t uVertexStride = sizeof(render::graphics::SVertexData);
      uint32_t uVertexOffset = 0;
      global::dx11::s_pDeviceContext->IASetInputLayout(m_pInputLayout);
      global::dx11::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);

      // Set topology
      D3D_PRIMITIVE_TOPOLOGY ePrimitiveTopology = m_eRenderMode == ERenderMode::SOLID ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
      global::dx11::s_pDeviceContext->IASetPrimitiveTopology(ePrimitiveTopology);

      // Set model matrix
      m_oConstantBuffer.GetData().Matrix = m_oTransform.ComputeModelMatrix();
      bool bOk = m_oConstantBuffer.WriteBuffer();
      assert(bOk);

      // Apply constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
      global::dx11::s_pDeviceContext->VSSetConstantBuffers(1, 1, &pConstantBuffer);

      // Set constants
      m_oConstantModelData.GetData().bUseGlobalLighting = m_bUseGlobalLighting;
      m_oConstantModelData.GetData().bHasTexture = false;
      m_oConstantModelData.GetData().bHasModel = false;
      bOk = m_oConstantModelData.WriteBuffer();
      assert(bOk);

      // Apply constant buffer
      pConstantBuffer = m_oConstantModelData.GetBuffer();
      global::dx11::s_pDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);

      // Draw
      global::dx11::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
      global::dx11::s_pDeviceContext->DrawIndexed(m_uIndices, 0, 0);
    }
  }
}
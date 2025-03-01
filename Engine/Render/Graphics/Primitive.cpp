#include "Primitive.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Shaders/Primitive/VertexShader.h"
#include <cassert>
#include <iostream>
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace graphics
  {
    namespace internal_primitive
    {
      // Triangle Primitive
      static const std::vector<CPrimitive::SPrimitiveInfo> s_oTrianglePrimitive =
      {
        { maths::CVector3(0, 1.0f, 0.0f),   maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(1.0f, -1.0f,  0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(-1.0f, -1.0f,  0.0f), maths::CVector3(1.0f, 1.0f, 1.0f) },
      };
      // Square Primitive
      static const std::vector<CPrimitive::SPrimitiveInfo> s_oSquarePrimitive =
      {
        { maths::CVector3(-0.5f, -0.5f, -0.5f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(-0.5f,  0.5f, -0.5f), maths::CVector3(0.0f, 1.0f, 0.0f) },
        { maths::CVector3(0.5f,  0.5f, -0.5f), maths::CVector3(0.0f, 0.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f, -0.5f), maths::CVector3(1.0f, 1.0f, 1.0f) },

        { maths::CVector3(-0.5f, -0.5f,  0.5f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(-0.5f,  0.5f,  0.5f), maths::CVector3(0.0f, 1.0f, 0.0f) },
        { maths::CVector3(0.5f,  0.5f,  0.5f), maths::CVector3(0.0f, 0.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f,  0.5f), maths::CVector3(1.0f, 1.0f, 1.0f) }
      };


      // 2D Square Indices
      static const std::vector<uint32_t> s_oSquareIndices =
      {
        0, 1, 2, // FRONT
        0, 2, 3, // FRONT
      };
      static const std::vector<uint32_t> s_oSquareWireframeIndices =
      {
        0, 1, 1, 2, 2, 3, 3, 0, // FRONT FACE
      };

      // 2D Triangle Indices
      static const std::vector<uint32_t> s_oTriangleIndices =
      {
        0, 1, 2, // FRONT
      };
      static const std::vector<uint32_t> s_oTriangleWireframeIndices =
      {
        0, 1, // Line 1
        1, 2, // Line 2
        2, 0  // Line 3
      };

      // 3D Cube Indices
      static const std::vector<uint32_t> s_oCubeIndices =
      {
        0, 1, 2, //FRONT
        0, 2, 3, //FRONT
        4, 7, 6, //BACK 
        4, 6, 5, //BACK
        3, 2, 6, //RIGHT SIDE
        3, 6, 7, //RIGHT SIDE
        4, 5, 1, //LEFT SIDE
        4, 1, 0, //LEFT SIDE
        1, 5, 6, //TOP
        1, 6, 2, //TOP
        0, 3, 7, //BOTTOM
        0, 7, 4, //BOTTOM
      };
      static const std::vector<uint32_t> s_oCubeWireframeIndices =
      {
        0, 1, 1, 2, 2, 3, 3, 0, // FRONT FACE
        4, 5, 5, 6, 6, 7, 7, 4, // BACK FACE
        0, 4, 1, 5, 2, 6, 3, 7  // CONNECTING EDGES
      };
    }

    // ------------------------------------
    CPrimitive::CPrimitive(EPrimitiveType _ePrimitiveType, ERenderMode _eRenderMode)
    {
      HRESULT hr = CreateInputLayout();
      assert(!FAILED(hr));

      // Create buffer from presets
      hr = CreatePrimitive(_ePrimitiveType, _eRenderMode);
      assert(!FAILED(hr));

      // Set values
      m_eRenderMode = _eRenderMode;
      m_ePrimitiveType = _ePrimitiveType;
    }
    // ------------------------------------
    CPrimitive::~CPrimitive()
    {
      m_oConstantBuffer.CleanBuffer();
      global::dx11::SafeRelease(m_pVertexBuffer);
      global::dx11::SafeRelease(m_pInputLayout);
    }
    // ------------------------------------
    HRESULT CPrimitive::CreatePrimitive(EPrimitiveType _ePrimitiveType, render::ERenderMode _eRenderMode)
    {
      switch (_ePrimitiveType)
      {
      case EPrimitiveType::E2D_SQUARE:
      {
        return CreateBufferFromVertexData
        (
          internal_primitive::s_oSquarePrimitive,
          _eRenderMode == SOLID ? internal_primitive::s_oSquareIndices : internal_primitive::s_oSquareWireframeIndices
        );
      }
      break;
      case EPrimitiveType::E2D_TRIANGLE:
      {
        return CreateBufferFromVertexData
        (
          internal_primitive::s_oTrianglePrimitive,
          _eRenderMode == SOLID ? internal_primitive::s_oTriangleIndices : internal_primitive::s_oTriangleWireframeIndices
        );
      }
      break;
      case EPrimitiveType::E3D_CUBE:
      {
        return CreateBufferFromVertexData
        (
          internal_primitive::s_oSquarePrimitive,
          _eRenderMode == SOLID ? internal_primitive::s_oCubeIndices : internal_primitive::s_oCubeWireframeIndices
        );
      }
      break;
      }

      return S_FALSE;
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateInputLayout()
    {
      D3D11_INPUT_ELEMENT_DESC oInputElementDesc[] =
      {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SPrimitiveInfo, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SPrimitiveInfo, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
      };

      return global::dx11::s_pDevice->CreateInputLayout
      (
        oInputElementDesc,
        ARRAYSIZE(oInputElementDesc),
        g_VertexShader,
        sizeof(g_VertexShader),
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
    void CPrimitive::SetColor(const maths::CVector3& _v3Color)
    {
      // Map
      D3D11_MAPPED_SUBRESOURCE oMappedSubresource;
      HRESULT hr = global::dx11::s_pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &oMappedSubresource);
      UNUSED_VARIABLE(hr);
      assert(!FAILED(hr));

      CPrimitive::SPrimitiveInfo* pPrimitiveInfo = (CPrimitive::SPrimitiveInfo*)(oMappedSubresource.pData);
      assert(pPrimitiveInfo);

      // Update color
      for (UINT uIndex = 0; uIndex < m_uVertexCount; ++uIndex)
      {
        pPrimitiveInfo[uIndex].Color = _v3Color;
      }

      // Unmap
      global::dx11::s_pDeviceContext->Unmap(m_pVertexBuffer, 0);

      // Save color
      m_v3CurrentColor = _v3Color;
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateBufferFromVertexData(const std::vector<CPrimitive::SPrimitiveInfo>& _vctPrimitiveInfo, const std::vector<UINT>& _vctIndexes)
    {
      if (_vctPrimitiveInfo.empty()) return -1;

      // Clean buffer
      m_oConstantBuffer.CleanBuffer();

      // Create constant buffer
      m_oConstantBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);

      // Create vertex buffer
      D3D11_BUFFER_DESC oVertexBufferDescriptor = {};
      oVertexBufferDescriptor.ByteWidth = UINT(sizeof(SPrimitiveInfo) * _vctPrimitiveInfo.size());
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = { 0 };
      oSubresourceData.pSysMem = _vctPrimitiveInfo.data();
      m_uVertexCount = (UINT)_vctPrimitiveInfo.size();

      HRESULT hr = global::dx11::s_pDevice->CreateBuffer
      (
        &oVertexBufferDescriptor,
        &oSubresourceData,
        &m_pVertexBuffer
      );
      if (_vctIndexes.empty()) return hr;

      // Config index buffer
      D3D11_BUFFER_DESC oIndexBufferDesc = {};
      oIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      oIndexBufferDesc.ByteWidth = (UINT)(sizeof(UINT) * _vctIndexes.size());
      oIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      oIndexBufferDesc.CPUAccessFlags = 0;
      m_uVertexCount = (UINT)_vctIndexes.size();

      D3D11_SUBRESOURCE_DATA oSubresourceIndexesData = {};
      oSubresourceIndexesData.pSysMem = _vctIndexes.data();

      return global::dx11::s_pDevice->CreateBuffer(&oIndexBufferDesc, &oSubresourceIndexesData, &m_pIndexBuffer);
    }
    // ------------------------------------
    void CPrimitive::DrawPrimitive()
    {
      // Set general data
      UINT uVertexStride = sizeof(render::graphics::CPrimitive::SPrimitiveInfo);
      UINT uVertexOffset = 0;
      global::dx11::s_pDeviceContext->IASetInputLayout(m_pInputLayout);
      global::dx11::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);

      // Set topology
      global::dx11::s_pDeviceContext->IASetPrimitiveTopology(m_eRenderMode == ERenderMode::SOLID ?
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

      // Set model matrix
      m_oConstantBuffer.GetData().mMatrix = m_oPrimitiveTransform.ComputeModelMatrix();
      m_oConstantBuffer.UpdateBuffer();

      // Apply constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
      global::dx11::s_pDeviceContext->VSSetConstantBuffers(1, 1, &pConstantBuffer);

      // 3D or 2D
      if (m_pIndexBuffer)
      {
        global::dx11::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        global::dx11::s_pDeviceContext->DrawIndexed(m_uVertexCount, 0, 0);
      }
      else
      {
        global::dx11::s_pDeviceContext->Draw(m_uVertexCount, 0);
      }
    }
  }
}
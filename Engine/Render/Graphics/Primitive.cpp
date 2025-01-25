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
      static const std::vector<CPrimitive::SPrimitiveInfo> s_oTrianglePrimitive =
      {
        // TRIANGLE
        { maths::CVector3(0, 1.0f, 0.0f),   maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(1.0f, -1.0f,  0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(-1.0f, -1.0f,  0.0f), maths::CVector3(1.0f, 1.0f, 1.0f) },
      };
      static const std::vector<CPrimitive::SPrimitiveInfo> s_oSquarePrimitive =
      {
        // FRONT FACE
        { maths::CVector3(-1.0f, 1.0f, 0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(1.0f, -1.0f, 0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(-1.0f,-1.0f, 0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) },                   
        { maths::CVector3(-1.0f, 1.0f, 0.0f),  maths::CVector3(1.0f,  1.0f,  1.0f) },
        { maths::CVector3(1.0f, 1.0f, 0.0f),   maths::CVector3(1.0f,  1.0f,  1.0f) },
        { maths::CVector3(1.0f, -1.0f, 0.0f),  maths::CVector3(1.0f,  1.0f,  1.0f) },
      };
      static const std::vector<CPrimitive::SPrimitiveInfo> s_oCubePrimitive =
      {
        { maths::CVector3(-1.0f,  -1.0f,  -1.0f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(-1.0f, 1.0f,  -1.0f), maths::CVector3(0.0f, 1.0f, 0.0f) },
        { maths::CVector3(1.0f, 1.0f,  -1.0f), maths::CVector3(0.0f, 0.0f, 1.0f)},
        { maths::CVector3(1.0f,  -1.0f,  -1.0f), maths::CVector3(1.0f, 1.0f, 1.0f) },

        { maths::CVector3(-1.0f,  -1.0f,  1.0f), maths::CVector3(1.0f, 0.0f, 0.0f)},
        { maths::CVector3(-1.0f, 1.0f,  1.0f), maths::CVector3(0.0f, 1.0f, 0.0f) },
        { maths::CVector3(1.0f, 1.0f,  1.0f), maths::CVector3(0.0f, 0.0f, 1.0f) },
        { maths::CVector3(1.0f,  -1.0f,  1.0f), maths::CVector3(1.0f, 1.0f, 1.0f) }
      };
      static const std::vector<uint32_t> s_oCubeIndexes = 
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
    }

    // ------------------------------------
    CPrimitive::CPrimitive(const EPrimitiveType& _ePrimitiveType)
    {
      HRESULT hr = CreateInputLayout();
      assert(!FAILED(hr));

      // Create buffer from presets
      switch (_ePrimitiveType)
      {
        case EPrimitiveType::SQUARE:
          hr = CreateBufferFromVertexData(internal_primitive::s_oSquarePrimitive);
          break;
        case EPrimitiveType::TRIANGLE:
          hr = CreateBufferFromVertexData(internal_primitive::s_oTrianglePrimitive);
          break;
        case EPrimitiveType::CUBE:
          hr = CreateBufferFromVertexData(internal_primitive::s_oCubePrimitive, internal_primitive::s_oCubeIndexes);
      }

      assert(!FAILED(hr));
    }
    // ------------------------------------
    CPrimitive::CPrimitive(const std::vector<SPrimitiveInfo>& _vctVertexData)
    {
      HRESULT hr = CreateInputLayout();
      assert(!FAILED(hr));

      // Create buffer from vertex data
      hr = CreateBufferFromVertexData(_vctVertexData);
      assert(!FAILED(hr));
    }
    // ------------------------------------
    CPrimitive::~CPrimitive()
    {
      m_oConstantBuffer.CleanBuffer();
      global::dx11::SafeRelease(m_pVertexBuffer);
      global::dx11::SafeRelease(m_pInputLayout);
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
      global::dx11::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
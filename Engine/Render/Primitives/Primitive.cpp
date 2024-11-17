#include "Primitive.h"
#include "Engine/Global/DX11GlobalInterface.h"
#include <d3dcompiler.h>
#include <cassert>
#include <iostream>

namespace render
{
  namespace primitive
  {
    namespace internal_primitive
    {
      static const std::vector<CPrimitive::SPrimitiveInfo> s_oTrianglePrimitive =
      {
        // TRIANGLE
        { maths::CVector3(0, 0.5f, 0.0f),   maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f,  0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(-0.5f, -0.5f,  0.0f), maths::CVector3(1.0f, 1.0f, 1.0f) },
      };
      static const std::vector<CPrimitive::SPrimitiveInfo> s_oSquarePrimitive =
      {
        // FRONT FACE
        { maths::CVector3(-0.5f, 0.5f, 0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f, 0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(-0.5f,-0.5f, 0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) },                   
        { maths::CVector3(-0.5f, 0.5f, 0.0f),  maths::CVector3(1.0f,  1.0f,  1.0f) },
        { maths::CVector3(0.5f, 0.5f, 0.0f),   maths::CVector3(1.0f,  1.0f,  1.0f) },
        { maths::CVector3(0.5f, -0.5f, 0.0f),  maths::CVector3(1.0f,  1.0f,  1.0f) },
      };
      static const std::vector<CPrimitive::SPrimitiveInfo> s_oCubePrimitive =
      {
        { maths::CVector3(-1.0f,  -1.0f,  -1.0f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(1.0f, -1.0f,  -1.0f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(-1.0f, 1.0f,  -1.0f), maths::CVector3(1.0f, 0.0f, 0.0f)},
        { maths::CVector3(1.0f,  1.0f,  -1.0f), maths::CVector3(1.0f, 0.0f, 0.0f) },

        { maths::CVector3(-1.0f,  -1.0f,  1.0f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(1.0f, -1.0f,  1.0f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(-1.0f, 1.0f,  1.0f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(1.0f,  1.0f,  1.0f), maths::CVector3(1.0f, 0.0f, 0.0f) }
      };
      static const std::vector<UINT> s_oCubeIndexes =
      {
        0, 1, 2, 2, 1, 3,
        4, 5, 6, 6, 5, 7,
        0, 1, 4, 4, 1, 5,
        2, 3, 6, 6, 3, 7,
        0, 2, 4, 4, 2, 6,
        1, 3, 5, 5, 3, 7
      };
    }

    // ------------------------------------
    CPrimitive::CPrimitive(const EPrimitiveType& _ePrimitiveType)
    {
      HRESULT hr = InitPrimitive();
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
      HRESULT hr = InitPrimitive();
      assert(!FAILED(hr));

      // Create buffer from vertex data
      hr = CreateBufferFromVertexData(_vctVertexData);
      assert(!FAILED(hr));
    }
    // ------------------------------------
    CPrimitive::~CPrimitive()
    {
      if (m_pVertexBuffer) { m_pVertexBuffer->Release(); }
      if (m_pInputLayout) { m_pInputLayout->Release(); }

      if (m_pVertexShaderBlob) { m_pVertexShaderBlob->Release(); }
      if (m_pVertexShader) { m_pVertexShader->Release(); }

      if (m_pPixelShaderBlob) { m_pPixelShaderBlob->Release(); }
      if (m_pPixelShader) { m_pPixelShader->Release(); }
    }
    // ------------------------------------
    HRESULT CPrimitive::InitPrimitive()
    {
      // Compile shaders
      HRESULT hr = CompileShaders();
      if (FAILED(hr)) return hr;

      // Init shaders
      hr = InitShaders();
      if (FAILED(hr)) return hr;

      // Create input layout
      hr = CreateInputLayout();
      if (FAILED(hr)) return hr;

      return hr;
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
        m_pVertexShaderBlob->GetBufferPointer(),
        m_pVertexShaderBlob->GetBufferSize(),
        &m_pInputLayout
      );
    }
    // ------------------------------------
    HRESULT CPrimitive::InitShaders()
    {
      // Create vertex shader
      HRESULT hr = global::dx11::s_pDevice->CreateVertexShader
      (
        m_pVertexShaderBlob->GetBufferPointer(),
        m_pVertexShaderBlob->GetBufferSize(),
        NULL,
        &m_pVertexShader
      );

      if (FAILED(hr)) return hr;

      // Create pixel shader
      return global::dx11::s_pDevice->CreatePixelShader
      (
        m_pPixelShaderBlob->GetBufferPointer(),
        m_pPixelShaderBlob->GetBufferSize(),
        NULL,
        &m_pPixelShader
      );
    }
    // ------------------------------------
    void CPrimitive::SetColor(const maths::CVector3& _v3Color)
    {
      // Map
      D3D11_MAPPED_SUBRESOURCE oMappedSubresource;
      HRESULT hResult = global::dx11::s_pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &oMappedSubresource);
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
    HRESULT CPrimitive::CompileShaders()
    {
      // Compile vertex shader
      HRESULT hr = D3DCompileFromFile
      (
        L"..\\Engine\\Shaders\\VertexShader.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_DEBUG,
        0,
        &m_pVertexShaderBlob,
        nullptr
      );

      if (FAILED(hr)) return hr;

      // Compile pixel shader
      return D3DCompileFromFile
      (
        L"..\\Engine\\Shaders\\PixelShader.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PSMain",
        "ps_5_0",
        D3DCOMPILE_DEBUG,
        0,
        &m_pPixelShaderBlob,
        nullptr
      );
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateBufferFromVertexData(const std::vector<CPrimitive::SPrimitiveInfo>& _vctPrimitiveInfo, const std::vector<UINT>& _vctIndexes)
    {
      if (_vctPrimitiveInfo.empty()) return -1;

      // Create constant buffer
      m_oConstantBuffer.Initialize(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);

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

      // Set as 3d primitive
      m_b3DPrimitive = true;

      // Config index buffer
      D3D11_BUFFER_DESC oIndexBufferDesc = {};
      oIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      oIndexBufferDesc.ByteWidth = sizeof(UINT) * _vctIndexes.size();
      oIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      oIndexBufferDesc.CPUAccessFlags = 0;

      D3D11_SUBRESOURCE_DATA oSubresourceIndexesData = {};
      oSubresourceIndexesData.pSysMem = _vctIndexes.data();

      return global::dx11::s_pDevice->CreateBuffer(&oIndexBufferDesc, &oSubresourceIndexesData, &m_pIndexBuffer);
    }
    // ------------------------------------
    void CPrimitive::Draw()
    {
      UINT uVertexStride = sizeof(render::primitive::CPrimitive::SPrimitiveInfo);
      UINT uVertexOffset = 0;

      // Set general data
      global::dx11::s_pDeviceContext->IASetInputLayout(m_pInputLayout);
      global::dx11::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);
      global::dx11::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Set pixel and vertex shaders
      global::dx11::s_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
      global::dx11::s_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

      // Calculate model matrix
      maths::CMatrix4x4 mMatrixTranslation = maths::CMatrix4x4::Translate(m_v3CurrentPosition);
      maths::CMatrix4x4 mMatrixScaling = maths::CMatrix4x4::Scale(maths::CVector3(1, 1, 1));
      maths::CMatrix4x4 mMatrixRotation = maths::CMatrix4x4::Rotation(maths::CVector3::Zero);
      m_oConstantBuffer.GetData().mMatrix = (mMatrixRotation * mMatrixScaling) * mMatrixTranslation;
      assert(m_oConstantBuffer.UpdateBuffer());

      // Apply constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
      global::dx11::s_pDeviceContext->VSSetConstantBuffers(1, 1, &pConstantBuffer);

      // Draw
      if (m_b3DPrimitive)
      {
        global::dx11::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        global::dx11::s_pDeviceContext->DrawIndexed(internal_primitive::s_oCubeIndexes.size(), 0, 0);
      }
      else
      {
        global::dx11::s_pDeviceContext->Draw(m_uVertexCount, 0);
      }
    }
  }
}
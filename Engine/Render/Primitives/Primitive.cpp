#include "Primitive.h"
#include "Engine/Global/DX11GlobalInterface.h"
#include <d3dcompiler.h>
#include <cassert>
#include <iostream>
#include "../Base/Engine.h"

namespace render
{
  namespace primitive
  {
    namespace internal_primitive
    {
      static const std::vector<CPrimitive::SPrimitiveInfo> s_oTrianglePrimitive =
      {
        // TRIANGLE
        { maths::CVector3(0, 0.5f, 0.0f),   maths::CVector3(1.0f, 1.0f, 1.0f) }, // Esquina superior izquierda
        { maths::CVector3(0.5f, -0.5f,  0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) }, // Esquina superior izquierda
        { maths::CVector3(-0.5f, -0.5f,  0.0f), maths::CVector3(1.0f, 1.0f, 1.0f) }, // Esquina superior izquierda
      };
      static const std::vector<CPrimitive::SPrimitiveInfo> s_oRectanglePrimitive =
      {
        // FRONT FACE
        { maths::CVector3(-0.5f, 0.5f, 0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) }, // Esquina superior izquierda
        { maths::CVector3(0.5f, -0.5f, 0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) }, // Esquina superior izquierda
        { maths::CVector3(-0.5f,-0.5f, 0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) }, // Esquina superior izquierda                    
        { maths::CVector3(-0.5f, 0.5f, 0.0f),  maths::CVector3(1.0f,  1.0f,  1.0f) }, // Esquina superior izquierda
        { maths::CVector3(0.5f, 0.5f, 0.0f),   maths::CVector3(1.0f,  1.0f,  1.0f) }, // Esquina superior izquierda
        { maths::CVector3(0.5f, -0.5f, 0.0f),  maths::CVector3(1.0f,  1.0f,  1.0f) }, // Esquina superior izquierda
      };
      static const std::vector<CPrimitive::SPrimitiveInfo> s_oCubePrimitive =
      {
        // FRONT FACE
        { maths::CVector3(-0.5f,  0.5f,  0.5f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(0.5f, -0.5f,  0.5f), maths::CVector3(0.0f, 1.0f, 0.0f) },
        { maths::CVector3(-0.5f, -0.5f,  0.5f), maths::CVector3(0.0f, 0.0f, 1.0f) },
        { maths::CVector3(-0.5f,  0.5f,  0.5f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(0.5f,  0.5f,  0.5f), maths::CVector3(1.0f, 1.0f, 0.0f) },
        { maths::CVector3(0.5f, -0.5f,  0.5f), maths::CVector3(0.0f, 1.0f, 0.0f) },

        // BACK FACE
        { maths::CVector3(-0.5f,  0.5f, -0.5f), maths::CVector3(0.0f, 1.0f, 1.0f) },
        { maths::CVector3(-0.5f, -0.5f, -0.5f), maths::CVector3(1.0f, 0.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f, -0.5f), maths::CVector3(0.5f, 0.5f, 0.5f) },
        { maths::CVector3(-0.5f,  0.5f, -0.5f), maths::CVector3(0.0f, 1.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f, -0.5f), maths::CVector3(0.5f, 0.5f, 0.5f) },
        { maths::CVector3(0.5f,  0.5f, -0.5f), maths::CVector3(1.0f, 1.0f, 1.0f) },

        // LEFT FACE
        { maths::CVector3(-0.5f,  0.5f, -0.5f), maths::CVector3(1.0f, 0.0f, 1.0f) },
        { maths::CVector3(-0.5f, -0.5f,  0.5f), maths::CVector3(0.5f, 0.5f, 0.5f) },
        { maths::CVector3(-0.5f, -0.5f, -0.5f), maths::CVector3(0.0f, 1.0f, 0.0f) },
        { maths::CVector3(-0.5f,  0.5f, -0.5f), maths::CVector3(1.0f, 0.0f, 1.0f) },
        { maths::CVector3(-0.5f,  0.5f,  0.5f), maths::CVector3(1.0f, 1.0f, 0.0f) },
        { maths::CVector3(-0.5f, -0.5f,  0.5f), maths::CVector3(0.5f, 0.5f, 0.5f) },

        // RIGHT FACE
        { maths::CVector3(0.5f,  0.5f, -0.5f), maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f, -0.5f), maths::CVector3(0.5f, 0.5f, 0.5f) },
        { maths::CVector3(0.5f, -0.5f,  0.5f), maths::CVector3(0.0f, 1.0f, 0.0f) },
        { maths::CVector3(0.5f,  0.5f, -0.5f), maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f,  0.5f), maths::CVector3(0.0f, 1.0f, 0.0f) },
        { maths::CVector3(0.5f,  0.5f,  0.5f), maths::CVector3(1.0f, 0.0f, 1.0f) },

        // TOP FACE
        { maths::CVector3(-0.5f,  0.5f, -0.5f), maths::CVector3(0.0f, 1.0f, 0.0f) },
        { maths::CVector3(-0.5f,  0.5f,  0.5f), maths::CVector3(1.0f, 0.0f, 0.0f) },
        { maths::CVector3(0.5f,  0.5f,  0.5f), maths::CVector3(0.5f, 0.5f, 0.5f) },
        { maths::CVector3(-0.5f,  0.5f, -0.5f), maths::CVector3(0.0f, 1.0f, 0.0f) },
        { maths::CVector3(0.5f,  0.5f,  0.5f), maths::CVector3(0.5f, 0.5f, 0.5f) },
        { maths::CVector3(0.5f,  0.5f, -0.5f), maths::CVector3(1.0f, 1.0f, 0.0f) },

        // BOTTOM FACE
        { maths::CVector3(-0.5f, -0.5f, -0.5f), maths::CVector3(0.0f, 0.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f,  0.5f), maths::CVector3(1.0f, 1.0f, 1.0f) },
        { maths::CVector3(-0.5f, -0.5f,  0.5f), maths::CVector3(0.5f, 0.5f, 0.5f) },
        { maths::CVector3(-0.5f, -0.5f, -0.5f), maths::CVector3(0.0f, 0.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f, -0.5f), maths::CVector3(0.0f, 1.0f, 1.0f) },
        { maths::CVector3(0.5f, -0.5f,  0.5f), maths::CVector3(1.0f, 1.0f, 1.0f) },
      };
    }

    // Static values
    const maths::CVector3 CPrimitive::s_vDefaultColor(1.0f, 1.0f, 1.0f);

    // ------------------------------------
    CPrimitive::CPrimitive(const EPrimitiveType& _ePrimitiveType)
    {
      HRESULT hr = InitPrimitive();
      assert(!FAILED(hr));

      // Create buffer from presets
      switch (_ePrimitiveType)
      {
      case EPrimitiveType::RECTANGLE:
        hr = CreateBufferFromVertexData(internal_primitive::s_oRectanglePrimitive);
        break;
      case EPrimitiveType::TRIANGLE:
        hr = CreateBufferFromVertexData(internal_primitive::s_oTrianglePrimitive);
        break;
      case EPrimitiveType::CUBE:
        hr = CreateBufferFromVertexData(internal_primitive::s_oCubePrimitive);
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
        /*
        { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        */
      };

      return global::dx11::s_pDX11Device->CreateInputLayout
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
      HRESULT hr = global::dx11::s_pDX11Device->CreateVertexShader
      (
        m_pVertexShaderBlob->GetBufferPointer(),
        m_pVertexShaderBlob->GetBufferSize(),
        NULL,
        &m_pVertexShader
      );

      if (FAILED(hr)) return hr;

      // Create pixel shader
      return global::dx11::s_pDX11Device->CreatePixelShader
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
      HRESULT hResult = global::dx11::s_pDX11DeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &oMappedSubresource);
      CPrimitive::SPrimitiveInfo* pPrimitiveInfo = (CPrimitive::SPrimitiveInfo*)(oMappedSubresource.pData);
      assert(pPrimitiveInfo);

      // Update color
      for (UINT uIndex = 0; uIndex < m_uVertexCount; ++uIndex)
      {
        pPrimitiveInfo[uIndex].Color = _v3Color;
      }

      // Unmap
      global::dx11::s_pDX11DeviceContext->Unmap(m_pVertexBuffer, 0);

      // Save color
      m_v3Color = _v3Color;
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
    HRESULT CPrimitive::CreateBufferFromVertexData(const std::vector<SPrimitiveInfo>& _vctPrimitiveInfo)
    {
      if (_vctPrimitiveInfo.empty()) return -1;

      // Create constant buffer
      m_oConstantBuffer.Initialize(global::dx11::s_pDX11Device, global::dx11::s_pDX11DeviceContext);

      // Create vertex buffer
      D3D11_BUFFER_DESC oVertexBufferDescriptor = {};
      oVertexBufferDescriptor.ByteWidth = UINT(sizeof(SPrimitiveInfo) * _vctPrimitiveInfo.size());
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = { 0 };
      oSubresourceData.pSysMem = _vctPrimitiveInfo.data();
      m_uVertexCount = (UINT)_vctPrimitiveInfo.size();

      return global::dx11::s_pDX11Device->CreateBuffer
      (
        &oVertexBufferDescriptor,
        &oSubresourceData,
        &m_pVertexBuffer
      );
    }
    // ------------------------------------
    void CPrimitive::Draw()
    {
      UINT uVertexStrid = sizeof(render::primitive::CPrimitive::SPrimitiveInfo);
      UINT uVertexOffset = 0;

      global::dx11::s_pDX11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      global::dx11::s_pDX11DeviceContext->IASetInputLayout(GetInputLayout());
      global::dx11::s_pDX11DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStrid, &uVertexOffset);

      // set vertex shader to use and pixel shader to use, and constant buffers for each
      global::dx11::s_pDX11DeviceContext->VSSetShader(GetVertexShader(), nullptr, 0);
      global::dx11::s_pDX11DeviceContext->PSSetShader(GetPixelShader(), nullptr, 0);

      maths::CMatrix4x4 mMatrixTranslation = maths::CMatrix4x4::Translate(GetPosition());
      maths::CMatrix4x4 mMatrixScaling = maths::CMatrix4x4::Scale(maths::CVector3(1, 1, 1));
      maths::CMatrix4x4 mMatrixRotation = maths::CMatrix4x4::Rotation(maths::CVector3::Zero);
      m_oConstantBuffer.GetData().mMatrix = (mMatrixRotation * mMatrixScaling) * mMatrixTranslation;
      assert(m_oConstantBuffer.UpdateBuffer());

      // Apply constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
      global::dx11::s_pDX11DeviceContext->VSSetConstantBuffers(1, 1, &pConstantBuffer); 

      // draw the vertex buffer with the shaders
      global::dx11::s_pDX11DeviceContext->Draw(GetIndexCount(), 0);
    }
  }
}
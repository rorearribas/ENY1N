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
      std::vector<CPrimitive::SPrimitiveInfo> oTriangle =
      {
        { maths::CVector3(0, 0.5f, 0.0f),   maths::CVector3(1.0f, 1.0f, 1.0f) }, // Esquina superior izquierda
        { maths::CVector3(0.5f, -0.5f,  0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) }, // Esquina superior izquierda
        { maths::CVector3(-0.5f, -0.5f,  0.0f), maths::CVector3(1.0f, 1.0f, 1.0f) }, // Esquina superior izquierda
      };
      std::vector<CPrimitive::SPrimitiveInfo> oRectangle =
      {
        // First triangle
        { maths::CVector3(-0.5f, 0.5f, 0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) }, // Esquina superior izquierda
        { maths::CVector3(0.5f, -0.5f, 0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) }, // Esquina superior izquierda
        { maths::CVector3(-0.5f,-0.5f, 0.0f),  maths::CVector3(1.0f, 1.0f, 1.0f) }, // Esquina superior izquierda
        // Second triangle                     
        { maths::CVector3(-0.5f, 0.5f, 0.0f),  maths::CVector3(1.0f,  1.0f,  1.0f) }, // Esquina superior izquierda
        { maths::CVector3(0.5f, 0.5f, 0.0f),   maths::CVector3(1.0f,  1.0f,  1.0f) }, // Esquina superior izquierda
        { maths::CVector3(0.5f, -0.5f, 0.0f),  maths::CVector3(1.0f,  1.0f,  1.0f) }, // Esquina superior izquierda
      };
    }

    CPrimitive::CPrimitive(const EPrimitiveType& _ePrimitiveType)
    {
      HRESULT hr = InitPrimitive();
      assert(!FAILED(hr));

      // Create buffer from presets
      switch (_ePrimitiveType)
      {
      case EPrimitiveType::RECTANGLE:
        hr = CreateBufferFromVertexData(global::dx11::s_pDX11Device, internal_primitive::oRectangle);
        break;
      case EPrimitiveType::TRIANGLE:
        hr = CreateBufferFromVertexData(global::dx11::s_pDX11Device, internal_primitive::oTriangle);
        break;
      }

      assert(!FAILED(hr));
    }
    // ------------------------------------
    CPrimitive::CPrimitive(std::vector<SPrimitiveInfo>& _vctVertexData)
    {
      HRESULT hr = InitPrimitive();
      assert(!FAILED(hr));

      // Create buffer from vertex data
      hr = CreateBufferFromVertexData(global::dx11::s_pDX11Device, _vctVertexData);
      assert(!FAILED(hr));
    }
    // ------------------------------------
    HRESULT CPrimitive::InitPrimitive()
    {
      // Compile shaders
      HRESULT hr = CompileShaders();
      if (FAILED(hr)) return hr;

      // Init shaders
      hr = InitShaders(global::dx11::s_pDX11Device);
      if (FAILED(hr)) return hr;

      // Create input layout
      hr = CreateInputLayout(global::dx11::s_pDX11Device);
      if (FAILED(hr)) return hr;

      return hr;
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateInputLayout(ID3D11Device* _pDevice)
    {
      D3D11_INPUT_ELEMENT_DESC oInputElementDesc[] =
      {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SPrimitiveInfo, m_vPosition), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SPrimitiveInfo, m_vColor), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        /*
        { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        */
      };

      return _pDevice->CreateInputLayout
      (
        oInputElementDesc,
        ARRAYSIZE(oInputElementDesc),
        m_pVertexShaderBlob->GetBufferPointer(),
        m_pVertexShaderBlob->GetBufferSize(),
        &m_pInputLayout
      );
    }
    // ------------------------------------
    HRESULT CPrimitive::InitShaders(ID3D11Device* _pDevice)
    {
      // Create vertex shader
      HRESULT hr = _pDevice->CreateVertexShader
      (
        m_pVertexShaderBlob->GetBufferPointer(),
        m_pVertexShaderBlob->GetBufferSize(),
        NULL,
        &m_pVertexShader
      );

      if (FAILED(hr)) return hr;

      // Create pixel shader
      return _pDevice->CreatePixelShader
      (
        m_pPixelShaderBlob->GetBufferPointer(),
        m_pPixelShaderBlob->GetBufferSize(),
        NULL,
        &m_pPixelShader
      );
    }
    // ------------------------------------
    CPrimitive::~CPrimitive()
    {
      if (m_pBuffer) { m_pBuffer->Release(); }
      if (m_pInputLayout) { m_pInputLayout->Release(); }

      if (m_pVertexShaderBlob) { m_pVertexShaderBlob->Release(); }
      if (m_pVertexShader) { m_pVertexShader->Release(); }

      if (m_pPixelShaderBlob) { m_pPixelShaderBlob->Release(); }
      if (m_pPixelShader) { m_pPixelShader->Release(); }
    }
    // ------------------------------------
    void CPrimitive::SetColor(const maths::CVector3& _v3Color)
    {
      if (m_vctVertexData.empty()) return;

      // Update colors
      for (size_t i = 0; i < m_vctVertexData.size(); ++i) 
      {
        m_vctVertexData[i].m_vColor = _v3Color; 
      }

      // Create a new buffer
      assert(!FAILED(CreateBufferFromVertexData(global::dx11::s_pDX11Device, m_vctVertexData)));

      // Save color
      m_v3Color = _v3Color;
    }
    // ------------------------------------
    HRESULT CPrimitive::CompileShaders()
    {
      // Compile vertex shader
      HRESULT hr = D3DCompileFromFile
      (
        L"D:\\Projects\\Personal\\ENY1N\\Engine\\Shaders\\VertexShader.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "vs_main",
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
        L"D:\\Projects\\Personal\\ENY1N\\Engine\\Shaders\\PixelShader.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "ps_main",
        "ps_5_0",
        D3DCOMPILE_DEBUG,
        0,
        &m_pPixelShaderBlob,
        nullptr
      );
    }
    // ------------------------------------
    HRESULT CPrimitive::CreateBufferFromVertexData(ID3D11Device* _pDevice, std::vector<SPrimitiveInfo>& _vctPrimitiveInfo)
    {
      if (_vctPrimitiveInfo.empty()) return -1;

      // Move data
      m_vctVertexData = std::move(_vctPrimitiveInfo);

      D3D11_BUFFER_DESC oVertexBufferDescriptor = {};
      oVertexBufferDescriptor.ByteWidth = UINT(sizeof(SPrimitiveInfo) * m_vctVertexData.size());
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

      D3D11_SUBRESOURCE_DATA oSubresourceData = { 0 };
      oSubresourceData.pSysMem = m_vctVertexData.data();

      // Release buffer
      if (m_pBuffer) { m_pBuffer->Release(); }

      return _pDevice->CreateBuffer
      (
        &oVertexBufferDescriptor,
        &oSubresourceData,
        &m_pBuffer
      );
    }
  }
}
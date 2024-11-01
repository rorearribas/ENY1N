#include "PrimitiveItem.h"
#include "Engine/Global/DX11GlobalInterface.h"
#include <d3dcompiler.h>
#include <cassert>

namespace render
{
  namespace items
  {
    CPrimitiveItem::CPrimitiveItem(std::vector<float>& _vctVertexData)
    {
      // Compile shaders
      HRESULT hr = CompileShaders();
      assert(!FAILED(hr));

      // Init shaders
      hr = InitShaders(global::dx11::s_pDX11Device);
      assert(!FAILED(hr));

      // Create input layout
      hr = CreateInputLayout(global::dx11::s_pDX11Device);
      assert(!FAILED(hr));

      // Create buffer from vertex data
      hr = CreateBufferFromVertexData(global::dx11::s_pDX11Device, _vctVertexData);
      assert(!FAILED(hr));
    }
    // ------------------------------------
    HRESULT CPrimitiveItem::CreateInputLayout(ID3D11Device* _pDevice)
    {
      D3D11_INPUT_ELEMENT_DESC oInputElementDesc[] =
      {
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
    HRESULT CPrimitiveItem::InitShaders(ID3D11Device* _pDevice)
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
    CPrimitiveItem::~CPrimitiveItem()
    {
      if (m_pBuffer) { m_pBuffer->Release(); }
      if (m_pInputLayout) { m_pInputLayout->Release(); }

      if (m_pVertexShaderBlob) { m_pBuffer->Release(); }
      if (m_pVertexShader) { m_pBuffer->Release(); }

      if (m_pPixelShaderBlob) { m_pPixelShaderBlob->Release(); }
      if (m_pPixelShader) { m_pPixelShader->Release(); }
    }
    // ------------------------------------
    HRESULT CPrimitiveItem::CompileShaders()
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
    HRESULT CPrimitiveItem::CreateBufferFromVertexData(ID3D11Device* _pDevice, std::vector<float>& _vctVertexData)
    {
      if (_vctVertexData.empty()) return -1;

      // Move data
      m_vctVertexData = std::move(_vctVertexData);

      D3D11_BUFFER_DESC oVertexBufferDescriptor = {};
      oVertexBufferDescriptor.ByteWidth = UINT(sizeof(float) * m_vctVertexData.size());
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
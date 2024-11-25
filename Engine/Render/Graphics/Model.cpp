#include "Model.h"
#include "Primitive.h"
#include "Engine/Global/GlobalResources.h"
#include <d3dcompiler.h>
#include <cassert>
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace graphics
  {
    // ------------------------------------
    CModel::CModel()
    {
      HRESULT hr = InitModel();
      UNUSED_VARIABLE(hr);
      assert(!FAILED(hr));
    }
    // ------------------------------------
    CModel::~CModel()
    {
      global::dx11::SafeRelease(m_pVertexBuffer);
      global::dx11::SafeRelease(m_pInputLayout);

      global::dx11::SafeRelease(m_pVertexShaderBlob);
      global::dx11::SafeRelease(m_pVertexShader);

      global::dx11::SafeRelease(m_pPixelShaderBlob);
      global::dx11::SafeRelease(m_pPixelShader);
    }
    // ------------------------------------
    HRESULT CModel::InitModel()
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
    HRESULT CModel::CreateInputLayout()
    {
      D3D11_INPUT_ELEMENT_DESC oInputElementDesc[] =
      {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SModelInfo, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SModelInfo, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
    HRESULT CModel::InitShaders()
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
    HRESULT CModel::CompileShaders()
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
    void CModel::DrawModel()
    {
      // Set general data
      UINT uVertexStride = sizeof(render::graphics::CModel::SModelInfo);
      UINT uVertexOffset = 0;
      global::dx11::s_pDeviceContext->IASetInputLayout(m_pInputLayout);
      global::dx11::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);
      global::dx11::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Set pixel and vertex shaders
      global::dx11::s_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
      global::dx11::s_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

      // Set model matrix
      m_oConstantBuffer.GetData().mMatrix = m_oModelTransform.ComputeModelMatrix();
      m_oConstantBuffer.UpdateBuffer();

      // Apply constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
      global::dx11::s_pDeviceContext->VSSetConstantBuffers(1, 1, &pConstantBuffer);

      // Draw
      global::dx11::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
      global::dx11::s_pDeviceContext->DrawIndexed(m_uVertexCount, 0, 0);
    }
  }
}
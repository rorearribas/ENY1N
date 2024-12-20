#include "Model.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Managers/ResourceManager.h"
#include <d3dcompiler.h>
#include "Libs/Macros/GlobalMacros.h"
#include <cassert>

namespace render
{
  namespace graphics
  {
    // ------------------------------------
    CModel::CModel(const char* _sModelPath, const char* _sBaseMltDir)
    {
      HRESULT hr = InitModel(_sModelPath, _sBaseMltDir);
      UNUSED_VARIABLE(hr);
      assert(!FAILED(hr));
    }
    // ------------------------------------
    CModel::~CModel()
    {
      m_oModelData.m_vctMeshes.clear();
      m_oModelData.m_vctVertexData.clear();

      global::dx11::SafeRelease(m_pVertexBuffer);
      global::dx11::SafeRelease(m_pInputLayout);

      global::dx11::SafeRelease(m_pVertexShaderBlob);
      global::dx11::SafeRelease(m_pVertexShader);

      global::dx11::SafeRelease(m_pPixelShaderBlob);
      global::dx11::SafeRelease(m_pPixelShader);
    }
    // ------------------------------------
    HRESULT CModel::CreateShaders()
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
        L"..\\Engine\\Shaders\\Model\\DeferredVertexShader.hlsl",
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
        L"..\\Engine\\Shaders\\Model\\DeferredPixelShader.hlsl",
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
    HRESULT CModel::CreateInputLayout()
    {
      D3D11_INPUT_ELEMENT_DESC oInputElementDesc[] =
      {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(render::graphics::SVertexData, TexCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
    HRESULT CModel::InitModel(const char* _sModelPath, const char* _sBaseMltDir)
    {
      // Try to load model
      CResourceManager* pResourceManager = CResourceManager::GetInstance();
      m_oModelData = std::move(pResourceManager->LoadModel(_sModelPath, _sBaseMltDir));
      if (m_oModelData.m_vctMeshes.empty()) return E_FAIL;

      // Init constant buffer
      m_oConstantBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);

      // Config vertex buffer
      D3D11_BUFFER_DESC oVertexBufferDescriptor = {};
      oVertexBufferDescriptor.ByteWidth = static_cast<UINT>((sizeof(render::graphics::SVertexData) * m_oModelData.m_vctVertexData.size()));
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = { 0 };
      oSubresourceData.pSysMem = m_oModelData.m_vctVertexData.data();

      HRESULT hr = global::dx11::s_pDevice->CreateBuffer
      (
        &oVertexBufferDescriptor,
        &oSubresourceData,
        &m_pVertexBuffer
      );
      if (FAILED(hr)) return hr;

      for (render::graphics::CMesh* pMesh : m_oModelData.m_vctMeshes)
      {
        pMesh->ApplyMaterials(m_pVertexBuffer);
      }

      // Compile shaders
      hr = CompileShaders();
      if (FAILED(hr)) return hr;

      // Init shaders
      hr = CreateShaders();
      if (FAILED(hr)) return hr;

      // Create input layout
      hr = CreateInputLayout();
      if (FAILED(hr)) return hr;

      return hr;
    }
    // ------------------------------------
    void CModel::DrawModel()
    {
      // Set vertex buffer
      UINT uVertexStride = sizeof(render::graphics::SVertexData);
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

      // Draw meshes
      for (render::graphics::CMesh* pMesh : m_oModelData.m_vctMeshes)
      {
        pMesh->DrawMesh();
      }
    }
  }
}
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
    CModel::CModel(const char* _sModelPath)
    {
      HRESULT hr = InitModel(_sModelPath);
      UNUSED_VARIABLE(hr);
      assert(!FAILED(hr));
    }
    // ------------------------------------
    CModel::~CModel()
    {
      m_oConstantBuffer.CleanBuffer();

      global::dx11::SafeRelease(m_pVertexBuffer);
      global::dx11::SafeRelease(m_pInputLayout);

      global::dx11::SafeRelease(m_pVertexShaderBlob);
      global::dx11::SafeRelease(m_pVertexShader);

      global::dx11::SafeRelease(m_pPixelShaderBlob);
      global::dx11::SafeRelease(m_pPixelShader);
    }
    // ------------------------------------
    HRESULT CModel::InitModel(const char* _sPath)
    {
      // Create buffers from model data
      CResourceManager* pResourceManager = CResourceManager::GetInstance();
      //m_pTexture = new render::texture::CTexture("C://Users//Ruben//Desktop//cat//Cat_diffuse.jpg");

      /*CResourceManager::SModelData oModelData = pResourceManager->LoadModelData(_sPath, "rb");
      UNUSED_VARIABLE(oModelData);*/

      CResourceManager::SModelData vctMeshes = pResourceManager->LoadModel(_sPath, "C://Users//Ruben//Desktop//Model_2");
      UNUSED_VARIABLE(vctMeshes);

      //render::graphics::CMesh* pMesh = vctMeshes[0];
      //UNUSED_VARIABLE(pMesh);

      HRESULT hr = CreateBuffersFromModelData(vctMeshes.m_vctVertexData, vctMeshes.m_vctIndexes);
      if (FAILED(hr)) return hr;

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
    HRESULT CModel::CreateBuffersFromModelData(const CMesh::TVertexDataList& _vctVertexInfo, const CMesh::TIndexesList& _vctIndexes)
    {
      // Create constant buffer
      m_oConstantBuffer.Initialize(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);

      // Config vertex buffer
      D3D11_BUFFER_DESC oVertexBufferDescriptor = {};
      oVertexBufferDescriptor.ByteWidth = static_cast<UINT>((sizeof(render::graphics::SVertexData) * _vctVertexInfo.size()));
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = { 0 };
      oSubresourceData.pSysMem = _vctVertexInfo.data();

      HRESULT hr = global::dx11::s_pDevice->CreateBuffer
      (
        &oVertexBufferDescriptor,
        &oSubresourceData,
        &m_pVertexBuffer
      );
      if (FAILED(hr)) return hr;

      // Config index buffer
      D3D11_BUFFER_DESC oIndexBufferDesc = {};
      oIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      oIndexBufferDesc.ByteWidth = static_cast<UINT>((sizeof(uint32_t) * _vctIndexes.size()));
      oIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      oIndexBufferDesc.CPUAccessFlags = 0;
      m_uVertexCount = static_cast<UINT>(_vctIndexes.size());

      D3D11_SUBRESOURCE_DATA oSubresourceIndexesData = {};
      oSubresourceIndexesData.pSysMem = _vctIndexes.data();

      return global::dx11::s_pDevice->CreateBuffer(&oIndexBufferDesc, &oSubresourceIndexesData, &m_pIndexBuffer);
    }
    // ------------------------------------
    HRESULT CModel::CreateInputLayout()
    { 
      D3D11_INPUT_ELEMENT_DESC oInputElementDesc[] =
      {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(render::graphics::SVertexData, TexCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
        L"..\\Engine\\Shaders\\Model\\MVertexShader.hlsl",
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
        L"..\\Engine\\Shaders\\Model\\MPixelShader.hlsl",
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
      UINT uVertexStride = sizeof(render::graphics::SVertexData);
      UINT uVertexOffset = 0;
      global::dx11::s_pDeviceContext->IASetInputLayout(m_pInputLayout);
      global::dx11::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);
      global::dx11::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      if (m_pTexture)
      {
        ID3D11ShaderResourceView* shaderResourceView = m_pTexture->GetShaderResourceView();
        ID3D11SamplerState* pSamplerState = m_pTexture->GetSamplerState();
        if (shaderResourceView && pSamplerState)
        {
          global::dx11::s_pDeviceContext->PSSetShaderResources(0, 1, &shaderResourceView);
          global::dx11::s_pDeviceContext->PSSetSamplers(0, 1, &pSamplerState);
        }
      }

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
#include "Mesh.h"
#include <d3dcompiler.h>
#include <cassert>
#include "Libs/Macros/GlobalMacros.h"
#include <iostream>

namespace render
{
  namespace graphics
  {
    CMesh::CMesh(const std::string& _sMeshName) : m_sMeshName(_sMeshName)
    {
      HRESULT hr = InitMesh();
      UNUSED_VARIABLE(hr);
      assert(!FAILED(hr));
    }
    // ------------------------------------
    CMesh::~CMesh()
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
    void CMesh::DrawMesh(const maths::CTransform& _oTransform)
    {
      // Set general data
      UINT uVertexStride = sizeof(render::graphics::SVertexData);
      UINT uVertexOffset = 0;
      global::dx11::s_pDeviceContext->IASetInputLayout(m_pInputLayout);
      global::dx11::s_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uVertexStride, &uVertexOffset);
      global::dx11::s_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Set pixel and vertex shaders
      global::dx11::s_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
      global::dx11::s_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

      // Set model matrix
      m_oConstantBuffer.GetData().mMatrix = _oTransform.ComputeModelMatrix();
      m_oConstantBuffer.UpdateBuffer();

      // Apply constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
      global::dx11::s_pDeviceContext->VSSetConstantBuffers(1, 1, &pConstantBuffer);

      // Draw textures
      for (auto& it : m_dctMaterials)
      {
        /*render::texture::CTexture* pAmbient = it.second->GetTexture(render::material::EModifierType::AMBIENT);
        if (pAmbient)
        {
          pAmbient->DrawTexture();
        }*/
        render::texture::CTexture* pDiffuse = it.second->GetTexture(render::material::EModifierType::DIFFUSE);
        if (pDiffuse)
        {
          pDiffuse->DrawTexture();
        }
      }

      // Draw
      global::dx11::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
      global::dx11::s_pDeviceContext->DrawIndexed(m_uVertexCount, 0, 0);
    }
    // ------------------------------------
    void CMesh::AddMaterial(render::material::CMaterial* _pMaterial, const uint32_t& _uMaterialIdx)
    {
      m_dctMaterials.emplace(_uMaterialIdx, _pMaterial);
    }
    // ------------------------------------
    HRESULT CMesh::InitMesh()
    {
      // Compile shaders
      HRESULT hr = CompileShaders();
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
    HRESULT CMesh::CreateShaders()
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
    HRESULT CMesh::CompileShaders()
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
    HRESULT CMesh::CreateInputLayout()
    {
      D3D11_INPUT_ELEMENT_DESC oInputElementDesc[] =
      {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
    HRESULT CMesh::CreateMesh(TVertexDataList& _vctVertexData, TIndexesList& _vctIndexes)
    {
      // Create constant buffer
      m_oConstantBuffer.Initialize(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);

      // Config vertex buffer
      D3D11_BUFFER_DESC oVertexBufferDescriptor = {};
      oVertexBufferDescriptor.ByteWidth = static_cast<UINT>((sizeof(render::graphics::SVertexData) * _vctVertexData.size()));
      oVertexBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
      oVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVertexBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = { 0 };
      oSubresourceData.pSysMem = _vctVertexData.data();

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
    void CMesh::ApplyMaterials()
    {
      // Set material
      uint32_t uOffsetMat = 0;
      for (int iIterator = 0; iIterator < m_vctMaterialsIds.size(); iIterator++)
      {
        int iMaterialId = m_vctMaterialsIds[iIterator];
        render::material::CMaterial* pMaterial = m_dctMaterials[iMaterialId];
        assert(pMaterial);

        D3D11_MAPPED_SUBRESOURCE oMappedSubresource;
        HRESULT hr = global::dx11::s_pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &oMappedSubresource);
        UNUSED_VARIABLE(hr);
        assert(!FAILED(hr));

        SVertexData* pVertexData = (SVertexData*)(oMappedSubresource.pData);
        assert(pVertexData);

        // Update color
        const uint32_t uMeshOffset = m_uVertexCount / static_cast<uint32_t>(m_vctMaterialsIds.size());
        for (UINT uIndex = uOffsetMat; uIndex < uMeshOffset + uOffsetMat; ++uIndex)
        {
          pVertexData[uIndex].Color = pMaterial->GetDiffuseColor();
        }

        // Unmap
        global::dx11::s_pDeviceContext->Unmap(m_pVertexBuffer, 0);

        // Add offset
        uOffsetMat += uMeshOffset;
      }
    }
  }
}

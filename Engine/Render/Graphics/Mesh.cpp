#include "Mesh.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Engine/Render/RenderTypes.h"
#include <cassert>

namespace render
{
  namespace gfx
  {
    std::vector<D3D11_INPUT_ELEMENT_DESC> SVertexData::s_vctInputElementDesc =
    {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::gfx::SVertexData, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::gfx::SVertexData, Normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::gfx::SVertexData, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(render::gfx::SVertexData, UV), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    // ------------------------------------
    bool SVertexData::operator==(const SVertexData& _other) const
    {
      return Position == _other.Position && // Pos
        Normal == _other.Normal && // Normal
        UV == _other.UV && // TexCoord
        MaterialID == _other.MaterialID; // Material ID
    }
    // ------------------------------------
    bool SVertexData::operator!=(const SVertexData& _other) const
    {
      return !(*this == _other);
    }
    // ------------------------------------
    CMesh::~CMesh()
    {
      ClearBuffers();
      ClearMaterials();
    }
    // ------------------------------------
    void CMesh::Draw()
    {
      // Push textures!
      bool bHasTexture = false;
      for (auto& it : m_dctMaterials)
      {
        texture::CTexture2D<SHADER_RESOURCE>* pDiffuseTexture = it.second->GetTexture(DIFFUSE);
        texture::CTexture2D<SHADER_RESOURCE>* pNormalTexture = it.second->GetTexture(NORMAL);
        texture::CTexture2D<SHADER_RESOURCE>* pSpecularTexture = it.second->GetTexture(SPECULAR);

        // Check if has textures
        if (pDiffuseTexture || pNormalTexture || pSpecularTexture)
        {
          bHasTexture = true;
        }

        // Texture list
        ID3D11ShaderResourceView* lstTextures[3] =
        {
          pDiffuseTexture ? pDiffuseTexture->GetView() : nullptr,
          pNormalTexture ? pNormalTexture->GetView() : nullptr,
          pSpecularTexture ? pSpecularTexture->GetView() : nullptr,
        };
        // Bind shaders
        global::dx::s_pDeviceContext->PSSetShaderResources(0, ARRAYSIZE(lstTextures), lstTextures);

        // Sampler list
        ID3D11SamplerState* lstSamplers[3] =
        {
          pDiffuseTexture ? pDiffuseTexture->GetSampler() : nullptr,
          pNormalTexture ? pNormalTexture->GetSampler() : nullptr,
          pSpecularTexture ? pSpecularTexture->GetSampler() : nullptr
        };
        // Bind samplers
        global::dx::s_pDeviceContext->PSSetSamplers(0, ARRAYSIZE(lstSamplers), lstSamplers);
      }

      // Update buffer
      m_oConstantModelData.GetData().IgnoreGlobalLighting = m_bIgnoreGlobalLighting;
      m_oConstantModelData.GetData().HasTexture = bHasTexture;
      bool bOk = m_oConstantModelData.WriteBuffer();
      UNUSED_VAR(bOk);
      assert(bOk);

      // Set constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantModelData.GetBuffer();
      global::dx::s_pDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);

      // Draw
      global::dx::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
      global::dx::s_pDeviceContext->DrawIndexed(static_cast<uint32_t>(m_vctIndices.size()), 0, 0);
    }
    // ------------------------------------
    void CMesh::AddMaterial(render::mat::CMaterial* _pMaterial, const uint32_t& _uMaterialIdx)
    {
      m_dctMaterials.emplace(_uMaterialIdx, _pMaterial);
    }
    // ------------------------------------
    HRESULT CMesh::CreateBuffer(TIndexesList& _vctIndices)
    {
      // Clean mesh
      ClearBuffers();

      // Init constant check texture
      m_oConstantModelData.Init(global::dx::s_pDevice, global::dx::s_pDeviceContext);

      // Get indexes
      m_vctIndices = std::move(_vctIndices);

      // Config index buffer
      D3D11_BUFFER_DESC oIndexBufferDesc = D3D11_BUFFER_DESC();
      oIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      oIndexBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(uint32_t) * m_vctIndices.size()));
      oIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      oIndexBufferDesc.CPUAccessFlags = 0;

      D3D11_SUBRESOURCE_DATA oSubresourceIndexesData = {};
      oSubresourceIndexesData.pSysMem = m_vctIndices.data();

      return global::dx::s_pDevice->CreateBuffer(&oIndexBufferDesc, &oSubresourceIndexesData, &m_pIndexBuffer);
    }
    // ------------------------------------
    void CMesh::IgnoreGlobalLighting(bool _bState)
    {
      m_bIgnoreGlobalLighting = _bState;
    }
    // ------------------------------------
    void CMesh::UpdateVertexColor(ID3D11Buffer* _pVertexBuffer)
    {
      // Mapped vertex buffer (CPU)
      D3D11_MAPPED_SUBRESOURCE oMappedSubresource;
      HRESULT hResult = global::dx::s_pDeviceContext->Map(_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &oMappedSubresource);
      UNUSED_VAR(hResult);
      assert(!FAILED(hResult));

      // Get vertex data
      SVertexData* pVertexData = static_cast<SVertexData*>(oMappedSubresource.pData);
      assert(pVertexData);

      // Offset
      uint32_t uOffset = 0;
      uint32_t uTrianglesSize = static_cast<uint32_t>(m_vctIndices.size()) / 3; // Get triangles
      for (uint32_t uIndex = 0; uIndex < uTrianglesSize; uIndex++)
      {
        // Set pixel color
        uint32_t uMeshOffset = (static_cast<uint32_t>(m_vctIndices.size()) / uTrianglesSize); // Get mesh offset
        for (uint32_t uJ = uOffset; uJ < uMeshOffset + uOffset; uJ++)
        {
          uint32_t uVertexDataIdx = m_vctIndices[uJ];
          auto it = m_dctMaterials.find(pVertexData[uVertexDataIdx].MaterialID);
          if (it != m_dctMaterials.end())
          {
            render::mat::CMaterial* pMaterial = it->second;
            pVertexData[uVertexDataIdx].Color = pMaterial->GetDiffuseColor(); // Set diffuse value.
          }
        }
        // Add offset
        uOffset += uMeshOffset;
      }

      // Unmap
      global::dx::s_pDeviceContext->Unmap(_pVertexBuffer, 0);
    }
    // ------------------------------------
    void CMesh::ClearBuffers()
    {
      global::dx::SafeRelease(m_pIndexBuffer);
      m_oConstantModelData.CleanBuffer();
    }
    // ------------------------------------
    void CMesh::ClearMaterials()
    {
      auto it = m_dctMaterials.begin();
      for (; it != m_dctMaterials.end(); ++it)
      {
        global::ReleaseObject(it->second);
      }
      m_dctMaterials.clear();
    }
  }
}

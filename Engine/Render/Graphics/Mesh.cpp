#include "Mesh.h"
#include <d3dcompiler.h>
#include <cassert>
#include "Libs/Macros/GlobalMacros.h"
#include <iostream>

namespace render
{
  namespace graphics
  {
    // ------------------------------------
    CMesh::~CMesh()
    {
      global::dx11::SafeRelease(m_pIndexBuffer);
      m_dctMaterials.clear();
    }
    // ------------------------------------
    void CMesh::DrawMesh()
    {
      // Draw textures
      for (auto& it : m_dctMaterials)
      {
        render::texture::CTexture* pDiffuse = it.second->GetTexture(render::material::EModifierType::DIFFUSE);
        if (pDiffuse) { pDiffuse->BindTexture(); }
        m_oConstantTexture.GetData().bHasTexture = pDiffuse;
        m_oConstantTexture.UpdateBuffer();

        // Set constant buffer
        ID3D11Buffer* pConstantBuffer = m_oConstantTexture.GetBuffer();
        global::dx11::s_pDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
      }

      // Draw
      global::dx11::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
      global::dx11::s_pDeviceContext->DrawIndexed(static_cast<uint32_t>(m_vctIndexes.size()), 0, 0);
    }
    // ------------------------------------
    void CMesh::AddMaterial(render::material::CMaterial* _pMaterial, const uint32_t& _uMaterialIdx)
    {
      m_dctMaterials.emplace(_uMaterialIdx, _pMaterial);
    }
    // ------------------------------------
    HRESULT CMesh::CreateMesh(TIndexesList& _vctIndexes)
    {
      // Init constant check texture
      m_oConstantTexture.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);

      // Get indexes
      m_vctIndexes = std::move(_vctIndexes);

      // Config index buffer
      D3D11_BUFFER_DESC oIndexBufferDesc = {};
      oIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      oIndexBufferDesc.ByteWidth = static_cast<UINT>((sizeof(uint32_t) * m_vctIndexes.size()));
      oIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      oIndexBufferDesc.CPUAccessFlags = 0;

      D3D11_SUBRESOURCE_DATA oSubresourceIndexesData = {};
      oSubresourceIndexesData.pSysMem = m_vctIndexes.data();

      return global::dx11::s_pDevice->CreateBuffer(&oIndexBufferDesc, &oSubresourceIndexesData, &m_pIndexBuffer);
    }
    // ------------------------------------
    void CMesh::ApplyMaterials(ID3D11Buffer* _pVertexBuffer)
    {
      // Mapped vertex buffer
      D3D11_MAPPED_SUBRESOURCE oMappedSubresource;
      HRESULT hr = global::dx11::s_pDeviceContext->Map(_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &oMappedSubresource);
      UNUSED_VARIABLE(hr);
      assert(!FAILED(hr));

      // Get vertex data
      SVertexData* pVertexData = static_cast<SVertexData*>(oMappedSubresource.pData);
      assert(pVertexData);

      // Offset
      uint32_t uOffsetMat = 0;
      uint32_t uMaxTriangles = static_cast<uint32_t>(m_vctIndexes.size()) / 3; // Get max triangles
      for (uint32_t uIndex = 0; uIndex < uMaxTriangles; uIndex++)
      {
        // Set material color
        uint32_t uMeshOffset = static_cast<uint32_t>(m_vctIndexes.size()) / uMaxTriangles; // Get mesh offset
        for (uint32_t uJ = uOffsetMat; uJ < uMeshOffset + uOffsetMat; uJ++)
        {
          uint32_t uVertexDataIdx = m_vctIndexes[uJ];
          auto it = m_dctMaterials.find(pVertexData[uVertexDataIdx].MaterialId);
          if (it != m_dctMaterials.end())
          {
            render::material::CMaterial* pMaterial = it->second;
            pVertexData[uVertexDataIdx].Color = pMaterial->GetDiffuseColor();
          }
        }
        // Add offset
        uOffsetMat += uMeshOffset;
      }

      // Unmap
      global::dx11::s_pDeviceContext->Unmap(_pVertexBuffer, 0);
    }
  }
}

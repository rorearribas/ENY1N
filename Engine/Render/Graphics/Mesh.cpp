#include "Mesh.h"
#include <d3dcompiler.h>
#include <cassert>
#include "Libs/Macros/GlobalMacros.h"
#include <iostream>

namespace render
{
  namespace graphics
  {
    std::vector<D3D11_INPUT_ELEMENT_DESC> SVertexData::s_vctInputElementDesc =
    {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(render::graphics::SVertexData, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(render::graphics::SVertexData, TexCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    // ------------------------------------
    bool SVertexData::operator==(const SVertexData& _other) const
    {
      return Position == _other.Position && // Pos
      Normal == _other.Normal && // Normal
      TexCoord == _other.TexCoord && // TexCoord
      Color == _other.Color && // Color
      MaterialId == _other.MaterialId; // Material ID
    }
    // ------------------------------------
    bool SVertexData::operator!=(const SVertexData& _other) const
    {
      return !(*this == _other);
    }

    // ------------------------------------
    CMesh::~CMesh()
    {
      Clean();
    }
    // ------------------------------------
    void CMesh::DrawMesh()
    {
      // Draw texture
      texture::CTexture* pTargetTexture = nullptr;
      for (auto& it : m_dctMaterials)
      {
        for (uint32_t uIndex = 0; uIndex < static_cast<uint32_t>(material::CMaterial::EType::COUNT); uIndex++)
        {
          material::CMaterial::EType eModifierType = static_cast<material::CMaterial::EType>(uIndex);
          pTargetTexture = it.second->GetTexture(eModifierType);
          if (pTargetTexture) 
          { 
            pTargetTexture->BindTexture(); 
            break;
          }
        }
      }

      // Update buffer
      m_oConstantModelData.GetData().bHasTexture = pTargetTexture ? 1 : 0;
      m_oConstantModelData.GetData().bHasModel = 1;
      m_oConstantModelData.GetData().bUseGlobalLightning = 1;
      bool bOk = m_oConstantModelData.WriteBuffer();
      UNUSED_VARIABLE(bOk);
      assert(bOk);

      // Set constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantModelData.GetBuffer();
      global::dx11::s_pDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);

      // Draw
      global::dx11::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
      global::dx11::s_pDeviceContext->DrawIndexed(static_cast<uint32_t>(m_vctIndices.size()), 0, 0);
    }
    // ------------------------------------
    void CMesh::AddMaterial(render::material::CMaterial* _pMaterial, const uint32_t& _uMaterialIdx)
    {
      m_dctMaterials.emplace(_uMaterialIdx, _pMaterial);
    }
    // ------------------------------------
    HRESULT CMesh::AssignIndexBuffer(TIndexesList& _vctIndices)
    {
      // Clean mesh
      Clean();

      // Init constant check texture
      m_oConstantModelData.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);

      // Get indexes
      m_vctIndices = std::move(_vctIndices);

      // Config index buffer
      D3D11_BUFFER_DESC oIndexBufferDesc = {};
      oIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      oIndexBufferDesc.ByteWidth = static_cast<UINT>((sizeof(uint32_t) * m_vctIndices.size()));
      oIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      oIndexBufferDesc.CPUAccessFlags = 0;

      D3D11_SUBRESOURCE_DATA oSubresourceIndexesData = {};
      oSubresourceIndexesData.pSysMem = m_vctIndices.data();

      return global::dx11::s_pDevice->CreateBuffer(&oIndexBufferDesc, &oSubresourceIndexesData, &m_pIndexBuffer);
    }
    // ------------------------------------
    void CMesh::UpdateVertexColor(ID3D11Buffer* _pVertexBuffer)
    {
      // Mapped vertex buffer
      D3D11_MAPPED_SUBRESOURCE oMappedSubresource;
      HRESULT hResult = global::dx11::s_pDeviceContext->Map(_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &oMappedSubresource);
      UNUSED_VARIABLE(hResult);
      assert(!FAILED(hResult));

      // Get vertex data
      SVertexData* pVertexData = static_cast<SVertexData*>(oMappedSubresource.pData);
      assert(pVertexData);

      // Offset
      uint32_t uOffsetMat = 0;
      uint32_t uMaxTriangles = static_cast<uint32_t>(m_vctIndices.size()) / 3; // Get max triangles
      for (uint32_t uIndex = 0; uIndex < uMaxTriangles; uIndex++)
      {
        // Set material color
        uint32_t uMeshOffset = static_cast<uint32_t>(m_vctIndices.size()) / uMaxTriangles; // Get mesh offset
        for (uint32_t uJ = uOffsetMat; uJ < uMeshOffset + uOffsetMat; uJ++)
        {
          uint32_t uVertexDataIdx = m_vctIndices[uJ];
          auto it = m_dctMaterials.find(pVertexData[uVertexDataIdx].MaterialId);
          if (it != m_dctMaterials.end())
          {
            render::material::CMaterial* pMaterial = it->second;
            pVertexData[uVertexDataIdx].Color = pMaterial->GetDiffuseColor(); // WIP Version
          }
        }
        // Add offset
        uOffsetMat += uMeshOffset;
      }

      // Unmap
      global::dx11::s_pDeviceContext->Unmap(_pVertexBuffer, 0);
    }
    // ------------------------------------
    void CMesh::Clean()
    {
      global::dx11::SafeRelease(m_pIndexBuffer);
      m_oConstantModelData.CleanBuffer();
    }
  }
}

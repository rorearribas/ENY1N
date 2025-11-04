#include "Mesh.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Engine/Render/RenderTypes.h"
#include <cassert>

namespace render
{
  namespace gfx
  {
    // ------------------------------------
    bool SVertexData::operator==(const SVertexData& _other) const
    {
      return Position == _other.Position && Normal == _other.Normal && TexCoord == _other.TexCoord;
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
      ClearMaterial();
    }
    // ------------------------------------
    void CMesh::Draw()
    {
      // Get textures
      texture::CTexture2D<SHADER_RESOURCE>* pDiffuseTexture = m_pMaterial ? m_pMaterial->GetTexture(DIFFUSE) : nullptr;
      texture::CTexture2D<SHADER_RESOURCE>* pNormalTexture = m_pMaterial ? m_pMaterial->GetTexture(NORMAL) : nullptr;
      texture::CTexture2D<SHADER_RESOURCE>* pSpecularTexture = m_pMaterial ? m_pMaterial->GetTexture(SPECULAR) : nullptr;

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

      // Update buffer
      m_oConstantBuffer.GetData().HasDiffuse = static_cast<bool>(pDiffuseTexture);
      m_oConstantBuffer.GetData().HasNormal = static_cast<bool>(pNormalTexture);
      m_oConstantBuffer.GetData().HasSpecular = static_cast<bool>(pSpecularTexture);
      bool bOk = m_oConstantBuffer.WriteBuffer();
      UNUSED_VAR(bOk);
      assert(bOk);

      // Set constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
      global::dx::s_pDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);

      // Draw
      global::dx::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
      global::dx::s_pDeviceContext->DrawIndexed(static_cast<uint32_t>(m_lstIndices.size()), 0, 0);
    }
    // ------------------------------------
    HRESULT CMesh::CreateBuffer(TIndicesList& _lstIndices)
    {
      // Clean mesh
      ClearBuffers();

      // Init constant check texture
      m_oConstantBuffer.Init(global::dx::s_pDevice, global::dx::s_pDeviceContext);

      // Get indexes
      m_lstIndices = std::move(_lstIndices);

      // Config index buffer
      D3D11_BUFFER_DESC oIndexBufferDesc = D3D11_BUFFER_DESC();
      oIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      oIndexBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(uint32_t) * m_lstIndices.size()));
      oIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      oIndexBufferDesc.CPUAccessFlags = 0;

      D3D11_SUBRESOURCE_DATA oSubresourceIndexesData = {};
      oSubresourceIndexesData.pSysMem = m_lstIndices.data();

      return global::dx::s_pDevice->CreateBuffer(&oIndexBufferDesc, &oSubresourceIndexesData, &m_pIndexBuffer);
    }
    // ------------------------------------
    void CMesh::ClearBuffers()
    {
      global::dx::SafeRelease(m_pIndexBuffer);
      m_oConstantBuffer.Clear();
      m_lstIndices.clear();
    }
    // ------------------------------------
    void CMesh::ClearMaterial()
    {
      m_pMaterial.reset();
    }
  }
}

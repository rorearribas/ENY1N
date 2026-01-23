#include "Mesh.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Engine/Render/RenderTypes.h"
#include "Engine/Engine.h"
#include <cassert>

namespace render
{
  namespace gfx
  {
    // ------------------------------------
    CMesh::CMesh(const TIndices& _lstIndices)
    {
      HRESULT hResult = CreateBuffer(_lstIndices);
      UNUSED_VAR(hResult);
#ifdef _DEBUG
      assert(!FAILED(hResult));
#endif // DEBUG
    }
    // ------------------------------------
    CMesh::~CMesh()
    {
      ClearBuffer();
      ClearMaterial();
    }
    // ------------------------------------
    void CMesh::Draw(uint32_t _uInstanceCount)
    {
      // Get textures
      texture::TSharedTexture pDiffuse = m_pMaterial ? m_pMaterial->GetTexture(render::ETexture::DIFFUSE) : nullptr;
      texture::TSharedTexture pNormal = m_pMaterial ? m_pMaterial->GetTexture(render::ETexture::NORMAL) : nullptr;
      texture::TSharedTexture pSpecular = m_pMaterial ? m_pMaterial->GetTexture(render::ETexture::SPECULAR) : nullptr;

      // Texture list
      ID3D11ShaderResourceView* lstTextures[3] =
      {
        pDiffuse ? pDiffuse->GetView() : nullptr,
        pNormal ? pNormal->GetView() : nullptr,
        pSpecular ? pSpecular->GetView() : nullptr,
      };
      // Bind shaders
      global::dx::s_pDeviceContext->PSSetShaderResources(0, ARRAYSIZE(lstTextures), lstTextures);

      // Update buffer
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      render::CRender* pRender = pEngine->GetRender();
      pRender->SetTexturesInfo(pDiffuse.get(), pNormal.get(), pSpecular.get());

      // Set index buffer
      global::dx::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

      // Draw mesh
      (_uInstanceCount > 0) ? global::dx::s_pDeviceContext->DrawIndexedInstanced(m_uIndexCount, _uInstanceCount, 0, 0, 0) : 
      global::dx::s_pDeviceContext->DrawIndexed(m_uIndexCount, 0, 0);
    }
    // ------------------------------------
    HRESULT CMesh::CreateBuffer(const TIndices& _lstIndices)
    {
      // Clean mesh
      ClearBuffer();

      // Get count
      m_uIndexCount = static_cast<uint32_t>(_lstIndices.size());

      // Config index buffer
      D3D11_BUFFER_DESC rBufferDesc = D3D11_BUFFER_DESC();
      rBufferDesc.Usage = D3D11_USAGE_DEFAULT;
      rBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(uint32_t) * _lstIndices.size()));
      rBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      rBufferDesc.CPUAccessFlags = 0;

      D3D11_SUBRESOURCE_DATA rSubresource = D3D11_SUBRESOURCE_DATA();
      rSubresource.pSysMem = _lstIndices.data();

      // Create index buffer
      return global::dx::s_pDevice->CreateBuffer(&rBufferDesc, &rSubresource, &m_pIndexBuffer);
    }
    // ------------------------------------
    void CMesh::ClearBuffer()
    {
      global::dx::SafeRelease(m_pIndexBuffer);
    }
    // ------------------------------------
    void CMesh::ClearMaterial()
    {
      m_pMaterial.reset();
    }
  }
}

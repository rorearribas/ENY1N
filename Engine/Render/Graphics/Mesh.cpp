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
    void CMesh::Draw(uint32_t _uInstanceCount, uint32_t _uStartOffset)
    {
      // Draw mesh
      global::dx::s_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
      global::dx::s_pDeviceContext->DrawIndexedInstanced(m_uIndices, _uInstanceCount, 0, 0, _uStartOffset);
    }
    // ------------------------------------
    HRESULT CMesh::CreateBuffer(const TIndices& _lstIndices)
    {
      // Clean mesh
      ClearBuffer();

      // Get count
      m_uIndices = static_cast<uint32_t>(_lstIndices.size());

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

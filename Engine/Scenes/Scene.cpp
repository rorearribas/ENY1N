#include "Scene.h"
#include "Engine/Global/DX11GlobalInterface.h"
#include <algorithm>

namespace scene
{
  CScene::~CScene()
  {
    DestroyAllPrimitives();
  }
  // ------------------------------------
  void CScene::DrawScene()
  {
    DrawPrimitives();
  }
  // ------------------------------------
  void CScene::DrawPrimitives()
  {
    for (int iIndex = 0; iIndex <= m_iRegisteredPrimitives; iIndex++)
    {
      render::items::CPrimitiveItem* pPrimitiveItem = m_vctPrimitiveItems[iIndex];
      UINT vertex_stride = 3 * sizeof(float);
      UINT vertex_offset = 0;

      global::dx11::s_pDX11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      global::dx11::s_pDX11DeviceContext->IASetInputLayout(pPrimitiveItem->GetInputLayout());

      ID3D11Buffer* pBuffer = pPrimitiveItem->GetBuffer();
      global::dx11::s_pDX11DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &vertex_stride, &vertex_offset);

      // set vertex shader to use and pixel shader to use, and constant buffers for each
      global::dx11::s_pDX11DeviceContext->VSSetShader(pPrimitiveItem->GetVertexShader(), NULL, 0);
      global::dx11::s_pDX11DeviceContext->PSSetShader(pPrimitiveItem->GetPixelShader(), NULL, 0);

      // draw the vertex buffer with the shaders
      global::dx11::s_pDX11DeviceContext->Draw(pPrimitiveItem->GetIndexCount(), 0);
    }
  }
  // ------------------------------------
  void CScene::DestroyAllPrimitives()
  {
    std::for_each(m_vctPrimitiveItems.begin(), m_vctPrimitiveItems.end(), [](render::items::CPrimitiveItem* _pPrimitive) 
    {
      if (_pPrimitive) 
      { 
        delete _pPrimitive; 
        _pPrimitive = nullptr; 
      }
    });
  }
  // ------------------------------------
  render::items::CPrimitiveItem* CScene::CreatePrimitiveItem(std::vector<float>& _vctVertexData)
  {
    if (m_iRegisteredPrimitives >= (s_iMaxPrimitives - 1)) return nullptr;
    render::items::CPrimitiveItem*& pPrimitiveItem = m_vctPrimitiveItems[++m_iRegisteredPrimitives];
    pPrimitiveItem = new render::items::CPrimitiveItem(_vctVertexData);
    return pPrimitiveItem;
  }
}
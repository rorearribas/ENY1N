#pragma once
#include <d3d11.h>
#include "RenderWindow.h"

namespace render
{
  class CRenderSystem
  {
  public:
    CRenderSystem();
    ~CRenderSystem() {}

  private:
    void InitDevice();

    CRenderWindow* m_pRenderWindow = nullptr;

    // DirectX 11
    ID3D11Device* m_pD3D11Device = nullptr;
    ID3D11DeviceContext* m_pD3D11DeviceContext = nullptr;

    IDXGISwapChain* m_pDXGISwapChain = nullptr;
    ID3D11RenderTargetView* m_pD3D11RenderTargetView = nullptr;

  };
}



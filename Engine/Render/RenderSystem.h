#pragma once
#include <d3d11.h>
#include "RenderWindow.h"

namespace render
{
  class CRenderSystem
  {
  public:
    CRenderSystem() {}
    ~CRenderSystem() {}

    void InitDevice();
    void InitWindow(const UINT32& _uWidth, const UINT32& _uHeight);

    ID3D11Device* GetDevice() { return m_pDevice; }
    ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }
    ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView; }

  private:
    CRenderWindow* m_pRenderWindow = nullptr;

    // DirectX 11
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pDeviceContext = nullptr;

    IDXGISwapChain* m_pDXGISwapChain = nullptr;
    ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
  };
}



#pragma once
#include <d3d11.h>
#include "RenderWindow.h"

namespace render
{
  class CRenderSystem
  {
  public:
    CRenderSystem(const UINT32& _uWidth, const UINT32& _uHeight);
    ~CRenderSystem();

    HRESULT InitDevice();
    void UpdateRender();

    const CRenderWindow* GetRenderWindow() { return m_pRenderWindow; }
    const ID3D11Device* GetDevice() { return m_pDevice; }
    const ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }
    const ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView; }

  private:
    HRESULT CreateRenderTargetView();
    void ConfigureViewport();

    // Render window
    CRenderWindow* m_pRenderWindow = nullptr;

    // DirectX
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pDeviceContext = nullptr;
    IDXGISwapChain* m_pDXGISwapChain = nullptr;
    ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
  };
}



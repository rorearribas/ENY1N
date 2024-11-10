#pragma once
#include <d3d11.h>
#include "Libs/Utils/Delegate.h"

namespace global
{
  namespace dx11
  {
    extern ID3D11Device* s_pDX11Device;
    extern ID3D11DeviceContext* s_pDX11DeviceContext;
    extern IDXGISwapChain* s_pDX11SwapChain;
    extern ID3D11RenderTargetView* s_pDX11RenderTargetView;
    extern utils::CDelegate<void(UINT32, UINT32)> s_oWindowResizeDelegate;
  }
}



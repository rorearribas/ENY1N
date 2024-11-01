#include "DX11GlobalInterface.h"

namespace global
{
  namespace dx11
  {
    ID3D11Device* s_pDX11Device = nullptr;
    ID3D11DeviceContext* s_pDX11DeviceContext = nullptr;
    IDXGISwapChain* s_pDX11SwapChain = nullptr;
    ID3D11RenderTargetView* s_pDX11RenderTargetView = nullptr;
  }
}

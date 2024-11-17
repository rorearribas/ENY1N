#include "DX11GlobalInterface.h"

namespace global
{
  namespace dx11
  {
    ID3D11Device* s_pDevice = nullptr;
    ID3D11DeviceContext* s_pDeviceContext = nullptr;
    utils::CDelegate<void(UINT32, UINT32)> s_oWindowResizeDelegate = {};
  }
}

#include "GlobalResources.h"

namespace global
{
  namespace dx11
  {
    ID3D11Device* s_pDevice = nullptr;
    ID3D11DeviceContext* s_pDeviceContext = nullptr;
  }

  namespace window
  {
    HWND s_oHwnd = nullptr;
  }

  namespace delegates
  {
    utils::CDelegate<void(UINT32, UINT32)> s_oWindowResizeDelegate = {};
    utils::CDelegate<void(RAWKEYBOARD*)> s_oUpdateKeyboardDelegate = {};
    utils::CDelegate<void(RAWMOUSE*)> s_oUpdateMouseDelegate = {};
  }
}

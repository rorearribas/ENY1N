#include "GlobalResources.h"

namespace global
{
  // Win handle
  namespace window
  {
    HWND s_oHwnd = nullptr;
  }
  // DirectX
  namespace dx11
  {
    ID3D11Device* s_pDevice = nullptr;
    ID3D11DeviceContext* s_pDeviceContext = nullptr;
  }
  // Global delegates
  namespace delegates
  {
    std::vector<utils::CDelegate<void(uint32_t, uint32_t)>> s_vctOnWindowResizeDelegates;
    utils::CDelegate<void(RAWKEYBOARD*)> s_oOnUpdateKeyboardDelegate;
    utils::CDelegate<void(RAWMOUSE*)> s_oUpdateMouseDelegate;
  }
}

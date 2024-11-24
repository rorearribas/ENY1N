#include "GlobalResources.h"

namespace global
{
  namespace window
  {
    HWND s_oHwnd = nullptr;
  }

  namespace dx11
  {
    ID3D11Device* s_pDevice = nullptr;
    ID3D11DeviceContext* s_pDeviceContext = nullptr;
  }

  namespace delegates
  {
    std::vector<utils::CDelegate<void(UINT32, UINT32)>> s_vctOnWindowResizeDelegates;
    utils::CDelegate<void(RAWKEYBOARD*)> s_oOnUpdateKeyboardDelegate = {};
    utils::CDelegate<void(RAWMOUSE*)> s_oUpdateMouseDelegate = {};
  }
}

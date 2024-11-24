#pragma once
#include <d3d11.h>
#include "Libs/Utils/Delegate.h"

namespace global
{
  namespace window
  {
    extern HWND s_oHwnd;
  }

  namespace dx11
  {
    extern ID3D11Device* s_pDevice;
    extern ID3D11DeviceContext* s_pDeviceContext;

    template<typename _ID3D11Resource>
    inline void SafeRelease(_ID3D11Resource*& _pPtr)
    {
      if (_pPtr)
      {
        _pPtr->Release();
        _pPtr = nullptr;
      }
    }
  }

  namespace delegates
  {
    extern std::vector< utils::CDelegate<void(UINT32, UINT32)>> s_vctOnWindowResizeDelegates;
    extern utils::CDelegate<void(RAWKEYBOARD*)> s_oOnUpdateKeyboardDelegate;
    extern utils::CDelegate<void(RAWMOUSE*)> s_oUpdateMouseDelegate;
  }
}



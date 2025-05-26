#pragma once
#include <d3d11.h>
#include <d3d11_1.h>
#include "Libs/Utils/Delegate.h"

namespace global
{
  // Win handle
  namespace window
  {
    extern HWND s_oHwnd;
  }

  // DirectX
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

  // Release objects
  template<typename T>
  inline bool ReleaseObject(T*& _pPtr)
  {
    if (_pPtr)
    {
      delete _pPtr;
      _pPtr = nullptr;
    }
    return !_pPtr;
  }

  // Global delegates
  namespace delegates
  {
    extern std::vector< utils::CDelegate<void(uint32_t, uint32_t)>> s_vctOnWindowResizeDelegates;
    extern utils::CDelegate<void(RAWKEYBOARD*)> s_oOnUpdateKeyboardDelegate;
    extern utils::CDelegate<void(RAWMOUSE*)> s_oUpdateMouseDelegate;
  }
}



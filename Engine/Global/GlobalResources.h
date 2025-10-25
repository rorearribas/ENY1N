#pragma once
#include "Engine/Managers/MemoryTracker.h"
#include "Libs/Utils/Delegate.h"
#include <d3d11.h>
#include <d3d11_1.h>

namespace global
{
  namespace mem
  {
    extern CMemoryTracker s_oMemoryTracker;
  }

  // Win handle
  namespace window
  {
    extern HWND s_oHwnd;
  }

  // DirectX
  namespace dx
  {
    // Device
    extern ID3D11Device* s_pDevice;
    extern ID3D11DeviceContext* s_pDeviceContext;
    // Vertex Inputs
    extern D3D11_INPUT_ELEMENT_DESC TStandardInputDesc[4];
    // Release memory
    template<typename _ID3D11Resource>
    inline bool SafeRelease(_ID3D11Resource*& _pPtr)
    {
      if (_pPtr)
      {
        _pPtr->Release();
        _pPtr = nullptr;
      }
      return !_pPtr;
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



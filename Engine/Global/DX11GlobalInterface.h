#pragma once
#include <d3d11.h>
#include "Libs/Utils/Delegate.h"

namespace global
{
  namespace dx11
  {
    extern ID3D11Device* s_pDevice;
    extern ID3D11DeviceContext* s_pDeviceContext;
    extern utils::CDelegate<void(UINT32, UINT32)> s_oWindowResizeDelegate;

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
}



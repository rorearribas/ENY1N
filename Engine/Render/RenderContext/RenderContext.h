#pragma once
#include <windows.h>
#include <d3d11.h>
#include <stdint.h>

namespace render
{
  struct TRenderContextDesc
  {
    HWND        pOutputWindow = nullptr;
    uint32_t    uWidth = 1920u;
    uint32_t    uHeight = 1080u;
    uint32_t    uRefreshRate = 60;
    uint32_t    uBufferCount = 1u;
    DXGI_FORMAT eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    bool     bFullscreen = false;
    bool     bVSync = true;
  };

  struct IRenderContext
  {
    virtual HRESULT Init(const TRenderContextDesc& _rRenderContextDesc) = 0;
  };
}


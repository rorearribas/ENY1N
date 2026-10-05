#pragma once
#include "RenderContext.h"
#include "RenderCommandsDX11.h"
#include "RenderDeviceDX11.h"
#include <stdint.h>
#include <d3d11.h>

namespace render
{
  class CRenderContextDX11 : public IRenderContext
  {
  public:
    CRenderContextDX11() = default;
    ~CRenderContextDX11() { Clean(); }

    HRESULT Init(const TRenderContextDesc& _rRenderContextDesc);

    inline CRenderDeviceDX11& GetDevice() { return m_oDevice; }
    inline const CRenderDeviceDX11& GetDevice() const { return m_oDevice; }
    inline CRenderCommandsDX11& GetCommands() { return m_oCommands; }
    inline const CRenderCommandsDX11& GetCommands() const { return m_oCommands; }
    inline IDXGISwapChain* GetSwapChain() const { return m_oDevice.GetSwapChain(); }

  private:
    void Clean();

    CRenderDeviceDX11 m_oDevice;
    CRenderCommandsDX11 m_oCommands;
  };
}

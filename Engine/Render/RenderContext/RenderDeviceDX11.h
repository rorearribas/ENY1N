#pragma once
#include "RenderContext.h"
#include <d3d11.h>

namespace render
{
  class CRenderCommandsDX11;

  class CRenderDeviceDX11
  {
  public:
    CRenderDeviceDX11() = default;
    ~CRenderDeviceDX11() { Clean(); }

    HRESULT Init(const TRenderContextDesc& _rRenderContextDesc, CRenderCommandsDX11& _rCommands);
    void Clean();

    ID3D11Device* GetDevice() const { return m_pDevice; }
    IDXGISwapChain* GetSwapChain() const { return m_pSwapChain; }
    ID3D11Device* operator->() const { return m_pDevice; }
    explicit operator bool() const { return m_pDevice != nullptr; }

  private:
    ID3D11Device* m_pDevice = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;
  };
}

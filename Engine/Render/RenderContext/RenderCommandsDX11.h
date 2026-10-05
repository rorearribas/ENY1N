#pragma once
#include <d3d11.h>

namespace render
{
  class CRenderCommandsDX11
  {
  public:
    CRenderCommandsDX11() = default;
    ~CRenderCommandsDX11() { Clean(); }

    void SetContext(ID3D11DeviceContext* _pContext);
    void Clean();

    ID3D11DeviceContext* GetContext() const { return m_pContext; }
    ID3D11DeviceContext* operator->() const { return m_pContext; }
    explicit operator bool() const { return m_pContext != nullptr; }

  private:
    ID3D11DeviceContext* m_pContext = nullptr;
  };
}

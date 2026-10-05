#include "RenderCommandsDX11.h"

namespace render
{
  void CRenderCommandsDX11::SetContext(ID3D11DeviceContext* _pContext)
  {
    Clean();
    m_pContext = _pContext;
  }
  // ------------------------------------
  void CRenderCommandsDX11::Clean()
  {
    if (m_pContext)
    {
      m_pContext->Release();
      m_pContext = nullptr;
    }
  }
}

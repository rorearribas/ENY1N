#include "RenderContextDX11.h"

namespace render
{
  HRESULT CRenderContextDX11::Init(const TRenderContextDesc& _rRenderContextDesc)
  {
    return m_oDevice.Init(_rRenderContextDesc, m_oCommands);
  }
  // ------------------------------------
  void CRenderContextDX11::Clean()
  {
    m_oCommands.Clean();
    m_oDevice.Clean();
  }
}
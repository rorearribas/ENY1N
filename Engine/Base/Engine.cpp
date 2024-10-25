#include "Engine.h"

namespace engine
{
  void CEngine::InitEngine(const UINT32& _uWidth, const UINT32& _uHeight)
  {
    m_pRenderSystem = new render::CRenderSystem();
    m_pRenderSystem->InitDevice();
    m_pRenderSystem->InitWindow(_uWidth, _uHeight);
  }
}
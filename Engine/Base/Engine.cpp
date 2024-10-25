#include "Engine.h"

namespace engine
{
  CEngine::CEngine()
  {
    m_pRenderSystem = new render::CRenderSystem();
  }

}
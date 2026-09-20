#include "ForwardRenderer.h"
#include "Engine/Render/Render.h"
#include "Engine/Scenes/RenderScene.h"

namespace render
{
  void CForwardRenderer::PrepareFrame()
  {
    // Prepare frame for forward rendering
  }
  // ------------------------------------
  void CForwardRenderer::Draw(scene::CRenderScene& _rRenderScene)
  {
    // Cache primitives
    _rRenderScene.CachePrimitives(*m_pRenderCamera);
#ifdef _DEBUG
    // Cache debug primitives
    _rRenderScene.CacheDebugPrimitives(*m_pRenderCamera);
#endif
    m_pRender->DrawPrimitives(_rRenderScene);
  }
}


#include "ForwardRenderer.h"
#include "Engine/Render/Render.h"
#include "Engine/Scenes/RenderScene.h"

namespace render
{
  namespace internal
  {
    static const wchar_t* s_sForwardPassMrk(L"Forward");
  }
  // ------------------------------------
  void CForwardRenderer::PrepareFrame()
  {
    // Prepare frame for forward rendering
  }
  // ------------------------------------
  void CForwardRenderer::Draw(scene::CRenderScene& _rRenderScene)
  {
    // Deferred pass
    m_pRender->BeginMarker(internal::s_sForwardPassMrk);
    {
      // Cache primitives
      _rRenderScene.CachePrimitives(*m_pRenderCamera);
#ifdef _DEBUG
      // Cache debug primitives
      _rRenderScene.CacheDebugPrimitives(*m_pRenderCamera);
#endif
      m_pRender->DrawPrimitives(_rRenderScene);
    }
    m_pRender->EndMarker();
  }
}


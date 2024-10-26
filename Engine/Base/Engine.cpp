#include "Engine.h"

namespace engine
{
  void CEngine::InitEngine(const UINT32& _uWidth, const UINT32& _uHeight)
  {
    if (!m_pRenderSystem)
    {
      m_pRenderSystem = new render::CRenderSystem(_uWidth, _uHeight);
      m_pRenderSystem->InitDevice();
      m_pRenderSystem->GetRenderWindow()->SetEnabled(true);
    }

    if (!m_pFixedTick)
    {
      m_pFixedTick = new global::CFixedTick(60);
    }
  }
  // ------------------------------------
  void CEngine::Update()
  {
    MSG oMsg = { 0 };
    while (WM_QUIT != oMsg.message)
    {
      if (PeekMessage(&oMsg, nullptr, 0, 0, PM_REMOVE))
      {
        TranslateMessage(&oMsg);
        DispatchMessage(&oMsg);
      }
      else
      {
        m_pFixedTick->UpdateTick();
        m_pRenderSystem->UpdateRender();
      }
    }
  }

}
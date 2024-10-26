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

      m_pTick = new utils::CTick(60);
    }
  }
  // ------------------------------------
  void CEngine::Update()
  {
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
      if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
      else
      {
        m_pTick->Update();
        m_pRenderSystem->UpdateRender();
      }
    }
  }

}
#include "Engine.h"
#include "Engine/Render/RenderItem/PrimitiveItem.h"
#include <cassert>

namespace engine
{
  void CEngine::InitEngine(const UINT32& _uWidth, const UINT32& _uHeight)
  {
    if (!m_pRenderSystem)
    {
      m_pRenderSystem = new render::CRenderSystem(_uWidth, _uHeight);
      HRESULT hr = m_pRenderSystem->InitDevice();
      assert(!FAILED(hr));
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
  // ------------------------------------
  render::items::CPrimitiveItem* CEngine::CreatePrimitiveItem(std::vector<float>& _vctVertexData)
  {
    return m_pRenderSystem ? m_pRenderSystem->CreatePrimitiveItem(_vctVertexData) : nullptr;
  }
}
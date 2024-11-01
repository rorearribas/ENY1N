#include "Engine.h"
#include "Engine/Render/RenderItem/PrimitiveItem.h"
#include <cassert>

namespace engine
{
  CEngine::~CEngine()
  {
    m_pRenderSystem.reset();
    m_pSceneManager.reset();
    m_pFixedTick.reset();
  }
  // ------------------------------------
  void CEngine::InitEngine(const UINT32& _uWidth, const UINT32& _uHeight)
  {
    // Create render system
    if (!m_pRenderSystem)
    {
      m_pRenderSystem = std::make_unique<render::CRenderSystem>(_uWidth, _uHeight);
      HRESULT hr = m_pRenderSystem->InitDevice();
      assert(!FAILED(hr));
    }
    // Create fixed tick
    if (!m_pFixedTick)
    {
      m_pFixedTick = std::make_unique<global::CFixedTick>(60);
    }
    // Create scene manager
    if (!m_pSceneManager)
    {
      m_pSceneManager = std::make_unique<scene::CSceneManager>();
      m_pSceneManager->InitScenes();
    }
    // Show window
    m_pRenderSystem->GetRenderWindow()->SetEnabled(true);
  }
  // ------------------------------------
  void CEngine::UpdateEngine()
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
        for (auto& pScene : m_pSceneManager->GetScenes())
        {
          m_pRenderSystem->Update(pScene);
        }
      }
    }
  }
  // ------------------------------------
  render::items::CPrimitiveItem* CEngine::CreatePrimitiveItem(std::vector<float>& _vctVertexData, const UINT32& _uSceneIndex)
  {
    return m_pSceneManager ? m_pSceneManager->CreatePrimitiveItem(_vctVertexData, _uSceneIndex) : nullptr;
  }
}
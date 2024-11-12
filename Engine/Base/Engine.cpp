#include "Engine.h"
#include "Engine/Render/Primitives/Primitive.h"
#include "Engine/Global/DX11GlobalInterface.h"
#include <cassert>

namespace engine
{
  CEngine::~CEngine()
  {
    m_pSceneManager.reset();
    m_pRender.reset();
  }
  // ------------------------------------
  void CEngine::InitEngine(const UINT32& _uWidth, const UINT32& _uHeight)
  {
    // Create render system
    m_pRender = std::make_unique<render::CRender>(_uWidth, _uHeight);
    HRESULT hr = m_pRender->Init();
    assert(!FAILED(hr));

    // Create scene manager
    m_pSceneManager = std::make_unique<scene::CSceneManager>();
    m_pSceneManager->InitScenes();
    m_pSceneManager->SetSceneEnabled(0, true);

    // Create fixed tick
    m_pTickRate = std::make_unique<tick::CTickRate>();

    // Show window
    m_pRender->GetRenderWindow()->SetEnabled(true);
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
        Loop();
      }
    }
  }
  // ------------------------------------
  void CEngine::Loop()
  {
    // Update tick rate
    m_pTickRate->UpdateTick();

    // Update current scene
    for (scene::CScene* pScene : m_pSceneManager->GetScenes())
    {
      if (pScene->IsEnabled())
      {
        m_pRender->DrawScene(pScene);
      }
    }
  }
  // ------------------------------------
  render::primitive::CPrimitive* CEngine::CreatePrimitive(const std::vector<render::primitive::CPrimitive::SPrimitiveInfo>& _vctVertexData, const UINT32& _uSceneIndex)
  {
    return m_pSceneManager ? m_pSceneManager->CreatePrimitive(_vctVertexData, _uSceneIndex) : nullptr;
  }
  // ------------------------------------
  render::primitive::CPrimitive* CEngine::CreatePrimitive(const render::primitive::CPrimitive::EPrimitiveType& _ePrimitiveType, const UINT32& _uSceneIndex /*= 0*/)
  {
    return m_pSceneManager ? m_pSceneManager->CreatePrimitive(_ePrimitiveType, _uSceneIndex) : nullptr;
  }
  // ------------------------------------
  void CEngine::DestroyPrimitive(const render::primitive::CPrimitive* _pPrimitive)
  {
    assert(_pPrimitive);
    m_pSceneManager->DestroyPrimitive(_pPrimitive);
  }
  // ------------------------------------
  void CEngine::DestroyAllPrimimitives(const UINT32& _uSceneIndex)
  {
    m_pSceneManager->DestroyAllPrimimitives(_uSceneIndex);
  }
}
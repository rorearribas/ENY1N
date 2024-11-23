#include "Engine.h"
#include "Engine/Render/Primitives/Primitive.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Input/InputManager.h"
#include <cassert>

#include <iostream>
#include <algorithm>
#include "c:/Users/Ruben/Downloads/imgui-master/imgui.h"
namespace engine
{
  namespace internal_engine
  {
    const float s_fMaxFixedDeltaTime = 1.f / 10.f;
    const int s_iTargetMaxFPS = 60;
  }

  CEngine::~CEngine()
  {
    m_pSceneManager.reset();
    m_pRender.reset();
  }
  // ------------------------------------
  void CEngine::InitEngine(UINT32 _uWidth, UINT32 _uHeight)
  {
    assert(!m_bInitialized);

    // Create render
    m_pRender = std::make_unique<render::CRender>(_uWidth, _uHeight);

    // Create scene manager
    m_pSceneManager = std::make_unique<scene::CSceneManager>();
    m_pSceneManager->SetSceneEnabled(0, true);

    // Marked as initialized
    m_bInitialized = true;
  }
  // ------------------------------------
  void CEngine::BeginDraw()
  {
    // Begin
    m_pRender->BeginDraw();
  }
  void CEngine::Draw()
  {
    // Draw the current scene
    m_pRender->Draw(m_pSceneManager->GetCurrentScene());
  }
  // ------------------------------------
  void CEngine::EndDraw()
  {
    // End 
    m_pRender->EndDraw();
  }
  // ------------------------------------
  void CEngine::Update(float _fDeltaTime)
  {
    // Update
    m_pRender->Update(_fDeltaTime);
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
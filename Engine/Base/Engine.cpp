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
    const float s_fMaxFixedDeltaTime = 1.f / 15.f;
    const int s_iTargetMaxFPS = 60;
  }

  CEngine::~CEngine()
  {
    m_pSceneManager.reset();
    m_pRender.reset();
    m_pTickRate.reset();
  }
  // ------------------------------------
  void CEngine::InitEngine(const UINT32& _uWidth, const UINT32& _uHeight)
  {
    assert(!m_bInitialized);

    // Create render
    m_pRender = std::make_unique<render::CRender>(_uWidth, _uHeight);

    // Create scene manager
    m_pSceneManager = std::make_unique<scene::CSceneManager>();
    m_pSceneManager->SetSceneEnabled(0, true);

    // Create fixed tick
    m_pTickRate = std::make_unique<tick::CTickRate>(internal_engine::s_iTargetMaxFPS);
    SetTargetFPS(internal_engine::s_iTargetMaxFPS);

    // Marked as initialized
    m_bInitialized = true;
  }
  // ------------------------------------
  void CEngine::UpdateEngine()
  {
    // Begin
    m_pTickRate->BeginFrame();
    m_pRender->BeginDraw();

    // Calculate 
    float fDeltaTime = m_pTickRate->DeltaTime();
    float fSeconds = std::clamp(fDeltaTime, 0.0f, internal_engine::s_fMaxFixedDeltaTime);
    m_fFixedDeltaAccumulator += fSeconds;

    // Call fixed update
     while (m_fFixedDeltaAccumulator >= m_fFixedDelta)
     {
       Update(m_fFixedDelta);
       m_fFixedDeltaAccumulator -= m_fFixedDelta;
     }

    // Dibujar la escena
    m_pRender->Draw(m_pSceneManager->GetCurrentScene());

    // Imgui utils
    if (ImGui::Button("Set max fps to unlimited"))
    {
      SetTargetFPS(999);
    }
    if (ImGui::Button("Set max fps to 144"))
    {
      SetTargetFPS(144);
    }
    if (ImGui::Button("Set max fps to 60"))
    {
      SetTargetFPS(60);
    }
    if (ImGui::Button("Set max fps to 30"))
    {
      SetTargetFPS(30);
    }

    // End 
    m_pRender->EndDraw();
    m_pTickRate->EndFrame();
  }
  // ------------------------------------
  void CEngine::Update(float _fDeltaTime)
  {
    // Update
    m_pRender->Update(_fDeltaTime);

    // Flush input manager
    input::CInputManager* pInputManager = input::CInputManager::GetInstance();
    pInputManager->Flush();
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
  void CEngine::SetTargetFPS(int _iMaxFPS)
  {
    m_pTickRate->SetMaxFPS(_iMaxFPS);
    m_fFixedDelta = static_cast<float>(1.0f / _iMaxFPS);
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
#include "Engine.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Input/InputManager.h"
#include <cassert>

#include <iostream>
#include "Libs/ImGui/imgui.h"

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

    // Create camera
    m_pCamera = std::make_unique<render::CCamera>();
    m_pCamera->SetPosition({ 0.0f, 0.0f, -10.0f });

    // Create scene manager
    m_pSceneManager = std::make_unique<scene::CSceneManager>();
    m_pSceneManager->SetSceneEnabled(0, true);

    // Set delegate
    utils::CDelegate<void(UINT32, UINT32)> oResizeDelegate(&CEngine::OnWindowResizeEvent, this);
    global::delegates::s_vctOnWindowResizeDelegates.push_back(oResizeDelegate);

    // Marked as initialized
    m_bInitialized = true;
  }
  // ------------------------------------
  void CEngine::OnWindowResizeEvent(UINT32 _uX, UINT32 _uY)
  {
    m_pCamera->SetAspectRatio(static_cast<float>(_uX / static_cast<float>(_uY)));
  }
  // ------------------------------------
  void CEngine::BeginDraw()
  {
    m_pRender->BeginDraw(); // Begin
  }
  void CEngine::DrawScene()
  {
    // Draw the current scene using the scene manager
    if (m_pSceneManager->GetCurrentScene())
    {
      m_pSceneManager->Draw();
    }

    // Test
    ImGui::Begin("Handler");

    if (ImGui::Button("Destroy models"))
    {
      engine::CEngine::GetInstance()->DestroyAllModels();
    }
    if (ImGui::Button("Fov 90"))
    {
      m_pCamera->SetFov(90.0f);
    }
    if (ImGui::Button("Fov 45"))
    {
      m_pCamera->SetFov(45.0f);
    }

    ImGui::End();
  }
  // ------------------------------------
  void CEngine::EndDraw()
  {
    m_pRender->EndDraw(); // End 
  }
  // ------------------------------------
  void CEngine::Update(float _fDeltaTime)
  {
    // Update camera
    m_pCamera->Update(_fDeltaTime);
  }
  // ------------------------------------
  render::graphics::CPrimitive* CEngine::CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData, const UINT32& _uSceneIndex)
  {
    return m_pSceneManager ? m_pSceneManager->CreatePrimitive(_vctVertexData, _uSceneIndex) : nullptr;
  }
  // ------------------------------------
  render::graphics::CPrimitive* CEngine::CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType, const UINT32& _uSceneIndex /*= 0*/)
  {
    return m_pSceneManager ? m_pSceneManager->CreatePrimitive(_ePrimitiveType, _uSceneIndex) : nullptr;
  }
  // ------------------------------------
  render::graphics::CModel* CEngine::CreateModel(const char* _sModelPath, const char* _sBaseMltDir, const UINT32& _uSceneIndex)
  {
    return m_pSceneManager ? m_pSceneManager->CreateModel(_sModelPath, _sBaseMltDir, _uSceneIndex) : nullptr;
  }
  // ------------------------------------
  void CEngine::DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_)
  {
    assert(pPrimitive_);
    m_pSceneManager->DestroyPrimitive(pPrimitive_);
  }
  // ------------------------------------
  void CEngine::DestroyAllPrimimitives(const UINT32& _uSceneIndex)
  {
    m_pSceneManager->DestroyAllPrimimitives(_uSceneIndex);
  }
  // ------------------------------------
  void CEngine::DestroyModel(render::graphics::CModel*& pModel_)
  {
    assert(pModel_);
    m_pSceneManager->DestroyModel(pModel_);
  }
  // ------------------------------------
  void CEngine::DestroyAllModels(const UINT32& _uSceneIndex)
  {
    m_pSceneManager->DestroyAllModels(_uSceneIndex);
  }
}
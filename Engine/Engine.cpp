#include "Engine.h"

#include "Engine/Render/Render.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Managers/InputManager.h"
#include "Engine/Shaders/Shader.h"

#include "Libs/Macros/GlobalMacros.h"
#include <cassert>
#include <iostream>

namespace engine
{
  CEngine::~CEngine()
  {
    m_pSceneManager.reset();
    m_pRender.reset();
  }
  // ------------------------------------
  void CEngine::Init(uint32_t _uWidth, uint32_t _uHeight)
  {
#ifdef _DEBUG
    assert(!m_bInitialized);
#endif // DEBUG
    LOG("Initializing engine...");

    // Create render
    m_pRender = std::make_unique<render::CRender>(_uWidth, _uHeight);

    // Create scene manager
    m_pSceneManager = std::make_unique<scene::CSceneManager>();
    m_pSceneManager->SetSceneEnabled(0, true);

    // Set cameras
    m_pRender->SetRenderCamera(m_pSceneManager->GetRenderCamera());
    m_pRender->SetShadowCamera(m_pSceneManager->GetShadowCamera());

    // Set delegate
    utils::CDelegate<void(uint32_t, uint32_t)> rDelegate(&CEngine::OnWindowResizeEvent, this);
    global::delegates::s_lstOnWindowResizeDelegates.push_back(rDelegate);

    // Marked as initialized
    m_bInitialized = true;
    SUCCESS_LOG("Engine initialized correctly!");
  }
  // ------------------------------------
  void CEngine::PrepareFrame()
  {
    // Prepare
    m_pRender->PrepareFrame();
  }
  // ------------------------------------
  void CEngine::Draw()
  {
    // Draw
#ifdef _DEBUG
    m_pSceneManager->GetCurrentScene()->DrawOctree();
#endif // _DEBUG
    m_pRender->Draw(m_pSceneManager->GetCurrentScene());

    // Flush camera state
    m_pSceneManager->GetRenderCamera()->FlushState();
    m_pSceneManager->GetShadowCamera()->FlushState();
  }
  // ------------------------------------
  utils::CWeakPtr<render::gfx::CPrimitive> const CEngine::CreatePrimitive(render::EPrimitive _eType, render::ERenderMode _eRenderMode, uint32_t _uSceneIndex)
  {
    return m_pSceneManager->CreatePrimitive(_eType, _eRenderMode, _uSceneIndex);
  }
  // ------------------------------------
  bool CEngine::DestroyPrimitive(utils::CWeakPtr<render::gfx::CPrimitive> _pPrimitive_)
  {
#ifdef _DEBUG
    assert(_pPrimitive_.IsValid());
#endif // DEBUG
    return m_pSceneManager->DestroyPrimitive(_pPrimitive_);
  }
  // ------------------------------------
  utils::CWeakPtr<render::gfx::CModel> const CEngine::LoadModel(const char* _sModelPath, uint32_t _uSceneIndex)
  {
    return m_pSceneManager->LoadModel(_sModelPath, _uSceneIndex);
  }
  // ------------------------------------
  bool CEngine::DestroyModel(utils::CWeakPtr<render::gfx::CModel> _wpModel_, uint32_t _uSceneIndex)
  {
#ifdef _DEBUG
    assert(_wpModel_.IsValid());
#endif // DEBUG
    return m_pSceneManager->DestroyModel(_wpModel_, _uSceneIndex);
  }
  //// ------------------------------------
  utils::CWeakPtr<render::lights::CDirectionalLight> const CEngine::CreateDirectionalLight(uint32_t _uSceneIndex)
  {
    return m_pSceneManager->CreateDirectionalLight(_uSceneIndex);
  }
  // ------------------------------------
  utils::CWeakPtr<render::lights::CPointLight> const CEngine::CreatePointLight(uint32_t _uSceneIndex)
  {
    return m_pSceneManager->CreatePointLight(_uSceneIndex);
  }
  // ------------------------------------
  utils::CWeakPtr<render::lights::CSpotLight> const CEngine::CreateSpotLight(uint32_t _uSceneIndex)
  {
    return m_pSceneManager->CreateSpotLight(_uSceneIndex);
  }
  // ------------------------------------
  bool CEngine::DestroyLight(utils::CWeakPtr<render::lights::CLight> _wpLight)
  {
#ifdef _DEBUG
    assert(_wpLight.IsValid());
#endif // DEBUG
    return m_pSceneManager->DestroyLight(_wpLight);
  }
#ifdef _DEBUG
  // ------------------------------------
  void CEngine::DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color,
    float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode)
  {
    m_pSceneManager->DrawCapsule(_v3Pos, _v3Rot, _v3Color, _fRadius, _fHeight, _iSubvH, _iSubvV, _eRenderMode);
  }
  // ------------------------------------
  void CEngine::DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    m_pSceneManager->DrawCube(_v3Pos, _v3Rot, _v3Size, _v3Color, _eRenderMode);
  }
  // ------------------------------------
  void CEngine::DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    m_pSceneManager->DrawSphere(_v3Pos, _fRadius, _iSubvH, _iSubvV, _v3Color, _eRenderMode);
  }
  // ------------------------------------
  void CEngine::DrawPlane(const math::CPlane& _rPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    m_pSceneManager->DrawPlane(_rPlane, _v3Size, _v3Color, _eRenderMode);
  }
  // ------------------------------------
  void CEngine::DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color)
  {
    m_pSceneManager->DrawLine(_v3Start, _v3Dest, _v3Color);
  }
#endif
  // ------------------------------------
  void CEngine::OnWindowResizeEvent(uint32_t _uX, uint32_t _uY)
  {
    GetCamera()->SetAspectRatio(static_cast<float>(_uX / static_cast<float>(_uY)));
  }
  // ------------------------------------
}
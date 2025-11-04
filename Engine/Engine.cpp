#include "Engine.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Managers/InputManager.h"
#include "Engine/Shaders/Shader.h"

#include <cassert>
#include <iostream>

namespace engine
{
  CEngine::~CEngine()
  {
    global::ReleaseObject(m_pSceneManager);
    global::ReleaseObject(m_pCamera);
    global::ReleaseObject(m_pRender);
  }
  // ------------------------------------
  void CEngine::Init(uint32_t _uWidth, uint32_t _uHeight)
  {
    assert(!m_bInitialized);
    LOG("Initializing engine...");

    // Create render
    m_pRender = new render::CRender(_uWidth, _uHeight);

    // Create camera
    m_pCamera = new render::CCamera();

    // Create scene manager
    m_pSceneManager = new scene::CSceneManager();
    m_pSceneManager->SetSceneEnabled(0, true);

    // Set delegate
    utils::CDelegate<void(uint32_t, uint32_t)> oResizeDelegate(&CEngine::OnWindowResizeEvent, this);
    global::delegates::s_lstOnWindowResizeDelegates.push_back(oResizeDelegate);

    // Marked as initialized
    m_bInitialized = true;
    SUCCESS_LOG("Engine initialized correctly!");
  }
  // ------------------------------------
  void CEngine::PushBeginDraw()
  {
    // Begin
    m_pRender->BeginDraw();
  }
  // ------------------------------------
  void CEngine::PushDraw()
  {
    // Draw current scene
    m_pRender->SetCurrentCamera(m_pCamera);
    m_pRender->Draw(m_pSceneManager->GetCurrentScene());
  }
  // ------------------------------------
  void CEngine::PushEndDraw()
  {
    // End 
    m_pRender->EndDraw();
  }
  // ------------------------------------
  render::gfx::CPrimitive* const CEngine::CreatePrimitive(render::gfx::EPrimitiveType _eType, render::ERenderMode _eRenderMode, uint32_t _uSceneIndex)
  {
    return m_pSceneManager->CreatePrimitive(_eType, _eRenderMode, _uSceneIndex);
  }
  // ------------------------------------
  render::gfx::CModel* const CEngine::CreateModel(const char* _sModelPath, uint32_t _uSceneIndex)
  {
    return m_pSceneManager->CreateModel(_sModelPath, _uSceneIndex);
  }
  // ------------------------------------
  render::lights::CDirectionalLight* const CEngine::CreateDirectionalLight(uint32_t _uSceneIndex)
  {
    return m_pSceneManager->CreateDirectionalLight(_uSceneIndex);
  }
  // ------------------------------------
  render::lights::CPointLight* const CEngine::CreatePointLight(uint32_t _uSceneIndex)
  {
    return m_pSceneManager->CreatePointLight(_uSceneIndex);
  }
  // ------------------------------------
  render::lights::CSpotLight* const CEngine::CreateSpotLight(uint32_t _uSceneIndex)
  {
    return m_pSceneManager->CreateSpotLight(_uSceneIndex);
  }
  // ------------------------------------
  void CEngine::DestroyPrimitive(render::gfx::CPrimitive*& pPrimitive_)
  {
    assert(pPrimitive_);
    m_pSceneManager->DestroyPrimitive(pPrimitive_);
  }
  // ------------------------------------
  void CEngine::DestroyModel(render::gfx::CModel*& pModel_)
  {
    assert(pModel_);
    m_pSceneManager->DestroyModel(pModel_);
  }
  // ------------------------------------
  void CEngine::DestroyLight(render::lights::CBaseLight*& pLight_)
  {
    assert(pLight_);
    m_pSceneManager->DestroyLight(pLight_);
  }
  // ------------------------------------
  void CEngine::DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color,
    float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode)
  {
    m_pSceneManager->DrawCapsule(_v3Pos, _v3Rot, _v3Color, _fRadius, _fHeight, _iSubvH, _iSubvV, _eRenderMode);
  }
  // ------------------------------------
  void CEngine::DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Size, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    m_pSceneManager->DrawCube(_v3Pos, _v3Size, _v3Rot, _v3Color, _eRenderMode);
  }
  // ------------------------------------
  void CEngine::DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    m_pSceneManager->DrawSphere(_v3Pos, _fRadius, _iSubvH, _iSubvV, _v3Color, _eRenderMode);
  }
  // ------------------------------------
  void CEngine::DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    m_pSceneManager->DrawPlane(_oPlane, _v3Size, _v3Color, _eRenderMode);
  }
  // ------------------------------------
  void CEngine::DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color)
  {
    m_pSceneManager->DrawLine(_v3Start, _v3Dest, _v3Color);
  }
  // ------------------------------------
  void CEngine::OnWindowResizeEvent(uint32_t _uX, uint32_t _uY)
  {
    m_pCamera->SetAspectRatio(static_cast<float>(_uX / static_cast<float>(_uY)));
  }
  // ------------------------------------
}
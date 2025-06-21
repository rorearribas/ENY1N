#include "Engine.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Managers/InputManager.h"
#include "Engine/Shaders/Shader.h"

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
    assert(!m_bInitialized);

    // Create render
    m_pRender = std::make_unique<render::CRender>(_uWidth, _uHeight);

    // Create camera
    m_pCamera = std::make_unique<render::CCamera>();

    // Create scene manager
    m_pSceneManager = std::make_unique<scene::CSceneManager>();
    m_pSceneManager->SetSceneEnabled(0, true);

    // Set delegate
    utils::CDelegate<void(uint32_t, uint32_t)> oResizeDelegate(&CEngine::OnWindowResizeEvent, this);
    global::delegates::s_vctOnWindowResizeDelegates.push_back(oResizeDelegate); 

    // Marked as initialized
    m_bInitialized = true;
  }
  // ------------------------------------
  void CEngine::PushBeginDraw()
  {
    m_pRender->BeginDraw(); // Begin
  }
  // ------------------------------------
  void CEngine::PushDrawProcess()
  {
    // Draw current scene
    m_pRender->Draw(m_pSceneManager->GetCurrentScene());
  }
  // ------------------------------------
  void CEngine::PushEndDraw()
  {
    m_pRender->EndDraw(); // End 
  }
  // ------------------------------------
  void CEngine::DrawCapsule(const math::CVector3& _v3Pos, float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    m_pSceneManager->DrawCapsule(_v3Pos, _fRadius, _fHeight, _iSubvH, _iSubvV, _v3Color, _eRenderMode);
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
  render::graphics::CPrimitive* const CEngine::CreatePrimitive
  (
    render::graphics::CPrimitive::EPrimitiveType _ePrimitiveType, 
    render::ERenderMode _eRenderMode,
    uint32_t _uSceneIndex
  )
  {
    return m_pSceneManager->CreatePrimitive(_ePrimitiveType, _eRenderMode,  _uSceneIndex);
  }
  // ------------------------------------
  render::graphics::CModel* const CEngine::CreateModel(const char* _sModelPath, const char* _sBaseMltDir, uint32_t _uSceneIndex)
  {
    return m_pSceneManager->CreateModel(_sModelPath, _sBaseMltDir, _uSceneIndex);
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
  void CEngine::DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_)
  {
    assert(pPrimitive_);
    m_pSceneManager->DestroyPrimitive(pPrimitive_);
  }
  // ------------------------------------
  void CEngine::DestroyModel(render::graphics::CModel*& pModel_)
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
  void CEngine::OnWindowResizeEvent(uint32_t _uX, uint32_t _uY)
  {
    m_pCamera->SetAspectRatio(static_cast<float>(_uX / static_cast<float>(_uY)));
  }
  // ------------------------------------
}
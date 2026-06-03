#include "SceneManager.h"
#include <algorithm>
#include <cassert>

namespace scene
{
  // ------------------------------------
  void CSceneManager::SetSceneEnabled(uint32_t _uIndex, bool _bEnabled) const
  {
    if (_uIndex > (m_lstScenes.GetMaxSize() - 1))
    {
      return;
    }
    m_pCurrentScene = m_lstScenes[_uIndex];
#ifdef _DEBUG
    assert(m_pCurrentScene.IsValid());
#endif // _DEBUG
    m_pCurrentScene->SetEnabled(_bEnabled);
  }
  // ------------------------------------
  utils::CWeakPtr<render::gfx::CModel> const CSceneManager::LoadModel(const char* _sModelPath, uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.GetSize() - 1))
    {
      return utils::CWeakPtr<render::gfx::CModel>();
    }
    utils::CWeakPtr<scene::CRenderScene> pScene = m_lstScenes[_uSceneIndex];
#ifdef _DEBUG
    assert(pScene.IsValid());
#endif // _DEBUG
    return pScene->LoadModel(_sModelPath);
  }
  // ------------------------------------
  bool CSceneManager::DestroyModel(utils::CWeakPtr<render::gfx::CModel> _wpModel_, uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.GetSize() - 1))
    {
      return false;
    }
    utils::CWeakPtr<scene::CRenderScene> pScene = m_lstScenes[_uSceneIndex];
#ifdef _DEBUG
    assert(pScene.IsValid());
#endif // _DEBUG
    return pScene->DestroyModel(_wpModel_);
  }
  // ------------------------------------
  utils::CWeakPtr<render::gfx::CPrimitive> const CSceneManager::CreatePrimitive(const render::EPrimitive& _ePrimitiveType, render::ERenderMode _eRenderMode, uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.GetSize() - 1))
    {
      return utils::CWeakPtr<render::gfx::CPrimitive>();
    }

    utils::CWeakPtr<scene::CRenderScene> pScene = m_lstScenes[_uSceneIndex];
#ifdef _DEBUG
    assert(pScene.IsValid());
#endif // _DEBUG
    return pScene->CreatePrimitive(_ePrimitiveType, _eRenderMode);
  }
  // ------------------------------------
  bool CSceneManager::DestroyPrimitive(utils::CWeakPtr<render::gfx::CPrimitive> _pPrimitive_, uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.GetSize() - 1))
    {
      return false;
    }
    utils::CWeakPtr<scene::CRenderScene> pScene = m_lstScenes[_uSceneIndex];
#ifdef _DEBUG
    assert(pScene.IsValid());
#endif // _DEBUG
    return pScene->DestroyPrimitive(_pPrimitive_);
  }
  // ------------------------------------
  utils::CWeakPtr<render::lights::CDirectionalLight> const CSceneManager::CreateDirectionalLight(uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.GetSize() - 1))
    {
      return utils::CWeakPtr<render::lights::CDirectionalLight>();
    }
    utils::CWeakPtr<scene::CRenderScene> pScene = m_lstScenes[_uSceneIndex];
#ifdef _DEBUG
    assert(pScene.IsValid());
#endif // _DEBUG
    return pScene->CreateDirectionalLight();
  }
  // ------------------------------------
  utils::CWeakPtr<render::lights::CPointLight> const CSceneManager::CreatePointLight(uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.GetSize() - 1))
    {
      return utils::CWeakPtr<render::lights::CPointLight>();
    }    
    utils::CWeakPtr<scene::CRenderScene> pScene = m_lstScenes[_uSceneIndex];
#ifdef _DEBUG
    assert(pScene.IsValid());
#endif // _DEBUG
    return pScene->CreatePointLight();
  }
  // ------------------------------------
  utils::CWeakPtr<render::lights::CSpotLight> const CSceneManager::CreateSpotLight(uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.GetSize() - 1))
    {
      return utils::CWeakPtr<render::lights::CSpotLight>();
    }
    utils::CWeakPtr<scene::CRenderScene> pScene = m_lstScenes[_uSceneIndex];
#ifdef _DEBUG
    assert(pScene.IsValid());
#endif // _DEBUG
    return pScene->CreateSpotLight();
  }
  // ------------------------------------
  bool CSceneManager::DestroyLight(utils::CWeakPtr<render::lights::CLight> _wpLight, uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.GetSize() - 1)) 
    {
      return false;
    }
    utils::CWeakPtr<scene::CRenderScene> pScene = m_lstScenes[_uSceneIndex];
#ifdef _DEBUG
    assert(pScene.IsValid());
#endif // _DEBUG
    return pScene->DestroyLight(_wpLight);
  }
  // ------------------------------------
  void CSceneManager::DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color,
    float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode)
  {
    if (m_pCurrentScene.IsValid())
    {
      m_pCurrentScene->DrawCapsule(_v3Pos, _v3Rot, _v3Color, _fRadius, _fHeight, _iSubvH, _iSubvV, _eRenderMode);
    }
  }
  // ------------------------------------
  void CSceneManager::DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_pCurrentScene.IsValid())
    {
      m_pCurrentScene->DrawCube(_v3Pos, _v3Rot, _v3Size, _v3Color, _eRenderMode);
    }
  }
  // ------------------------------------
  void CSceneManager::DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_pCurrentScene.IsValid())
    {
      m_pCurrentScene->DrawSphere(_v3Pos, _fRadius, _iSubvH, _iSubvV, _v3Color, _eRenderMode);
    }
  }
  // ------------------------------------
  void CSceneManager::DrawPlane(const math::CPlane& _rPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_pCurrentScene.IsValid())
    {
      m_pCurrentScene->DrawPlane(_rPlane, _v3Size, _v3Color, _eRenderMode);
    }
  }
  // ------------------------------------
  void CSceneManager::DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color)
  {
    if (m_pCurrentScene.IsValid())
    {
      m_pCurrentScene->DrawLine(_v3Start, _v3Dest, _v3Color);
    }
  }
  // ------------------------------------
  void CSceneManager::Setup()
  {
    // Create scenes
    for (int iIdx = 0; iIdx < m_lstScenes.GetMaxSize(); iIdx++)
    {
      utils::CWeakPtr<scene::CRenderScene> pScene = m_lstScenes.Create(iIdx);
      pScene->SetEnabled(false);
    }

    // Create cameras
    for (int iIdx = 0; iIdx < m_lstCameras.GetMaxSize(); iIdx++)
    {
      m_lstCameras.Create();
    }
  }
  // ------------------------------------
  void CSceneManager::Clear()
  {
    m_lstScenes.Clear();
    m_lstCameras.Clear();
  }
}
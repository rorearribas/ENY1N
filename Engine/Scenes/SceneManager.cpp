#include "SceneManager.h"
#include <algorithm>
#include <cassert>

namespace scene
{
  // ------------------------------------
  void CSceneManager::SetSceneEnabled(uint32_t _uIndex, bool _bEnabled) const
  {
    if (_uIndex > (s_iMaxScenes - 1))
    {
      return;
    }
    m_pCurrentScene = m_lstScenes[_uIndex];
    m_pCurrentScene->SetEnabled(_bEnabled);
  }
  // ------------------------------------
  render::gfx::CPrimitive* const CSceneManager::CreatePrimitive
  (
    const render::EPrimitive& _ePrimitiveType,
    render::ERenderMode _eRenderMode,
    uint32_t _uSceneIndex
  )
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.size() - 1))
    {
      return nullptr;
    }
    scene::CScene* pScene = m_lstScenes[_uSceneIndex];
    return pScene->CreatePrimitive(_ePrimitiveType, _eRenderMode);
  }
  // ------------------------------------
  bool CSceneManager::DestroyPrimitive(render::gfx::CPrimitive*& _pPrimitive_, uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.size() - 1))
    {
      return false;
    }
    scene::CScene* pScene = m_lstScenes.at(_uSceneIndex);
    return pScene->DestroyPrimitive(_pPrimitive_);
  }
  // ------------------------------------
  utils::CWeakPtr<render::gfx::CModel> const CSceneManager::LoadModel(const char* _sModelPath, uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.size() - 1))
    {
      return utils::CWeakPtr<render::gfx::CModel>();
    }
    scene::CScene* pScene = m_lstScenes[_uSceneIndex];
    return pScene->LoadModel(_sModelPath);
  }
  // ------------------------------------
  bool CSceneManager::DestroyModel(utils::CWeakPtr<render::gfx::CModel> _wpModel_, uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.size() - 1))
    {
      return false;
    }
    scene::CScene* pScene = m_lstScenes.at(_uSceneIndex);
    return pScene->DestroyModel(_wpModel_);
  }
  // ------------------------------------
  render::lights::CDirectionalLight* const CSceneManager::CreateDirectionalLight(uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.size() - 1))
    {
      return nullptr;
    }
    scene::CScene* pScene = m_lstScenes[_uSceneIndex];
    return pScene->CreateDirectionalLight();
  }
  // ------------------------------------
  render::lights::CPointLight* const CSceneManager::CreatePointLight(uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.size() - 1))
    {
      return nullptr;
    }    
    scene::CScene* pScene = m_lstScenes[_uSceneIndex];
    return pScene->CreatePointLight();
  }
  // ------------------------------------
  render::lights::CSpotLight* const CSceneManager::CreateSpotLight(uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.size() - 1))
    {
      return nullptr;
    }
    scene::CScene* pScene = m_lstScenes[_uSceneIndex];
    return pScene->CreateSpotLight();
  }
  // ------------------------------------
  bool CSceneManager::DestroyLight(render::lights::CLight*& _pLight_, uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_lstScenes.size() - 1)) 
    {
      return false;
    }
    scene::CScene* pScene = m_lstScenes.at(_uSceneIndex);
    return pScene->DestroyLight(_pLight_);
  }
  // ------------------------------------
  void CSceneManager::DrawCapsule(const math::CVector3& _v3Pos, const math::CVector3& _v3Rot, const math::CVector3& _v3Color,
    float _fRadius, float _fHeight, int _iSubvH, int _iSubvV, render::ERenderMode _eRenderMode)
  {
    if (m_pCurrentScene)
    {
      m_pCurrentScene->DrawCapsule(_v3Pos, _v3Rot, _v3Color, _fRadius, _fHeight, _iSubvH, _iSubvV, _eRenderMode);
    }
  }
  // ------------------------------------
  void CSceneManager::DrawCube(const math::CVector3& _v3Pos, const math::CVector3& _v3Size, const math::CVector3& _v3Rot, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_pCurrentScene)
    {
      m_pCurrentScene->DrawCube(_v3Pos, _v3Size, _v3Rot, _v3Color, _eRenderMode);
    }
  }
  // ------------------------------------
  void CSceneManager::DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iSubvH, int _iSubvV, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_pCurrentScene)
    {
      m_pCurrentScene->DrawSphere(_v3Pos, _fRadius, _iSubvH, _iSubvV, _v3Color, _eRenderMode);
    }
  }
  // ------------------------------------
  void CSceneManager::DrawPlane(const math::CPlane& _oPlane, const math::CVector3& _v3Size, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_pCurrentScene)
    {
      m_pCurrentScene->DrawPlane(_oPlane, _v3Size, _v3Color, _eRenderMode);
    }
  }
  // ------------------------------------
  void CSceneManager::DrawLine(const math::CVector3& _v3Start, const math::CVector3& _v3Dest, const math::CVector3& _v3Color)
  {
    if (m_pCurrentScene)
    {
      m_pCurrentScene->DrawLine(_v3Start, _v3Dest, _v3Color);
    }
  }
  // ------------------------------------
  void CSceneManager::CreateScenes()
  {
    int iCurrentIdx = 0;
    std::for_each(m_lstScenes.begin(), m_lstScenes.end(), [&](scene::CScene*& _pScene)
    {
      _pScene = new scene::CScene(iCurrentIdx++);
      _pScene->SetEnabled(false);
    });
  }
  // ------------------------------------
  void CSceneManager::DestroyAllScenes()
  {
    std::for_each(m_lstScenes.begin(), m_lstScenes.end(), [](CScene*& _pScene)
    {
      global::ReleaseObject(_pScene);
    });
  }
}
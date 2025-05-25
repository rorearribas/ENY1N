#include "SceneManager.h"
#include <algorithm>
#include <cassert>

namespace scene
{
  CSceneManager::CSceneManager()
  {
    CreateScenes();
  }
  // ------------------------------------
  CSceneManager::~CSceneManager()
  {
    DestroyAllScenes();
  }
  // ------------------------------------
  void CSceneManager::CreateScenes()
  {
    int iCurrentIdx = 0;
    std::for_each(m_vctScenes.begin(), m_vctScenes.end(), [&](scene::CScene*& _pScene) 
    {
      _pScene = new scene::CScene(iCurrentIdx++);
      _pScene->SetSceneEnabled(false);
    });
  }
  // ------------------------------------
  void CSceneManager::DisableAllScenes() const
  {
    std::for_each(m_vctScenes.begin(), m_vctScenes.end(), [&](scene::CScene* _pScene)
    {
      _pScene->SetSceneEnabled(false);
    });
  }
  // ------------------------------------
  void CSceneManager::DrawLine(const math::CVector3& _v3Origin, const math::CVector3& _v3Dest, const math::CVector3& _v3Color)
  {
    if (m_pCurrentScene)
    {
      m_pCurrentScene->DrawLine(_v3Origin, _v3Dest, _v3Color);
    }
  }
  // ------------------------------------
  void CSceneManager::DrawCube(const math::CVector3& _v3Origin, float _fSize, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_pCurrentScene)
    {
      m_pCurrentScene->DrawCube(_v3Origin, _fSize, _v3Color, _eRenderMode);
    }
  }
  // ------------------------------------
  void CSceneManager::DrawSphere(const math::CVector3& _v3Pos, float _fRadius, int _iStacks, int _iSlices, const math::CVector3& _v3Color, render::ERenderMode _eRenderMode)
  {
    if (m_pCurrentScene)
    {
      m_pCurrentScene->DrawSphere(_v3Pos, _fRadius, _iStacks, _iSlices, _v3Color, _eRenderMode);
    }
  }
  // ------------------------------------
  void CSceneManager::SetSceneEnabled(uint32_t _uIndex, bool _bEnabled) const
  {
    if (_uIndex > (s_iMaxScenes - 1)) return;
    m_pCurrentScene = m_vctScenes[_uIndex];
    m_pCurrentScene->SetSceneEnabled(_bEnabled);
  }
  // ------------------------------------
  render::graphics::CPrimitive* const CSceneManager::CreatePrimitive
  (
    const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType, 
    render::ERenderMode _eRenderMode,
    uint32_t _uSceneIndex
  )
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_vctScenes.size() - 1)) return nullptr;
    scene::CScene* pScene = m_vctScenes[_uSceneIndex];
    return pScene->CreatePrimitive(_ePrimitiveType, _eRenderMode);
  }
  // ------------------------------------
  render::graphics::CModel* const CSceneManager::CreateModel(const char* _sModelPath, const char* _sBaseMltDir, uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_vctScenes.size() - 1)) return nullptr;
    scene::CScene* pScene = m_vctScenes[_uSceneIndex];
    return pScene->CreateModel(_sModelPath, _sBaseMltDir);
  }
  // ------------------------------------
  render::lights::CDirectionalLight* const CSceneManager::CreateDirectionalLight(uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_vctScenes.size() - 1)) return nullptr;
    scene::CScene* pScene = m_vctScenes[_uSceneIndex];
    return pScene->CreateDirectionalLight();
  }
  // ------------------------------------
  render::lights::CPointLight* const CSceneManager::CreatePointLight(uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_vctScenes.size() - 1)) return nullptr;
    scene::CScene* pScene = m_vctScenes[_uSceneIndex];
    return pScene->CreatePointLight();
  }
  // ------------------------------------
  render::lights::CSpotLight* const CSceneManager::CreateSpotLight(uint32_t _uSceneIndex)
  {
    if (static_cast<size_t>(_uSceneIndex) > (m_vctScenes.size() - 1)) return nullptr;
    scene::CScene* pScene = m_vctScenes[_uSceneIndex];
    return pScene->CreateSpotLight();
  }
  // ------------------------------------
  void CSceneManager::DestroyModel(render::graphics::CModel*& _pModel_, uint32_t _uIndex)
  {
    if (static_cast<size_t>(_uIndex) > (m_vctScenes.size() - 1)) return;
    scene::CScene* pScene = m_vctScenes.at(_uIndex);
    pScene->DestroyModel(_pModel_);
  }
  // ------------------------------------
  void CSceneManager::DestroyPrimitive(render::graphics::CPrimitive*& _pPrimitive_, uint32_t _uIndex)
  {
    if (static_cast<size_t>(_uIndex) > (m_vctScenes.size() - 1)) return;
    scene::CScene* pScene = m_vctScenes.at(_uIndex);
    pScene->DestroyPrimitive(_pPrimitive_);
  }
  // ------------------------------------
  void CSceneManager::DestroyLight(render::lights::CBaseLight*& _pLight_, uint32_t _uIndex)
  {
    if (static_cast<size_t>(_uIndex) > (m_vctScenes.size() - 1)) return;
    scene::CScene* pScene = m_vctScenes.at(_uIndex);
    pScene->DestroyLight(_pLight_);
  }
  // ------------------------------------
  void CSceneManager::DestroyAllScenes()
  {
    std::for_each(m_vctScenes.begin(), m_vctScenes.end(), [](CScene*& _pScene)
    {
      global::ReleaseObject(_pScene);
    });
  }
}
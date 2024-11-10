#include "SceneManager.h"
#include <algorithm>

namespace scene
{
  CSceneManager::~CSceneManager()
  {
    DestroyAllScenes();
  }
  // ------------------------------------
  void CSceneManager::InitScenes()
  {
    int iCurrentIdx = 0;
    std::for_each(m_vctScenes.begin(), m_vctScenes.end(), [&](scene::CScene*& _pScene) 
    {
      _pScene = new scene::CScene(iCurrentIdx++);
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
  void CSceneManager::SetSceneEnabled(const UINT32& _uIndex, bool _bEnabled) const
  {
    if (_uIndex > (s_iMaxScenes - 1)) return;
    scene::CScene* pScene = m_vctScenes[_uIndex];
    pScene->SetSceneEnabled(_bEnabled);
  }
  // ------------------------------------
  render::primitive::CPrimitive* CSceneManager::CreatePrimitive(const std::vector<render::primitive::CPrimitive::SPrimitiveInfo>& _vctVertexData, const UINT32& _uSceneIndex)
  {
    if ((size_t)(_uSceneIndex) > (m_vctScenes.size() - 1)) return nullptr;
    scene::CScene* pScene = m_vctScenes[_uSceneIndex];
    return pScene->CreatePrimitive(_vctVertexData);
  }
  // ------------------------------------
  render::primitive::CPrimitive* CSceneManager::CreatePrimitive(const render::primitive::CPrimitive::EPrimitiveType& _ePrimitiveType, const UINT32& _uSceneIndex)
  {
    if ((size_t)(_uSceneIndex) > (m_vctScenes.size() - 1)) return nullptr;
    scene::CScene* pScene = m_vctScenes[_uSceneIndex];
    return pScene->CreatePrimitive(_ePrimitiveType);
  }
  // ------------------------------------
  void CSceneManager::DestroyAllScenes()
  {
    std::for_each(m_vctScenes.begin(), m_vctScenes.end(), [](CScene* _pScene)
    {
      delete _pScene;
      _pScene = nullptr;
    });
  }
}
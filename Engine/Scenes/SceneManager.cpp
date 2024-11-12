#include "SceneManager.h"
#include <algorithm>
#include <cassert>

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
  void CSceneManager::DestroyPrimitive(const render::primitive::CPrimitive* _pPrimitive)
  {
    assert(_pPrimitive);
    for (scene::CScene* pScene : m_vctScenes)
    {
      const scene::CScene::TPrimitiveList& vctPrimitives = pScene->GetPrimitives();
      auto it = std::find(vctPrimitives.begin(), vctPrimitives.end(), _pPrimitive);
      if (it != vctPrimitives.end())
      {
        pScene->DestroyPrimitive(_pPrimitive);
        break;
      }
    }
  }
  // ------------------------------------
  void CSceneManager::DestroyAllPrimimitives(const UINT32& _uSceneIndex)
  {
    scene::CScene* pScene = m_vctScenes.at(_uSceneIndex);
    assert(pScene);
    pScene->DestroyAllPrimitives();
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
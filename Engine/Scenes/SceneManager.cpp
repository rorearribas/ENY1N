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
  void CSceneManager::Draw()
  {
    m_pCurrentScene->DrawScene();
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
    m_pCurrentScene = m_vctScenes[_uIndex];
    m_pCurrentScene->SetSceneEnabled(_bEnabled);
  }
  // ------------------------------------
  render::graphics::CPrimitive* CSceneManager::CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData, const UINT32& _uSceneIndex)
  {
    if ((size_t)(_uSceneIndex) > (m_vctScenes.size() - 1)) return nullptr;
    scene::CScene* pScene = m_vctScenes[_uSceneIndex];
    return pScene->CreatePrimitive(_vctVertexData);
  } 
  // ------------------------------------
  render::graphics::CPrimitive* CSceneManager::CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType, const UINT32& _uSceneIndex)
  {
    if ((size_t)(_uSceneIndex) > (m_vctScenes.size() - 1)) return nullptr;
    scene::CScene* pScene = m_vctScenes[_uSceneIndex];
    return pScene->CreatePrimitive(_ePrimitiveType);
  }
  // ------------------------------------
  render::graphics::CModel* CSceneManager::CreateModel(const char* _sModelPath, const char* _sBaseMltDir, const UINT32& _uSceneIndex)
  {
    if ((size_t)(_uSceneIndex) > (m_vctScenes.size() - 1)) return nullptr;
    scene::CScene* pScene = m_vctScenes[_uSceneIndex];
    return pScene->CreateModel(_sModelPath, _sBaseMltDir);
  }
  // ------------------------------------
  void CSceneManager::DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_)
  {
    assert(pPrimitive_);
    for (scene::CScene* pScene : m_vctScenes)
    {
      const scene::CScene::TPrimitiveList& vctPrimitives = pScene->GetPrimitives();
      auto it = std::find(vctPrimitives.begin(), vctPrimitives.end(), pPrimitive_);
      if (it != vctPrimitives.end())
      {
        pScene->DestroyPrimitive(pPrimitive_);
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
  void CSceneManager::DestroyModel(render::graphics::CModel*& pModel_)
  {
    assert(pModel_);
    for (scene::CScene* pScene : m_vctScenes)
    {
      const scene::CScene::TModelList& vctModels = pScene->GetModels();
      auto it = std::find(vctModels.begin(), vctModels.end(), pModel_);
      if (it != vctModels.end())
      {
        pScene->DestroyModel(pModel_);
        break;
      }
    }
  }
  // ------------------------------------
  void CSceneManager::DestroyAllModels(const UINT32& _uSceneIndex)
  {
    scene::CScene* pScene = m_vctScenes.at(_uSceneIndex);
    assert(pScene);
    pScene->DestroyAllModels();
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
#include "Scene.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Base/Engine.h"
#include "Engine/Render/Render.h"
#include <algorithm>
#include <random>
#include <cassert>

namespace scene
{
  CScene::~CScene()
  {
    DestroyAllPrimitives();
    DestroyAllModels();
  }
  // ------------------------------------
  void CScene::Draw()
  {
    DrawModels();
    DrawPrimitives();
  }
  // ------------------------------------
  void CScene::DrawPrimitives()
  {
    for (int iIndex = 0; iIndex < m_iRegisteredPrimitives; iIndex++)
    {
      render::graphics::CPrimitive* pPrimitiveItem = m_vctPrimitiveItems[iIndex];
      pPrimitiveItem->DrawPrimitive();
    }
  }
  // ------------------------------------
  void CScene::DrawModels()
  {
    for (int iIndex = 0; iIndex < m_iRegisteredModels; iIndex++)
    {
      render::graphics::CModel* pModel = m_vctModels[iIndex];
      pModel->DrawModel();
    }
  }
  // ------------------------------------
  void CScene::DestroyAllPrimitives()
  {
    std::for_each(m_vctPrimitiveItems.begin(), m_vctPrimitiveItems.end(), [](render::graphics::CPrimitive*& _pPrimitive) 
    {
      if (_pPrimitive) 
      { 
        delete _pPrimitive; 
        _pPrimitive = nullptr; 
      }
    });
    m_iRegisteredPrimitives = 0;
  }
  // ------------------------------------
  void CScene::DestroyAllModels()
  {
    std::for_each(m_vctModels.begin(), m_vctModels.end(), [](render::graphics::CModel*& _pModel)
      {
        if (_pModel)
        {
          delete _pModel;
          _pModel = nullptr;
        }
      });
    m_iRegisteredModels = 0;
  }
  // ------------------------------------
  render::graphics::CPrimitive* CScene::CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData)
  {
    if (m_iRegisteredPrimitives >= s_iMaxPrimitives) return nullptr;
    render::graphics::CPrimitive*& pPrimitiveItem = m_vctPrimitiveItems[m_iRegisteredPrimitives++];
    pPrimitiveItem = new render::graphics::CPrimitive(_vctVertexData);
    return pPrimitiveItem;
  }
  // ------------------------------------
  render::graphics::CPrimitive* CScene::CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType)
  {
    if (m_iRegisteredPrimitives >= s_iMaxPrimitives) return nullptr;
    render::graphics::CPrimitive*& pPrimitiveItem = m_vctPrimitiveItems[m_iRegisteredPrimitives++];
    pPrimitiveItem = new render::graphics::CPrimitive(_ePrimitiveType);
    return pPrimitiveItem;
  }
  // ------------------------------------
  render::graphics::CModel* CScene::CreateModel(const char* _sModelPath, const char* _sBaseMltDir)
  {
    if (m_iRegisteredPrimitives >= s_iMaxPrimitives) return nullptr;
    render::graphics::CModel*& pModel = m_vctModels[m_iRegisteredModels++];
    pModel = new render::graphics::CModel(_sModelPath, _sBaseMltDir);
    return pModel;
  }
  // ------------------------------------
  void CScene::DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_)
  {
    assert(pPrimitive_);
    auto it = std::find(m_vctPrimitiveItems.begin(), m_vctPrimitiveItems.end(), pPrimitive_);
    if (it != m_vctPrimitiveItems.end())
    {
      delete* it;
      *it = nullptr;
      m_iRegisteredPrimitives--;

      auto oReorderFunc = std::remove_if(m_vctPrimitiveItems.begin(), m_vctPrimitiveItems.end(), 
      [] (render::graphics::CPrimitive* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_vctPrimitiveItems.end(), nullptr); // Set nullptr
    }
    pPrimitive_ = nullptr;
  }
  // ------------------------------------
  void CScene::DestroyModel(render::graphics::CModel*& pModel_)
  {
    assert(pModel_);
    auto it = std::find(m_vctModels.begin(), m_vctModels.end(), pModel_);
    if (it != m_vctModels.end())
    {
      delete* it;
      *it = nullptr;
      m_iRegisteredModels--;

      auto oReorderFunc = std::remove_if(m_vctModels.begin(), m_vctModels.end(),
        [](render::graphics::CModel* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_vctModels.end(), nullptr); // Set nullptr
    }
    pModel_ = nullptr;
  }
}
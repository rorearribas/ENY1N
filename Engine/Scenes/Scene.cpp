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
  }
  // ------------------------------------
  void CScene::DrawScene()
  {
    DrawPrimitives();
  }
  // ------------------------------------
  void CScene::DrawPrimitives()
  {
    for (int iIndex = 0; iIndex < m_iRegisteredPrimitives; iIndex++)
    {
      render::primitive::CPrimitive* pPrimitiveItem = m_vctPrimitiveItems[iIndex];
      pPrimitiveItem->DrawPrimitive();
    }
  }
  // ------------------------------------
  void CScene::DestroyAllPrimitives()
  {
    std::for_each(m_vctPrimitiveItems.begin(), m_vctPrimitiveItems.end(), [](render::primitive::CPrimitive*& _pPrimitive) 
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
  render::primitive::CPrimitive* CScene::CreatePrimitive(const std::vector<render::primitive::CPrimitive::SPrimitiveInfo>& _vctVertexData)
  {
    if (m_iRegisteredPrimitives >= s_iMaxPrimitives) return nullptr;
    render::primitive::CPrimitive*& pPrimitiveItem = m_vctPrimitiveItems[m_iRegisteredPrimitives++];
    pPrimitiveItem = new render::primitive::CPrimitive(_vctVertexData);
    return pPrimitiveItem;
  }
  // ------------------------------------
  render::primitive::CPrimitive* CScene::CreatePrimitive(const render::primitive::CPrimitive::EPrimitiveType& _ePrimitiveType)
  {
    if (m_iRegisteredPrimitives >= s_iMaxPrimitives) return nullptr;
    render::primitive::CPrimitive*& pPrimitiveItem = m_vctPrimitiveItems[m_iRegisteredPrimitives++];
    pPrimitiveItem = new render::primitive::CPrimitive(_ePrimitiveType);
    return pPrimitiveItem;
  }
  // ------------------------------------
  void CScene::DestroyPrimitive(const render::primitive::CPrimitive* _pPrimitive)
  {
    assert(_pPrimitive);
    auto it = std::find(m_vctPrimitiveItems.begin(), m_vctPrimitiveItems.end(), _pPrimitive);
    if (it != m_vctPrimitiveItems.end())
    {
      delete* it;
      *it = nullptr;
      m_iRegisteredPrimitives--;

      auto oReorderFunc = std::remove_if(m_vctPrimitiveItems.begin(), m_vctPrimitiveItems.end(), 
      [] (render::primitive::CPrimitive* ptr) { return ptr == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_vctPrimitiveItems.end(), nullptr); // Set nullptr
    }
  }
}
#include "Scene.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Base/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/Lights/PointLight.h"
#include "Engine/Render/Lights/DirectionalLight.h"
#include "Engine/Render/Lights/Light.h"
#include <algorithm>
#include <random>
#include <cassert>

namespace scene
{
  CScene::~CScene()
  {
    DestroyAllPrimitives();
    DestroyAllModels();
    DestroyAllLights();
  }
  // ------------------------------------
  void CScene::DrawPrimitives()
  {
    for (uint32_t uIndex = 0; uIndex < m_uRegisteredPrimitives; uIndex++)
    {
      render::graphics::CPrimitive* pPrimitiveItem = m_vctPrimitiveItems[uIndex];
      pPrimitiveItem->DrawPrimitive();
    }
  }
  // ------------------------------------
  void CScene::DrawModels()
  {
    for (uint32_t uIndex = 0; uIndex < m_uRegisteredModels; uIndex++)
    {
      render::graphics::CModel* pModel = m_vctModels[uIndex];
      pModel->DrawModel();
    }
  }
  // ------------------------------------
  void CScene::UpdateLights()
  {
    for (uint32_t uIndex = 0; uIndex < m_uRegisteredLights; uIndex++)
    {
      render::lights::CLight* pLight = m_vctLights[uIndex];
      pLight->UpdateLight();
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
    m_uRegisteredPrimitives = 0;
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
    m_uRegisteredModels = 0;
  }
  // ------------------------------------
  void CScene::DestroyAllLights()
  {
    std::for_each(m_vctLights.begin(), m_vctLights.end(), [](render::lights::CLight*& _pLight)
    {
      if (_pLight)
      {
        delete _pLight;
        _pLight = nullptr;
      }
    });
    m_uRegisteredLights = 0;
  }
  // ------------------------------------
  render::graphics::CPrimitive* CScene::CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData)
  {
    if (m_uRegisteredPrimitives >= s_iMaxPrimitives) return nullptr;
    render::graphics::CPrimitive*& pPrimitiveItem = m_vctPrimitiveItems[m_uRegisteredPrimitives++];
    pPrimitiveItem = new render::graphics::CPrimitive(_vctVertexData);
    return pPrimitiveItem;
  }
  // ------------------------------------
  render::graphics::CPrimitive* CScene::CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType)
  {
    if (m_uRegisteredPrimitives >= s_iMaxPrimitives) return nullptr;
    render::graphics::CPrimitive*& pPrimitiveItem = m_vctPrimitiveItems[m_uRegisteredPrimitives++];
    pPrimitiveItem = new render::graphics::CPrimitive(_ePrimitiveType);
    return pPrimitiveItem;
  }
  // ------------------------------------
  render::graphics::CModel* CScene::CreateModel(const char* _sModelPath, const char* _sBaseMltDir)
  {
    if (m_uRegisteredPrimitives >= s_iMaxPrimitives) return nullptr;
    render::graphics::CModel*& pModel = m_vctModels[m_uRegisteredModels++];
    pModel = new render::graphics::CModel(_sModelPath, _sBaseMltDir);
    return pModel;
  }
  // ------------------------------------
  render::lights::CDirectionalLight* CScene::CreateDirectionalLight()
  {
    if (m_uRegisteredLights >= s_iMaxLights) return nullptr;
    render::lights::CLight*& pDirectional = m_vctLights[m_uRegisteredLights++];
    pDirectional = new render::lights::CDirectionalLight();
    return static_cast<render::lights::CDirectionalLight*>(pDirectional);
  }
  // ------------------------------------
  render::lights::CPointLight* CScene::CreatePointLight()
  {
    if (m_uRegisteredLights >= s_iMaxLights) return nullptr;
    render::lights::CLight*& pPointLight = m_vctLights[m_uRegisteredLights++];
    pPointLight = new render::lights::CPointLight();
    return static_cast<render::lights::CPointLight*>(pPointLight);
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
      m_uRegisteredPrimitives--;

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
      m_uRegisteredModels--;

      auto oReorderFunc = std::remove_if(m_vctModels.begin(), m_vctModels.end(),
        [](render::graphics::CModel* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_vctModels.end(), nullptr); // Set nullptr
    }
    pModel_ = nullptr;
  }
  // ------------------------------------
  void CScene::DestroyLight(render::lights::CLight*& pLight_)
  {
    assert(pLight_);
    auto it = std::find(m_vctLights.begin(), m_vctLights.end(), pLight_);
    if (it != m_vctLights.end())
    {
      delete* it;
      *it = nullptr;
      m_uRegisteredLights--;

      auto oReorderFunc = std::remove_if(m_vctLights.begin(), m_vctLights.end(),
      [](render::lights::CLight* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_vctLights.end(), nullptr); // Set nullptr
    }
    pLight_ = nullptr;
  }

}
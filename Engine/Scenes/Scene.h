#pragma once
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Libs/Macros/GlobalMacros.h"
#include <cassert>
#include <array>
#include <algorithm>

namespace render { class CRender; }
namespace render { namespace lights { class CSpotLight; } }
namespace render { namespace lights { class CLight; } }
namespace render { namespace lights { class CPointLight; } }
namespace render { namespace lights { class CDirectionalLight; } }

namespace internal
{
  template<typename T, size_t MAX_ITEMS>
  class CSceneItemList
  {
  public:
    template<typename ...Args>
    T* CreateItem(Args&&... args)
    {
      T*& pItem = m_vctItemList[m_uRegisteredItems++];
      pItem = new T(std::forward<Args>(args)...);
      return pItem;
    }
    bool RemoveItem(T* _pItem_);
    void ClearAll();

    const uint32_t& CurrentSize() const { return m_uRegisteredItems; }
    uint32_t MaxSize() { return static_cast<uint32_t>(MAX_ITEMS);}

    std::array<T*, MAX_ITEMS>& operator()() { return m_vctItemList; }
    T* operator[](size_t Index) { return m_vctItemList[Index]; }

  private:
    std::array<T*, MAX_ITEMS> m_vctItemList = std::array<T*, MAX_ITEMS>();
    uint32_t m_uRegisteredItems = 0;
  };

  template<typename T, size_t MAX_ITEMS>
  void internal::CSceneItemList<T, MAX_ITEMS>::ClearAll()
  {
    std::for_each(m_vctItemList.begin(), m_vctItemList.end(), [](T* _pItem)
    {
      if (_pItem)
      {
        delete _pItem;
        _pItem = nullptr;
      }
    });
    m_uRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  bool internal::CSceneItemList<T, MAX_ITEMS>::RemoveItem(T* _pItem_)
  {
    auto it = std::find(m_vctItemList.begin(), m_vctItemList.end(), _pItem_);
    if (it != m_vctItemList.end())
    {
      delete* it;
      *it = nullptr;
      m_uRegisteredItems--;

      auto oReorderFunc = std::remove_if(m_vctItemList.begin(), m_vctItemList.end(),
      [](T* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_vctItemList.end(), nullptr); // Set nullptr

      return true;
    }
    return false;
  }
}

namespace scene
{
  class CScene
  {
  public:
    static int constexpr s_iMaxSpotLights = 100;
    static int constexpr s_iMaxLights = 200;
    static int constexpr s_iMaxModels = 2500;
    static int constexpr s_iMaxPrimitives = 500;
    static int constexpr s_iMaxPointLights = 100;

    // Lights
    typedef internal::CSceneItemList<render::lights::CPointLight, s_iMaxPointLights> TPointLightsList;
    typedef internal::CSceneItemList<render::lights::CSpotLight, s_iMaxSpotLights> TSpotLightsList;

    // Graphics
    typedef internal::CSceneItemList<render::graphics::CPrimitive, s_iMaxPrimitives> TPrimitiveList;
    typedef internal::CSceneItemList<render::graphics::CModel, s_iMaxPrimitives> TModelList;

  public:
    CScene(const UINT32& _uIndex);
    ~CScene();

    const uint32_t& GetSceneIndex() const { return m_uSceneIdx; }
    void SetSceneEnabled(bool _bEnabled) { m_bEnabled = _bEnabled; }
    const bool& IsEnabled() const { return m_bEnabled; }

    // Graphics
    render::graphics::CPrimitive* CreatePrimitive(const std::vector<render::graphics::CPrimitive::SPrimitiveInfo>& _vctVertexData);
    render::graphics::CPrimitive* CreatePrimitive(const render::graphics::CPrimitive::EPrimitiveType& _ePrimitiveType);
    render::graphics::CModel* CreateModel(const char* _sModelPath, const char* _sBaseMltDir);

    // Lights
    render::lights::CDirectionalLight* CreateDirectionalLight();
    render::lights::CPointLight* CreatePointLight();
    render::lights::CSpotLight* CreateSpotLight();

    void DestroyPrimitive(render::graphics::CPrimitive*& pPrimitive_);
    void DestroyModel(render::graphics::CModel*& pModel_);
    void DestroyLight(render::lights::CLight*& pLight_);

  private:
    friend class render::CRender;

    bool DestroyPointLight(render::lights::CPointLight* pLight_);
    bool DestroySpotLight(render::lights::CSpotLight* pLight_);

    void DestroyAllPrimitives();
    void DestroyAllModels();

    void DestroyAllLights();

    void DrawPrimitives();
    void DrawModels();
    void UpdateLights();

  private:
    bool m_bEnabled = false;
    UINT32 m_uSceneIdx = 0;

    // Graphics
    TModelList m_vctModels = {};
    TPrimitiveList m_vctPrimitiveItems = {};

    // Lights
    render::lights::CDirectionalLight* m_pDirectionalLight = nullptr;
    TPointLightsList m_vctPointLights = {};
    TSpotLightsList m_vctSpotLights = {};

    // Global lightning buffer
    CConstantBuffer<SGlobalLightningData<s_iMaxPointLights, s_iMaxSpotLights>> m_oLightningBuffer;
  };
}
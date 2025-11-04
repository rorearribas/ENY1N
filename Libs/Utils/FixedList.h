#pragma once
#include <algorithm>
#include <array>
#include <iostream>

namespace utils
{
  template<typename T, size_t MAX_ITEMS>
  class CFixedList {
  public:
    template<typename Inheritance = T, typename ...Args>
    T* RegisterItem(Args&&... args)
    {
      if (m_uRegisteredItems >= MAX_ITEMS)
      {
        return nullptr;
      }

      Inheritance* pItem = new Inheritance(std::forward<Args>(args)...);
      m_lstItemList[m_uRegisteredItems] = pItem;
      return m_lstItemList[m_uRegisteredItems++];
    }

    bool RemoveItem(T* _pItem_);
    void ClearAll();

    const uint32_t& CurrentSize() const { return m_uRegisteredItems; }
    size_t GetMaxSize() const { return static_cast<size_t>(MAX_ITEMS); }

    T* operator[](int _iIndex) { return m_lstItemList[_iIndex]; }

  private:
    std::array<T*, MAX_ITEMS> m_lstItemList = std::array<T*, MAX_ITEMS>();
    uint32_t m_uRegisteredItems = 0;
  };

  template<typename T, size_t MAX_ITEMS>
  void CFixedList<T, MAX_ITEMS>::ClearAll()
  {
    for (uint32_t uIndex = 0; uIndex < m_uRegisteredItems; uIndex++)
    {
      global::ReleaseObject(m_lstItemList[uIndex]);
    }
    m_uRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  bool CFixedList<T, MAX_ITEMS>::RemoveItem(T* _pItem_)
  {
    auto it = std::find(m_lstItemList.begin(), m_lstItemList.end(), _pItem_);
    if (it != m_lstItemList.end() && *it)
    {
      global::ReleaseObject(*it);
      m_uRegisteredItems--;

      auto oReorderFunc = std::remove_if(m_lstItemList.begin(), m_lstItemList.end(),
        [](T* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_lstItemList.end(), nullptr); // Set nullptr

      return true;
    }
    return false;
  }
}

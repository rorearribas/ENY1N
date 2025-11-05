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
      m_lstFixed[m_uRegisteredItems] = pItem;
      return m_lstFixed[m_uRegisteredItems++];
    }

    T* operator[](int _iIndex) { return m_lstFixed[_iIndex]; }

    bool RemoveItem(T* _pItem_);
    void ClearAll();

    T* begin() { return m_lstFixed.begin(); }
    const T* begin() const { return m_lstFixed.begin(); }

    const T* end() const { return m_lstFixed.begin() + m_uRegisteredItems; }
    T* end() { return m_lstFixed.begin() + m_uRegisteredItems; }

    const uint32_t& CurrentSize() const { return m_uRegisteredItems; }
    size_t GetMaxSize() const { return static_cast<size_t>(MAX_ITEMS); }

  private:
    std::array<T*, MAX_ITEMS> m_lstFixed = std::array<T*, MAX_ITEMS>();
    uint32_t m_uRegisteredItems = 0;
  };

  template<typename T, size_t MAX_ITEMS>
  void CFixedList<T, MAX_ITEMS>::ClearAll()
  {
    for (uint32_t uIndex = 0; uIndex < m_uRegisteredItems; uIndex++)
    {
      global::ReleaseObject(m_lstFixed[uIndex]);
    }
    m_uRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  bool CFixedList<T, MAX_ITEMS>::RemoveItem(T* _pItem_)
  {
    auto it = std::find(m_lstFixed.begin(), m_lstFixed.end(), _pItem_);
    if (it != m_lstFixed.end() && *it)
    {
      global::ReleaseObject(*it);
      m_uRegisteredItems--;

      auto oReorderFunc = std::remove_if(m_lstFixed.begin(), m_lstFixed.end(),
        [](T* _pPtr) { return _pPtr == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_lstFixed.end(), nullptr); // Set nullptr

      return true;
    }
    return false;
  }
}

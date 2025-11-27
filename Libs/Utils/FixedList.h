#pragma once
#include <algorithm>
#include <array>
#include <iostream>

namespace utils
{
  template<typename T, size_t MAX_ITEMS>
  class CFixedList {
  public:
    template<typename _Type = T, typename ...Args>
    inline T* Create(Args&&... args)
    {
      if (m_tRegisteredItems >= MAX_ITEMS)
      {
        return nullptr;
      }

      _Type* pItem = new _Type(std::forward<Args>(args)...);
      m_lstFixed[m_tRegisteredItems] = pItem;
      return m_lstFixed[m_tRegisteredItems++];
    }

    inline T* operator[](size_t _tIndex) { return m_lstFixed[_tIndex]; }
    inline const T* operator[](size_t _tIndex) const { return m_lstFixed[_tIndex]; }

    bool Remove(T* _pItem_);
    void Clear();

    inline const size_t& GetCurrentSize() const { return m_tRegisteredItems; }
    inline size_t GetMaxSize() const { return MAX_ITEMS; }

  private:
    std::array<T*, MAX_ITEMS> m_lstFixed = std::array<T*, MAX_ITEMS>();
    size_t m_tRegisteredItems = 0;
  };

  template<typename T, size_t MAX_ITEMS>
  void CFixedList<T, MAX_ITEMS>::Clear()
  {
    for (size_t tIndex = 0; tIndex < m_tRegisteredItems; ++tIndex)
    {
      global::ReleaseObject(m_lstFixed[tIndex]);
    }
    m_tRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  bool CFixedList<T, MAX_ITEMS>::Remove(T* _pItem_)
  {
    auto it = std::find(m_lstFixed.begin(), m_lstFixed.end(), _pItem_);
    if (it != m_lstFixed.end())
    {
      *it = m_lstFixed[m_tRegisteredItems - 1];
      m_lstFixed[m_tRegisteredItems - 1] = nullptr;
      m_tRegisteredItems--;
      return true;
    }
    return false;
  }
}

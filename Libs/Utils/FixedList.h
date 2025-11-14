#pragma once
#include <algorithm>
#include <array>
#include <iostream>

namespace utils
{
  template<typename T, uint32_t MAX_ITEMS>
  class CFixedList {
  public:
    template<typename Inheritance = T, typename ...Args>
    inline T* Create(Args&&... args)
    {
      if (m_uRegisteredItems >= MAX_ITEMS)
      {
        return nullptr;
      }

      Inheritance* pItem = new Inheritance(std::forward<Args>(args)...);
      m_lstFixed[m_uRegisteredItems] = pItem;
      return m_lstFixed[m_uRegisteredItems++];
    }

    inline T* operator[](uint32_t _uIndex) { return m_lstFixed[_uIndex]; }
    inline const T* operator[](uint32_t _uIndex) const { return m_lstFixed[_uIndex]; }

    bool Remove(T* _pItem_);
    void Clear();

    inline const uint32_t& GetCurrentSize() const { return m_uRegisteredItems; }
    inline uint32_t GetMaxSize() const { return MAX_ITEMS; }

  private:
    std::array<T*, MAX_ITEMS> m_lstFixed = std::array<T*, MAX_ITEMS>();
    uint32_t m_uRegisteredItems = 0;
  };

  template<typename T, uint32_t MAX_ITEMS>
  void CFixedList<T, MAX_ITEMS>::Clear()
  {
    for (uint32_t uIndex = 0; uIndex < m_uRegisteredItems; ++uIndex)
    {
      global::ReleaseObject(m_lstFixed[uIndex]);
    }
    m_uRegisteredItems = 0;
  }

  template<typename T, uint32_t MAX_ITEMS>
  bool CFixedList<T, MAX_ITEMS>::Remove(T* _pItem_)
  {
    auto it = std::find(m_lstFixed.begin(), m_lstFixed.end(), _pItem_);
    if (it != m_lstFixed.end())
    {
      *it = m_lstFixed[m_uRegisteredItems - 1];
      m_lstFixed[m_uRegisteredItems - 1] = nullptr;
      m_uRegisteredItems--;
      return true;
    }
    return false;
  }
}

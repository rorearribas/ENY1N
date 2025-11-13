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

    // Set only ptr
    inline void Insert(T* _pMem)
    {
      if (m_uRegisteredItems >= MAX_ITEMS)
      {
        return;
      }
      m_lstFixed[m_uRegisteredItems++] = _pMem;
    }

    inline T* operator[](uint32_t _uIndex) { return m_lstFixed[_uIndex]; }
    inline const T* operator[](uint32_t _uIndex) const { return m_lstFixed[_uIndex]; }

    bool Remove(T* _pItem_, bool _bReleaseMem = true);
    void Clear(bool _bReleaseMem = true);

    inline const uint32_t& GetCurrentSize() const { return m_uRegisteredItems; }
    inline uint32_t GetMaxSize() const { return MAX_ITEMS; }

  private:
    std::array<T*, MAX_ITEMS> m_lstFixed = std::array<T*, MAX_ITEMS>();
    uint32_t m_uRegisteredItems = 0;
  };

  template<typename T, uint32_t MAX_ITEMS>
  void CFixedList<T, MAX_ITEMS>::Clear(bool _bReleaseMem)
  {
    auto it = m_lstFixed.begin();
    for (; it != m_lstFixed.end(); ++it)
    {
      if (_bReleaseMem)
      {
        global::ReleaseObject(*it);
      }
      else
      {
        *it = nullptr;
      }
    }
    m_uRegisteredItems = 0;
  }

  template<typename T, uint32_t MAX_ITEMS>
  bool CFixedList<T, MAX_ITEMS>::Remove(T* _pItem_, bool _bReleaseMem)
  {
    auto it = std::find(m_lstFixed.begin(), m_lstFixed.end(), _pItem_);
    if (it != m_lstFixed.end() && *it)
    {
      if (_bReleaseMem)
      {
        global::ReleaseObject(*it);
      }
      else
      {
        *it = nullptr;
      }
      m_uRegisteredItems--;

      auto oFunc = std::remove_if(m_lstFixed.begin(), m_lstFixed.end(), [](T* _pPtr) { return _pPtr == nullptr; });
      std::fill(oFunc, m_lstFixed.end(), nullptr); // Set nullptr
      return true;
    }
    return false;
  }
}

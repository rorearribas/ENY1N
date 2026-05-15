#pragma once
#include <algorithm>
#include <array>
#include <iostream>

namespace utils
{
  template<typename T, size_t MAX_ITEMS>
  class CFixedList {
  public:
    CFixedList() : m_tRegisteredItems(0) { Init(); }
    ~CFixedList() { Clear(); }

    template<typename _Type = T, typename ...Args>
    inline T* Create(Args&&... args)
    {
      if (m_tRegisteredItems >= MAX_ITEMS)
      {
        return nullptr;
      }

      for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
      {
        T* pCurrentItem = m_lstFixed[tIndex];
        if (!pCurrentItem)
        {
          _Type* pItem = new _Type(std::forward<Args>(args)...);
          m_lstFixed[tIndex] = pItem;
          m_lstAssignedItems[m_tRegisteredItems++] = tIndex;
          return pItem;
        }
      }
      return nullptr;
    }

    inline T* operator[](size_t _tIndex)
    {
      bool bAssignedBlock = (_tIndex < MAX_ITEMS&& m_lstAssignedItems[_tIndex] >= 0);
      return bAssignedBlock ? m_lstFixed[m_lstAssignedItems[_tIndex]] : nullptr;
    }
    inline const T* operator[](size_t _tIndex) const
    {
      bool bAssignedBlock = (_tIndex < MAX_ITEMS&& m_lstAssignedItems[_tIndex] >= 0);
      return bAssignedBlock ? m_lstFixed[m_lstAssignedItems[_tIndex]] : nullptr;
    }

    bool Remove(T* _pItem_);
    void Clear();

    inline const bool IsEmpty() const { return m_tRegisteredItems == 0; }
    inline const size_t GetCurrentSize() const { return m_tRegisteredItems; }
    inline size_t GetMaxSize() const { return MAX_ITEMS; }

  private:
    void Init()
    {
      m_lstAssignedItems.fill(-1);
    }

  private:
    std::array<T*, MAX_ITEMS> m_lstFixed = std::array<T*, MAX_ITEMS>();
    std::array<__int64, MAX_ITEMS> m_lstAssignedItems = std::array<__int64, MAX_ITEMS>();
    size_t m_tRegisteredItems = 0;
  };

  template<typename T, size_t MAX_ITEMS>
  bool CFixedList<T, MAX_ITEMS>::Remove(T* _pItem_)
  {
    for (size_t tIndex = 0; tIndex < m_tRegisteredItems; ++tIndex)
    {
      size_t tValidIdx = m_lstAssignedItems[tIndex];
      T*& pValue = m_lstFixed[tValidIdx];
      if (pValue == _pItem_)
      {
        delete pValue;
        pValue = nullptr;
        m_lstAssignedItems[tIndex] = -1;

        std::swap(m_lstAssignedItems[tIndex], m_lstAssignedItems[m_tRegisteredItems - 1]);
        --m_tRegisteredItems;
        return true;
      }
    }
    return false;
  }

  template<typename T, size_t MAX_ITEMS>
  void CFixedList<T, MAX_ITEMS>::Clear()
  {
    for (size_t tIndex = 0; tIndex < m_tRegisteredItems; ++tIndex)
    {
      size_t tValidIdx = m_lstAssignedItems[tIndex];
      T*& pValue = m_lstFixed[tValidIdx];
      delete pValue;
      pValue = nullptr;
    }
    m_tRegisteredItems = 0;
  }
}

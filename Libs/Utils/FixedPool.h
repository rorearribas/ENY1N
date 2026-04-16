#pragma once
#include <algorithm>
#include <bitset>
#include <cstddef>
#include <cassert>
#include <new>
#include <iostream>
#include <type_traits>
#include <array>

namespace utils
{
  template<typename T, size_t MAX_ITEMS>
  class CFixedPool
  {
  public:
    CFixedPool() : m_tRegisteredItems(0) { Init(); }
    ~CFixedPool() { Clear(); }

    template<typename ItemType = T, typename ...Args>
    inline T* Create(Args&&... args)
    {
      if (m_tRegisteredItems >= MAX_ITEMS)
      {
#ifdef _DEBUG
        assert(false);
#endif
        return nullptr;
      }

      for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
      {
        if (m_lstAssignedItems[tIndex] == -1)
        {
          void* pMem = static_cast<void*>(&m_lstPool[tIndex]);
          ItemType* pItem = new (pMem) ItemType(std::forward<Args>(args)...);
          m_lstAssignedItems[m_tRegisteredItems++] = tIndex;
          return pItem;
        }
      }

      return nullptr;
    }

    inline T* operator[](size_t _tIndex)
    {
      bool bAssignedBlock = (_tIndex < MAX_ITEMS && m_lstAssignedItems[_tIndex] >= 0);
      return bAssignedBlock ? reinterpret_cast<T*>(&m_lstPool[m_lstAssignedItems[_tIndex]]) : nullptr;
    }

    inline const T* operator[](size_t _tIndex) const
    {
      bool bAssignedBlock = (_tIndex < MAX_ITEMS && (m_lstAssignedItems[_tIndex] >= 0));
      return bAssignedBlock ? reinterpret_cast<const T*>(&m_lstPool[m_lstAssignedItems[_tIndex]]) : nullptr;
    }

    bool Remove(T*& _pItem_);
    void Clear();

    inline T* begin() { return reinterpret_cast<T*>(m_lstPool); }
    inline const T* begin() const { return reinterpret_cast<const T*>(m_lstPool); }

    inline T* end() { return reinterpret_cast<T*>(m_lstPool) + m_tRegisteredItems; }
    inline const T* end() const { return reinterpret_cast<const T*>(m_lstPool) + m_tRegisteredItems; }

    inline T* last() { return reinterpret_cast<T*>(&m_lstPool[m_tRegisteredItems > 0 ? (m_tRegisteredItems - 1) : 0]); }
    inline const T* last() const { return reinterpret_cast<const T*>(&m_lstPool[m_tRegisteredItems > 0 ? (m_tRegisteredItems - 1) : 0]); }

    inline const bool IsEmpty() const { return m_tRegisteredItems == 0; }
    inline const size_t GetSize() const { return m_tRegisteredItems; }
    inline const size_t GetMaxSize() const { return MAX_ITEMS; }

  private:
    void Init()
    {
      for (size_t i = 0; i < MAX_ITEMS; i++)
      {
        m_lstAssignedItems[i] = -1;
      }
    }

  private:
    std::aligned_storage_t<sizeof(T), alignof(std::max_align_t)> m_lstPool[MAX_ITEMS];
    std::array<__int64, MAX_ITEMS> m_lstAssignedItems = std::array<__int64, MAX_ITEMS>();
    size_t m_tRegisteredItems = 0;
  };

  template<typename T, size_t MAX_ITEMS>
  bool CFixedPool<T, MAX_ITEMS>::Remove(T*& _pItem_)
  {
    for (size_t tIndex = 0; tIndex < m_tRegisteredItems; ++tIndex)
    {
      size_t tValidIdx = m_lstAssignedItems[tIndex];
      T* pItem = reinterpret_cast<T*>(&m_lstPool[tValidIdx]);
      if (pItem == _pItem_)
      {
        pItem->~T();
        m_lstAssignedItems[tIndex] = -1;
        _pItem_ = nullptr;

        std::swap(m_lstAssignedItems[tIndex], m_lstAssignedItems[m_tRegisteredItems - 1]);
        --m_tRegisteredItems;
        return true;
      }
    }
    return false;
  }

  template<typename T, size_t MAX_ITEMS>
  void CFixedPool<T, MAX_ITEMS>::Clear()
  {
    for (size_t tIndex = 0; tIndex < m_tRegisteredItems; ++tIndex)
    {
      size_t tValidIdx = m_lstAssignedItems[tIndex];
      T* pItem = reinterpret_cast<T*>(&m_lstPool[tValidIdx]);
      pItem->~T();
      m_lstAssignedItems[tIndex] = -1;
    }
    m_tRegisteredItems = 0;
  }
}

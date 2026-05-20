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
        if (!m_lstMemoryState[tIndex])
        {
          void* pMem = static_cast<void*>(&m_lstPool[tIndex]);
          ItemType* pItem = new (pMem) ItemType(std::forward<Args>(args)...);

          m_lstMemoryState[tIndex] = true;
          m_lstAssignedItems[m_tRegisteredItems++] = tIndex;
          return pItem;
        }
      }
      return nullptr;
    }

    inline T* operator[](size_t _tIndex)
    {
      bool bAssignedBlock = (_tIndex < m_tRegisteredItems);
      return bAssignedBlock ? reinterpret_cast<T*>(&m_lstPool[m_lstAssignedItems[_tIndex]]) : nullptr;
    }

    inline const T* operator[](size_t _tIndex) const
    {
      bool bAssignedBlock = (_tIndex < m_tRegisteredItems);
      return bAssignedBlock ? reinterpret_cast<const T*>(&m_lstPool[m_lstAssignedItems[_tIndex]]) : nullptr;
    }

    class CIterator
    {
    public:
      CIterator(CFixedPool* _pPool, size_t _tIdx) : m_pPool(_pPool), m_tIndex(_tIdx) {}
      inline T& operator*() { return *(*m_pPool)[m_tIndex]; }
      inline T* operator->() { return (*m_pPool)[m_tIndex]; }
      inline CIterator& operator++() { ++m_tIndex; return *this; }
      inline bool operator!=(const CIterator& _rOther) const { return m_tIndex != _rOther.m_tIndex; }

    private:
      friend class CFixedPool;
      CFixedPool* m_pPool;
      size_t m_tIndex;
    };

    inline CIterator begin() { return CIterator(this, 0); }
    inline CIterator end() { return CIterator(this, m_tRegisteredItems); }

    inline T* last() { return m_tRegisteredItems > 0 ? reinterpret_cast<T*>(&m_lstPool[m_lstAssignedItems[m_tRegisteredItems - 1]]) : nullptr; }
    inline const T* last() const { return m_tRegisteredItems > 0 ? reinterpret_cast<const T*>(&m_lstPool[m_lstAssignedItems[m_tRegisteredItems - 1]]) : nullptr; }

    bool Remove(T*& _pItem_);
    void Clear();

    inline const bool IsEmpty() const { return m_tRegisteredItems == 0; }
    inline const size_t GetSize() const { return m_tRegisteredItems; }
    inline const size_t GetMaxSize() const { return MAX_ITEMS; }

  private:
    void Init()
    {
      m_lstAssignedItems.fill(-1);
      m_lstMemoryState.reset();
    }

  private:
    std::aligned_storage_t<sizeof(T), alignof(std::max_align_t)> m_lstPool[MAX_ITEMS];
    std::array<__int64, MAX_ITEMS> m_lstAssignedItems = std::array<__int64, MAX_ITEMS>();

    std::bitset<MAX_ITEMS> m_lstMemoryState;
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
        _pItem_ = nullptr;

        m_lstMemoryState[tValidIdx] = false;
        m_lstAssignedItems[tIndex] = -1;

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
    }
    m_lstAssignedItems.fill(-1);
    m_lstMemoryState.reset();
    m_tRegisteredItems = 0;
  }
}
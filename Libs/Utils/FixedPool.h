#pragma once
#include <algorithm>
#include <bitset>
#include <cstddef>
#include <cassert>
#include <new>
#include <iostream>
#include <type_traits>

namespace utils
{
  template<typename T, size_t MAX_ITEMS>
  class CFixedPool
  {
  public:
    CFixedPool() : m_tRegisteredItems(0) { Init(); }
    ~CFixedPool() { Clear(); }

    template<typename ...Args>
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
        if (!m_lstAssignedBlocks.test(tIndex))
        {
          void* pMem = static_cast<void*>(&m_lstPool[tIndex]);
          T* pItem = new (pMem) T(std::forward<Args>(args)...);

          m_lstAssignedBlocks.set(tIndex);
          ++m_tRegisteredItems;
          return pItem;
        }
      }

      return nullptr;
    }

    inline T* operator[](size_t _tIndex)
    {
      bool bAssignedBlock = (_tIndex < MAX_ITEMS && m_lstAssignedBlocks.test(_tIndex));
      return bAssignedBlock ? reinterpret_cast<T*>(&m_lstPool[_tIndex]) : nullptr;
    }

    inline const T* operator[](size_t _tIndex) const
    {
      bool bAssignedBlock = (_tIndex < MAX_ITEMS && m_lstAssignedBlocks.test(_tIndex));
      return bAssignedBlock ? reinterpret_cast<const T*>(&m_lstPool[_tIndex]) : nullptr;
    }

    bool Remove(T*& _pItem_);
    void Clear();

    inline T* begin() { return reinterpret_cast<T*>(m_lstPool); }
    inline const T* begin() const { return reinterpret_cast<const T*>(m_lstPool); }

    inline T* end() { return reinterpret_cast<T*>(m_lstPool) + m_tRegisteredItems; }
    inline const T* end() const { return reinterpret_cast<const T*>(m_lstPool) + m_tRegisteredItems; }

    inline T* last() { return reinterpret_cast<T*>(&m_lstPool[m_tRegisteredItems > 0 ? (m_tRegisteredItems - 1) : 0]); }
    inline const T* last() const { return reinterpret_cast<const T*>(&m_lstPool[m_tRegisteredItems > 0 ? (m_tRegisteredItems - 1) : 0]); }

    inline const size_t GetSize() const { return m_tRegisteredItems; }
    inline const size_t GetMaxSize() const { return MAX_ITEMS; }

  private:
    void Init()
    {
      m_lstAssignedBlocks.reset();
    }

  private:
    std::aligned_storage_t<sizeof(T), alignof(std::max_align_t)> m_lstPool[MAX_ITEMS];
    std::bitset<MAX_ITEMS> m_lstAssignedBlocks;
    size_t m_tRegisteredItems;
  };

  template<typename T, size_t MAX_ITEMS>
  bool CFixedPool<T, MAX_ITEMS>::Remove(T*& _pItem_)
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
    {
      if (m_lstAssignedBlocks.test(tIndex))
      {
        T* pItem = reinterpret_cast<T*>(&m_lstPool[tIndex]);
        if (pItem == _pItem_)
        {
          pItem->~T();
          m_lstAssignedBlocks.reset(tIndex);
          _pItem_ = nullptr;
          --m_tRegisteredItems;
          return true;
        }
      }
    }
    return false;
  }

  template<typename T, size_t MAX_ITEMS>
  void CFixedPool<T, MAX_ITEMS>::Clear()
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
    {
      if (m_lstAssignedBlocks.test(tIndex))
      {
        T* pItem = reinterpret_cast<T*>(&m_lstPool[tIndex]);
        pItem->~T();
        m_lstAssignedBlocks.reset(tIndex);
      }
    }
    m_tRegisteredItems = 0;
  }
}

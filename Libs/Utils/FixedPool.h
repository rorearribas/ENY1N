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
    CFixedPool() : m_uRegisteredItems(0) { Init(); }
    ~CFixedPool() { ClearAll(); }

    template<typename ...Args>
    T* CreateItem(Args&&... args)
    {
      if (m_uRegisteredItems >= MAX_ITEMS)
      {
#ifdef _DEBUG
        assert(false);
#endif
        return nullptr;
      }

      for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
      {
        if (!m_vctAssignedBlocks.test(tIndex))
        {
          void* pMem = static_cast<void*>(&m_vctPool[tIndex]);
          T* pItem = new (pMem) T(std::forward<Args>(args)...);

          m_vctAssignedBlocks.set(tIndex);
          ++m_uRegisteredItems;
          return pItem;
        }
      }

      return nullptr;
    }

    bool RemoveItem(T*& _pItem_);
    void ClearAll();

    const uint32_t& CurrentSize() const { return m_uRegisteredItems; }
    size_t GetMaxSize() const { return MAX_ITEMS; }

    T* operator[](size_t index)
    {
      bool bAssignedBlock = (index < MAX_ITEMS && m_vctAssignedBlocks.test(index));
      return bAssignedBlock ? reinterpret_cast<T*>(&m_vctPool[index]) : nullptr;
    }

    const T* operator[](size_t index) const
    {
      bool bAssignedBlock = (index < MAX_ITEMS && m_vctAssignedBlocks.test(index));
      return bAssignedBlock ? reinterpret_cast<const T*>(&m_vctPool[index]) : nullptr;
    }

  private:
    void Init()
    {
      m_vctAssignedBlocks.reset();
    }

  private:
    std::aligned_storage_t<sizeof(T), alignof(std::max_align_t)> m_vctPool[MAX_ITEMS];
    std::bitset<MAX_ITEMS> m_vctAssignedBlocks;
    uint32_t m_uRegisteredItems;
  };

  template<typename T, size_t MAX_ITEMS>
  void CFixedPool<T, MAX_ITEMS>::ClearAll()
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
    {
      if (m_vctAssignedBlocks.test(tIndex))
      {
        T* pItem = reinterpret_cast<T*>(&m_vctPool[tIndex]);
        pItem->~T();
        m_vctAssignedBlocks.reset(tIndex);
      }
    }
    m_uRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  bool CFixedPool<T, MAX_ITEMS>::RemoveItem(T*& _pItem_)
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
    {
      if (m_vctAssignedBlocks.test(tIndex))
      {
        T* pItem = reinterpret_cast<T*>(&m_vctPool[tIndex]);
        if (pItem == _pItem_)
        {
          pItem->~T();
          m_vctAssignedBlocks.reset(tIndex);
          _pItem_ = nullptr;
          --m_uRegisteredItems;
          return true;
        }
      }
    }
    return false;
  }
}

#pragma once
#include <algorithm>
#include <array>
#include <iostream>
#include <new>
#include <cstddef>

namespace utils
{
  template<typename T, size_t MAX_ITEMS>
  class CCircularPool
  {
  public:
    CCircularPool() : m_uRegisteredItems(0), m_tItemIndex(0) { Init(); }
    ~CCircularPool() { ClearAll(); }

    template<typename ...Args>
    T* RegisterItem(Args&&... args)
    {
      // Remove item
      size_t tTargetBlock = m_tItemIndex * sizeof(T);
      T* pPtr = reinterpret_cast<T*>(m_lstPool.data() + tTargetBlock);
      RemoveItem(pPtr);

      // Set block
      void* pMem = reinterpret_cast<void*>(m_lstPool.data() + tTargetBlock);
      T* pItem = new (pMem) T(std::forward<Args>(args)...);

      // Update assigned + current index
      m_lstAssignedBlock[m_tItemIndex] = true;
      m_tItemIndex = (m_tItemIndex + 1) % MAX_ITEMS;
      m_uRegisteredItems++;

      return pItem;
    }
    bool RemoveItem(T*& _pItem_);
    void ClearAll();

    size_t GetMaxSize() { return MAX_ITEMS; }
    const size_t& GetCurrentIndex() const { return m_tItemIndex; }
    const uint32_t& CurrentSize() const { return m_uRegisteredItems; }

    T* operator[](size_t _tIndex)
    {
      if (_tIndex < 0 || _tIndex >= MAX_ITEMS) return nullptr;
      if (!m_lstAssignedBlock[_tIndex]) return nullptr;

      size_t tTargetBlock = _tIndex * sizeof(T);
      return reinterpret_cast<T*>(m_lstPool.data() + tTargetBlock);
    }

  private:
    void Init();

  private:
    std::aligned_storage_t<sizeof(T), alignof(std::max_align_t)> m_lstPool[MAX_ITEMS];
    std::array<bool, MAX_ITEMS> m_lstAssignedBlock;

    uint32_t m_uRegisteredItems;
    size_t m_tItemIndex;
  };

  template<typename T, size_t MAX_ITEMS>
  void CCircularPool<T, MAX_ITEMS>::Init()
  {
    // Initialize
    m_lstAssignedBlock.reset();
    <
  }

  template<typename T, size_t MAX_ITEMS>
  void CCircularPool<T, MAX_ITEMS>::ClearAll()
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; tIndex++)
    {
      if (m_lstAssignedBlock[tIndex])
      {
        size_t tCurrentBlock = tIndex * sizeof(T);
        T* pItem = reinterpret_cast<T*>(m_lstPool.data() + tCurrentBlock);
        pItem->~T();
        memset(m_lstPool.data() + tCurrentBlock, 0, sizeof(T));
        m_lstAssignedBlock[tIndex] = false;
      }
    }
    m_uRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  bool CCircularPool<T, MAX_ITEMS>::RemoveItem(T*& _pItem_)
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
    {
      if (!m_lstAssignedBlock[tIndex])
      {
        continue;
      }
      size_t tTargetBlock = tIndex * sizeof(T);
      T* pItem = reinterpret_cast<T*>(m_lstPool.data() + tIndex * sizeof(T));
      bool bFound = pItem == _pItem_;
      if (bFound)
      {
        pItem->~T();
        memset(m_lstPool.data() + tTargetBlock, 0, sizeof(T));
        _pItem_ = nullptr;

        m_lstAssignedBlock[tIndex] = false;
        --m_uRegisteredItems;
        return true;
      }
    }
    return false;
  }
}

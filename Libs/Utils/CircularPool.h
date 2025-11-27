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
    CCircularPool() : m_tRegisteredItems(0), m_tItemIndex(0) { Init(); }
    ~CCircularPool() { Clear(); }

    template<typename ...Args>
    inline T* RegisterItem(Args&&... args)
    {
      // Remove item
      size_t tTargetBlock = static_cast<size_t>(m_tItemIndex * sizeof(T));
      T* pPtr = reinterpret_cast<T*>(m_lstPool.data() + tTargetBlock);
      Remove(pPtr);

      // Set block
      void* pMem = reinterpret_cast<void*>(m_lstPool.data() + tTargetBlock);
      T* pItem = new (pMem) T(std::forward<Args>(args)...);

      // Update assigned + current index
      m_lstAssignedBlock[m_tItemIndex] = true;
      m_tItemIndex = (m_tItemIndex + 1) % MAX_ITEMS;
      m_tRegisteredItems++;

      return pItem;
    }

    inline T* operator[](size_t _tIndex)
    {
      if (_tIndex < 0 || _tIndex >= MAX_ITEMS || !m_lstAssignedBlock[_tIndex])
      {
        return nullptr;
      }

      size_t tTargetBlock = static_cast<size_t>(_tIndex * sizeof(T));
      return reinterpret_cast<T*>(m_lstPool.data() + tTargetBlock);
    }

    bool Remove(T*& _pItem_);
    void Clear();

    inline T* Begin() { return reinterpret_cast<T*>(m_lstPool); }
    inline const T* Begin() const { return reinterpret_cast<const T*>(m_lstPool); }

    inline T* End() { return reinterpret_cast<T*>(m_lstPool) + m_tRegisteredItems; }
    inline const T* End() const { return reinterpret_cast<const T*>(m_lstPool) + m_tRegisteredItems; }

    inline size_t GetMaxSize() { return MAX_ITEMS; }
    inline const size_t& GetCurrentIndex() const { return m_tItemIndex; }
    inline const size_t& CurrentSize() const { return m_tRegisteredItems; }

    inline T* begin() { return reinterpret_cast<T*>(m_lstPool); }
    inline const T* begin() const { return reinterpret_cast<const T*>(m_lstPool); }

    inline T* end() { return reinterpret_cast<T*>(m_lstPool) + m_tRegisteredItems; }
    inline const T* end() const { return reinterpret_cast<const T*>(m_lstPool) + m_tRegisteredItems; }

  private:
    void Init();

  private:
    std::aligned_storage_t<sizeof(T), alignof(std::max_align_t)> m_lstPool[MAX_ITEMS];
    std::array<bool, MAX_ITEMS> m_lstAssignedBlock;

    size_t m_tRegisteredItems;
    size_t m_tItemIndex;
  };

  template<typename T, size_t MAX_ITEMS>
  void CCircularPool<T, MAX_ITEMS>::Init()
  {
    // Initialize
    m_lstAssignedBlock.reset();
  }

  template<typename T, size_t MAX_ITEMS>
  void CCircularPool<T, MAX_ITEMS>::Clear()
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; tIndex++)
    {
      if (m_lstAssignedBlock[tIndex])
      {
        size_t uCurrentBlock = static_cast<size_t>(tIndex * sizeof(T));
        T* pItem = reinterpret_cast<T*>(m_lstPool.data() + uCurrentBlock);
        pItem->~T();
        memset(m_lstPool.data() + uCurrentBlock, 0, sizeof(T));
        m_lstAssignedBlock[tIndex] = false;
      }
    }
    m_tRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  bool CCircularPool<T, MAX_ITEMS>::Remove(T*& _pItem_)
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
    {
      if (!m_lstAssignedBlock[tIndex])
      {
        continue;
      }
      size_t uTargetBlock = static_cast<size_t>(tIndex * sizeof(T));
      T* pItem = reinterpret_cast<T*>(m_lstPool.data() + tIndex * sizeof(T));
      if (pItem == _pItem_)
      {
        pItem->~T();
        memset(m_lstPool.data() + uTargetBlock, 0, sizeof(T));
        _pItem_ = nullptr;

        m_lstAssignedBlock[tIndex] = false;
        --m_tRegisteredItems;
        return true;
      }
    }
    return false;
  }
}

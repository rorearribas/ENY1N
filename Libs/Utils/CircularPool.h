#pragma once
#include <algorithm>
#include <array>
#include <iostream>
#include <new>
#include <cstddef>

namespace utils
{
  template<typename T, uint32_t MAX_ITEMS>
  class CCircularPool
  {
  public:
    CCircularPool() : m_uRegisteredItems(0), m_uItemIndex(0) { Init(); }
    ~CCircularPool() { Clear(); }

    template<typename ...Args>
    inline T* RegisterItem(Args&&... args)
    {
      // Remove item
      uint32_t uTargetBlock = static_cast<uint32_t>(m_uItemIndex * sizeof(T));
      T* pPtr = reinterpret_cast<T*>(m_lstPool.data() + uTargetBlock);
      Remove(pPtr);

      // Set block
      void* pMem = reinterpret_cast<void*>(m_lstPool.data() + uTargetBlock);
      T* pItem = new (pMem) T(std::forward<Args>(args)...);

      // Update assigned + current index
      m_lstAssignedBlock[m_uItemIndex] = true;
      m_uItemIndex = (m_uItemIndex + 1) % MAX_ITEMS;
      m_uRegisteredItems++;

      return pItem;
    }

    inline T* operator[](uint32_t _uIndex)
    {
      if (_uIndex < 0 || _uIndex >= MAX_ITEMS || !m_lstAssignedBlock[_uIndex])
      {
        return nullptr;
      }

      uint32_t uTargetBlock = static_cast<uint32_t>(_uIndex * sizeof(T));
      return reinterpret_cast<T*>(m_lstPool.data() + uTargetBlock);
    }

    bool Remove(T*& _pItem_);
    void Clear();

    inline T* Begin() { return reinterpret_cast<T*>(m_lstPool); }
    inline const T* Begin() const { return reinterpret_cast<const T*>(m_lstPool); }

    inline T* End() { return reinterpret_cast<T*>(m_lstPool) + m_uRegisteredItems; }
    inline const T* End() const { return reinterpret_cast<const T*>(m_lstPool) + m_uRegisteredItems; }

    inline uint32_t GetMaxSize() { return MAX_ITEMS; }
    inline const uint32_t& GetCurrentIndex() const { return m_uItemIndex; }
    inline const uint32_t& CurrentSize() const { return m_uRegisteredItems; }

    inline T* begin() { return reinterpret_cast<T*>(m_lstPool); }
    inline const T* begin() const { return reinterpret_cast<const T*>(m_lstPool); }

    inline T* end() { return reinterpret_cast<T*>(m_lstPool) + m_uRegisteredItems; }
    inline const T* end() const { return reinterpret_cast<const T*>(m_lstPool) + m_uRegisteredItems; }

  private:
    void Init();

  private:
    std::aligned_storage_t<sizeof(T), alignof(std::max_align_t)> m_lstPool[MAX_ITEMS];
    std::array<bool, MAX_ITEMS> m_lstAssignedBlock;

    uint32_t m_uRegisteredItems;
    uint32_t m_uItemIndex;
  };

  template<typename T, uint32_t MAX_ITEMS>
  void CCircularPool<T, MAX_ITEMS>::Init()
  {
    // Initialize
    m_lstAssignedBlock.reset();
  }

  template<typename T, uint32_t MAX_ITEMS>
  void CCircularPool<T, MAX_ITEMS>::Clear()
  {
    for (uint32_t uIndex = 0; uIndex < MAX_ITEMS; uIndex++)
    {
      if (m_lstAssignedBlock[uIndex])
      {
        uint32_t uCurrentBlock = static_cast<uint32_t>(uIndex * sizeof(T));
        T* pItem = reinterpret_cast<T*>(m_lstPool.data() + uCurrentBlock);
        pItem->~T();
        memset(m_lstPool.data() + uCurrentBlock, 0, sizeof(T));
        m_lstAssignedBlock[uIndex] = false;
      }
    }
    m_uRegisteredItems = 0;
  }

  template<typename T, uint32_t MAX_ITEMS>
  bool CCircularPool<T, MAX_ITEMS>::Remove(T*& _pItem_)
  {
    for (uint32_t uIndex = 0; uIndex < MAX_ITEMS; ++uIndex)
    {
      if (!m_lstAssignedBlock[uIndex])
      {
        continue;
      }
      uint32_t uTargetBlock = static_cast<uint32_t>(uIndex * sizeof(T));
      T* pItem = reinterpret_cast<T*>(m_lstPool.data() + uIndex * sizeof(T));
      if (pItem == _pItem_)
      {
        pItem->~T();
        memset(m_lstPool.data() + uTargetBlock, 0, sizeof(T));
        _pItem_ = nullptr;

        m_lstAssignedBlock[uIndex] = false;
        --m_uRegisteredItems;
        return true;
      }
    }
    return false;
  }
}

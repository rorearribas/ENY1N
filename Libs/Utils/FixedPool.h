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
  template<typename T, uint32_t MAX_ITEMS>
  class CFixedPool
  {
  public:
    CFixedPool() : m_uRegisteredItems(0) { Init(); }
    ~CFixedPool() { Clear(); }

    template<typename ...Args>
    inline T* Create(Args&&... args)
    {
      if (m_uRegisteredItems >= MAX_ITEMS)
      {
#ifdef _DEBUG
        assert(false);
#endif
        return nullptr;
      }

      for (uint32_t uIndex = 0; uIndex < MAX_ITEMS; ++uIndex)
      {
        if (!m_lstAssignedBlocks.test(uIndex))
        {
          void* pMem = static_cast<void*>(&m_lstPool[uIndex]);
          T* pItem = new (pMem) T(std::forward<Args>(args)...);

          m_lstAssignedBlocks.set(uIndex);
          ++m_uRegisteredItems;
          return pItem;
        }
      }

      return nullptr;
    }

    inline T* operator[](uint32_t _uIndex)
    {
      bool bAssignedBlock = (_uIndex < MAX_ITEMS && m_lstAssignedBlocks.test(_uIndex));
      return bAssignedBlock ? reinterpret_cast<T*>(&m_lstPool[_uIndex]) : nullptr;
    }

    inline const T* operator[](uint32_t _uIndex) const
    {
      bool bAssignedBlock = (_uIndex < MAX_ITEMS && m_lstAssignedBlocks.test(_uIndex));
      return bAssignedBlock ? reinterpret_cast<const T*>(&m_lstPool[_uIndex]) : nullptr;
    }

    bool Remove(T*& _pItem_);
    void Clear();

    inline T* begin() { return reinterpret_cast<T*>(m_lstPool); }
    inline const T* begin() const { return reinterpret_cast<const T*>(m_lstPool); }

    inline T* end() { return reinterpret_cast<T*>(m_lstPool) + m_uRegisteredItems; }
    inline const T* end() const { return reinterpret_cast<const T*>(m_lstPool) + m_uRegisteredItems; }

    inline T* last() { return reinterpret_cast<T*>(&m_lstPool[m_uRegisteredItems > 0 ? (m_uRegisteredItems - 1) : 0]); }
    inline const T* last() const { return reinterpret_cast<const T*>(&m_lstPool[m_uRegisteredItems > 0 ? (m_uRegisteredItems - 1) : 0]); }

    inline const uint32_t& GetCurrentSize() const { return m_uRegisteredItems; }
    inline uint32_t GetMaxSize() const { return MAX_ITEMS; }

  private:
    void Init()
    {
      m_lstAssignedBlocks.reset();
    }

  private:
    std::aligned_storage_t<sizeof(T), alignof(std::max_align_t)> m_lstPool[MAX_ITEMS];
    std::bitset<MAX_ITEMS> m_lstAssignedBlocks;
    uint32_t m_uRegisteredItems;
  };

  template<typename T, uint32_t MAX_ITEMS>
  bool CFixedPool<T, MAX_ITEMS>::Remove(T*& _pItem_)
  {
    for (uint32_t uIndex = 0; uIndex < MAX_ITEMS; ++uIndex)
    {
      if (m_lstAssignedBlocks.test(uIndex))
      {
        T* pItem = reinterpret_cast<T*>(&m_lstPool[uIndex]);
        if (pItem == _pItem_)
        {
          pItem->~T();
          m_lstAssignedBlocks.reset(uIndex);
          _pItem_ = nullptr;
          --m_uRegisteredItems;
          return true;
        }
      }
    }
    return false;
  }

  template<typename T, uint32_t MAX_ITEMS>
  void CFixedPool<T, MAX_ITEMS>::Clear()
  {
    for (uint32_t uIndex = 0; uIndex < MAX_ITEMS; ++uIndex)
    {
      if (m_lstAssignedBlocks.test(uIndex))
      {
        T* pItem = reinterpret_cast<T*>(&m_lstPool[uIndex]);
        pItem->~T();
        m_lstAssignedBlocks.reset(uIndex);
      }
    }
    m_uRegisteredItems = 0;
  }
}

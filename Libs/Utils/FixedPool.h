#pragma once
#include <algorithm>
#include <bitset>
#include <cstddef>
#include <cassert>
#include <new>
#include <iostream>

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
        if (!m_vctAssignedBlock.test(tIndex))
        {
          size_t offset = tIndex * sizeof(T);
          void* pMem = static_cast<void*>(m_vctBytes + offset);
          T* pItem = new (pMem) T(std::forward<Args>(args)...);

          m_vctAssignedBlock.set(tIndex);
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

    T* operator[](size_t _tIndex)
    {
      if (_tIndex >= MAX_ITEMS || !m_vctAssignedBlock.test(_tIndex))
        return nullptr;

      size_t offset = _tIndex * sizeof(T);
      return reinterpret_cast<T*>(m_vctBytes + offset);
    }

  private:
    void Init();

  private:
    alignas(T) unsigned char m_vctBytes[sizeof(T) * MAX_ITEMS];
    std::bitset<MAX_ITEMS> m_vctAssignedBlock;
    uint32_t m_uRegisteredItems;
  };

  template<typename T, size_t MAX_ITEMS>
  void utils::CFixedPool<T, MAX_ITEMS>::Init()
  {
    // Initialize
    std::fill(m_vctBytes, m_vctBytes + (sizeof(T) * MAX_ITEMS), static_cast<unsigned char>(0));
    m_vctAssignedBlock.reset();
  }

  template<typename T, size_t MAX_ITEMS>
  void CFixedPool<T, MAX_ITEMS>::ClearAll()
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
    {
      if (m_vctAssignedBlock.test(tIndex))
      {
        size_t offset = tIndex * sizeof(T);
        T* pItem = reinterpret_cast<T*>(m_vctBytes + offset);
        pItem->~T();
        std::memset(m_vctBytes + offset, 0, sizeof(T));
        m_vctAssignedBlock.reset(tIndex);
      }
    }
    m_uRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  bool CFixedPool<T, MAX_ITEMS>::RemoveItem(T*& _pItem_)
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
    {
      if (m_vctAssignedBlock.test(tIndex))
      {
        size_t offset = tIndex * sizeof(T);
        T* pItem = reinterpret_cast<T*>(m_vctBytes + offset);

        if (pItem == _pItem_)
        {
          pItem->~T();
          std::memset(m_vctBytes + offset, 0, sizeof(T));
          m_vctAssignedBlock.reset(tIndex);
          _pItem_ = nullptr;
          --m_uRegisteredItems;
          return true;
        }
      }
    }

    return false;
  }
}

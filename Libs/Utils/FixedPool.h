#pragma once
#include <algorithm>
#include <array>
#include <iostream>
#include <new>
#include <cstddef>
#include <cassert>

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

      // Find valid block
      T* pItem = nullptr;
      for (size_t tIndex = 0; tIndex < MAX_ITEMS; tIndex++)
      {
        if (!m_vctAssignedBlock[tIndex])
        {
          size_t tTargetPos = tIndex * sizeof(T);
          void* pMem = reinterpret_cast<void*>(m_vctBytes.data() + tTargetPos);
          pItem = new (pMem) T(std::forward<Args>(args)...);

          // Register
          m_vctAssignedBlock[tIndex] = true;
          m_uRegisteredItems++;
          break;
        }
      }

      return pItem;
    }
    bool RemoveItem(T*& _pItem_);
    void ClearAll();

    const uint32_t& CurrentSize() const { return m_uRegisteredItems; }
    size_t GetMaxSize() { return static_cast<size_t>(MAX_ITEMS); }

    T* operator[](size_t _tIndex)
    {
      if (_tIndex < 0 || _tIndex >= MAX_ITEMS) return nullptr;
      if (!m_vctAssignedBlock[_tIndex]) return nullptr;

      size_t tTargetBlock = _tIndex * sizeof(T);
      return reinterpret_cast<T*>(m_vctBytes.data() + tTargetBlock);
    }

  private:
    void Init();

  private:
    alignas(T)std::array<std::byte, sizeof(T)* MAX_ITEMS> m_vctBytes;
    std::array<bool, MAX_ITEMS> m_vctAssignedBlock;
    uint32_t m_uRegisteredItems;
  };

  template<typename T, size_t MAX_ITEMS>
  void CFixedPool<T, MAX_ITEMS>::Init()
  {
    // Initialize
    std::fill(m_vctAssignedBlock.begin(), m_vctAssignedBlock.end(), false);
    std::fill(m_vctBytes.begin(), m_vctBytes.end(), std::byte{ 0 });
  }

  template<typename T, size_t MAX_ITEMS>
  void CFixedPool<T, MAX_ITEMS>::ClearAll()
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; tIndex++)
    {
      if (m_vctAssignedBlock[tIndex])
      {
        size_t tCurrentBlock = tIndex * sizeof(T);
        T* pItem = reinterpret_cast<T*>(m_vctBytes.data() + tCurrentBlock);
        pItem->~T();
        memset(m_vctBytes.data() + tCurrentBlock, 0, sizeof(T));
        m_vctAssignedBlock[tIndex] = false;
      }
    }
    m_uRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  bool CFixedPool<T, MAX_ITEMS>::RemoveItem(T*& _pItem_)
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
    {
      if (!m_vctAssignedBlock[tIndex])
      {
        continue;
      }

      size_t tTargetBlock = tIndex * sizeof(T);
      T* pItem = reinterpret_cast<T*>(m_vctBytes.data() + tIndex * sizeof(T));
      bool bFound = pItem == _pItem_;
      if (bFound)
      {
        pItem->~T();
        memset(m_vctBytes.data() + tTargetBlock, 0, sizeof(T));
        _pItem_ = nullptr;

        m_vctAssignedBlock[tIndex] = false;
        --m_uRegisteredItems;
        return true;
      }

    }
    return false;
  }
}

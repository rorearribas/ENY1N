#pragma once
#include <algorithm>
#include <array>
#include <iostream>
#include <new>
#include <cstddef>

namespace utils
{
  template<typename T, size_t MAX_ITEMS>
  class CMemoryPool
  {
  public:
    CMemoryPool()
    {
      for (size_t tIndex = 0; tIndex < MAX_ITEMS; tIndex++)
      {
        m_vctBlockAllocated[tIndex] = false;
      }
    }

    template<typename ...Args>
    T* CreateItem(Args&&... args)
    {
      if (m_uRegisteredItems >= MAX_ITEMS)
      {
        std::cout << "Maximum items reached!" << std::endl;
        return nullptr;
      }

      // Find valid block
      T* pItem = nullptr;
      for (size_t tIndex = 0; tIndex < MAX_ITEMS; tIndex++)
      {
        if (!m_vctBlockAllocated[tIndex])
        {
          size_t tTargetPos = tIndex * sizeof(T);
          void* pMem = reinterpret_cast<void*>(m_vctItemList.data() + tTargetPos);
          pItem = new (pMem) T(std::forward<Args>(args)...);

          // Register
          m_vctBlockAllocated[tIndex] = true;
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

    T* operator[](int _iIndex)
    {
      if (_iIndex < 0 || _iIndex >= static_cast<int>(MAX_ITEMS)) return nullptr;
      if (!m_vctBlockAllocated[_iIndex]) return nullptr;
      return reinterpret_cast<T*>(m_vctItemList.data() + _iIndex * sizeof(T));
    }

  private:
    alignas(T)std::array<std::byte, sizeof(T)* MAX_ITEMS> m_vctItemList = std::array<std::byte, sizeof(T) * MAX_ITEMS>();
    std::array<bool, MAX_ITEMS> m_vctBlockAllocated = std::array<bool, MAX_ITEMS>();
    uint32_t m_uRegisteredItems = 0;
  };

  template<typename T, size_t MAX_ITEMS>
  void CMemoryPool<T, MAX_ITEMS>::ClearAll()
  {
    for (size_t iIndex = 0; iIndex < MAX_ITEMS; iIndex++)
    {
      if (m_vctBlockAllocated[iIndex])
      {
        size_t tTargetBlock = iIndex * sizeof(T);
        T* pItem = reinterpret_cast<T*>(m_vctItemList.data() + tTargetBlock);
        pItem->~T();
        memset(m_vctItemList.data() + tTargetBlock, 0, sizeof(T));
        m_vctBlockAllocated[iIndex] = false;
      }
    }
    m_uRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  bool CMemoryPool<T, MAX_ITEMS>::RemoveItem(T*& _pItem_)
  {
    for (size_t iIndex = 0; iIndex < MAX_ITEMS; ++iIndex)
    {
      if (!m_vctBlockAllocated[iIndex])
      {
        continue;
      }

      size_t tTargetBlock = iIndex * sizeof(T);
      T* pItem = reinterpret_cast<T*>(m_vctItemList.data() + iIndex * sizeof(T));
      if (pItem == _pItem_)
      {
        pItem->~T();
        memset(m_vctItemList.data() + tTargetBlock, 0, sizeof(T));
        _pItem_ = nullptr;

        m_vctBlockAllocated[iIndex] = false;
        --m_uRegisteredItems;
        return true;
      }
    }
    return false;
  }
}

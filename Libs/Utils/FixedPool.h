#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <bitset>
#include <cstddef>
#include <new>
#include <type_traits>
#include "WeakPtr.h"

namespace utils
{
  template<typename T, size_t MAX_ITEMS>
  class CFixedPool
  {
  private:
    struct TSlotData
    {
      size_t tDenseIndex = 0;
      size_t tGeneration = 0;
    };

    struct TInternalData
    {
      alignas(alignof(T)) unsigned char Data[sizeof(T)];
      size_t SlotIdx = 0;

      inline T* Get() { return reinterpret_cast<T*>(Data); }
      inline const T* Get() const { return reinterpret_cast<const T*>(Data); }
    };

  public:
    CFixedPool() { Init(); }
    ~CFixedPool() { Clear(); }

    template<typename _Type = T, typename ...Args>
    inline CWeakPtr<_Type> Create(Args&&... args)
    {
      static_assert(std::is_base_of_v<T, _Type>, "_Type must inherit from or be of type T");
      if (m_tRegisteredItems >= MAX_ITEMS)
      {
#ifdef _DEBUG
        assert(false);
#endif
        return CWeakPtr<_Type>();
      }

      for (size_t tSlotIdx = 0; tSlotIdx < MAX_ITEMS; ++tSlotIdx)
      {
        if (!m_lstSlotStates[tSlotIdx])
        {
          size_t tDenseIdx = m_tRegisteredItems;
          TInternalData& rInternalData = m_lstInternalData[tDenseIdx];
          TSlotData& pSlot = m_lstSparseSlots[tSlotIdx];

          new (rInternalData.Data) _Type(std::forward<Args>(args)...);
          rInternalData.SlotIdx = tSlotIdx;

          pSlot.tDenseIndex = tDenseIdx;
          pSlot.tGeneration++;
          m_lstSlotStates[tSlotIdx] = true;

          m_tRegisteredItems++;
          _Type* pRawPtr = static_cast<_Type*>(rInternalData.Get());
          return CWeakPtr<_Type>(pRawPtr, &pSlot.tGeneration, pSlot.tGeneration);
        }
      }

      return CWeakPtr<_Type>();
    }

    inline CWeakPtr<T> operator[](size_t _tIndex)
    {
      if (_tIndex >= m_tRegisteredItems)
      {
        return CWeakPtr<T>();
      }
      TInternalData& rInternalData = m_lstInternalData[_tIndex];
      TSlotData& rSlot = m_lstSparseSlots[rInternalData.SlotIdx];
      return CWeakPtr<T>(rInternalData.Get(), &rSlot.tGeneration, rSlot.tGeneration);
    }

    inline const CWeakPtr<T> operator[](size_t _tIndex) const
    {
      if (_tIndex >= m_tRegisteredItems)
      {
        return CWeakPtr<T>();
      }
      const TInternalData& rInternalData = m_lstInternalData[_tIndex];
      const TSlotData& rSlot = m_lstSparseSlots[rInternalData.SlotIdx];
      return CWeakPtr<T>(const_cast<T*>(rInternalData.Get()), const_cast<size_t*>(&rSlot.tGeneration), rSlot.tGeneration);
    }

    class CIterator
    {
    public:
      CIterator(CFixedPool* _pList, size_t _tIdx) : m_pList(_pList), m_tIndex(_tIdx) {}

      inline T* operator*() { return m_pList->m_lstInternalData[m_tIndex].Get(); }
      inline T* operator->() { return m_pList->m_lstInternalData[m_tIndex].Get(); }

      inline CIterator& operator++() { ++m_tIndex; return *this; }
      inline bool operator!=(const CIterator& _rOther) const { return m_tIndex != _rOther.m_tIndex; }

    private:
      CFixedPool* m_pList;
      size_t m_tIndex;
    };

    inline T* front() const { return m_tRegisteredItems > 0 ? const_cast<T*>(m_lstInternalData[0].Get()) : nullptr; }
    inline CIterator begin() { return CIterator(this, 0); }

    inline T* back() const { return m_tRegisteredItems > 0 ? const_cast<T*>(m_lstInternalData[m_tRegisteredItems - 1].Get()) : nullptr; }
    inline CIterator end() { return CIterator(this, m_tRegisteredItems); }

    size_t FindIndex(const CWeakPtr<T>& _pItem);
    bool RemoveAt(size_t _tIndex);

    bool Remove(const CWeakPtr<T>& _pItem);
    bool Remove(T*& _pItem_);
    void Clear();

    inline const size_t& GetSize() const { return m_tRegisteredItems; }
    inline const bool IsEmpty() const { return m_tRegisteredItems == 0; }
    inline size_t GetMaxSize() const { return MAX_ITEMS; }

  private:
    void Init()
    {
      m_lstSlotStates.reset();
    }

  private:
    std::array<TInternalData, MAX_ITEMS> m_lstInternalData = std::array<TInternalData, MAX_ITEMS>();
    std::array<TSlotData, MAX_ITEMS> m_lstSparseSlots = std::array<TSlotData, MAX_ITEMS>();

    std::bitset<MAX_ITEMS> m_lstSlotStates;
    size_t m_tRegisteredItems = 0;
  };

  template<typename T, size_t MAX_ITEMS>
  bool CFixedPool<T, MAX_ITEMS>::Remove(T*& _pItem_)
  {
    size_t tIndex = FindIndex(CWeakPtr<T>(_pItem_, nullptr, 0));
    if (tIndex == static_cast<size_t>(-1))
    {
      for (size_t i = 0; i < m_tRegisteredItems; i++)
      {
        if (m_lstInternalData[i].Get() == _pItem_)
        {
          tIndex = i;
          break;
        }
      }
    }

    if (tIndex != static_cast<size_t>(-1) && RemoveAt(tIndex))
    {
      _pItem_ = nullptr;
      return true;
    }
    return false;
  }

  template<typename T, size_t MAX_ITEMS>
  void CFixedPool<T, MAX_ITEMS>::Clear()
  {
    if (m_tRegisteredItems == 0)
    {
      return;
    }

    // Flush data
    for (size_t tIndex = 0; tIndex < m_tRegisteredItems; tIndex++)
    {
      m_lstInternalData[tIndex].Get()->~T();
    }

    // Update sparse slots
    for (size_t tIndex = 0; tIndex < m_tRegisteredItems; tIndex++)
    {
      m_lstSparseSlots[tIndex].tGeneration++;
    }

    // Reset state
    m_lstSlotStates.reset();
    m_tRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  size_t CFixedPool<T, MAX_ITEMS>::FindIndex(const CWeakPtr<T>& _pItem)
  {
    if (!_pItem.IsValid() || _pItem.GetTargetGen() == nullptr)
    {
      return static_cast<size_t>(-1);
    }

    ptrdiff_t tSlotIdx = _pItem.GetTargetGen() - &m_lstSparseSlots[0].tGeneration;
    if (tSlotIdx < 0 || tSlotIdx >= static_cast<ptrdiff_t>(MAX_ITEMS))
    {
      return static_cast<size_t>(-1);
    }

    size_t tDenseIdx = m_lstSparseSlots[tSlotIdx].tDenseIndex;
    if (tDenseIdx < m_tRegisteredItems && m_lstInternalData[tDenseIdx].SlotIdx == static_cast<size_t>(tSlotIdx))
    {
      return tDenseIdx;
    }
    return static_cast<size_t>(-1);
  }

  template<typename T, size_t MAX_ITEMS>
  bool CFixedPool<T, MAX_ITEMS>::Remove(const CWeakPtr<T>& _pItem)
  {
    size_t tIndex = FindIndex(_pItem);
    if (tIndex != static_cast<size_t>(-1))
    {
      return RemoveAt(tIndex);
    }
    return false;
  }

  template<typename T, size_t MAX_ITEMS>
  bool CFixedPool<T, MAX_ITEMS>::RemoveAt(size_t _tIndex)
  {
    if (_tIndex >= m_tRegisteredItems)
    {
      return false;
    }

    // Destructor
    m_lstInternalData[_tIndex].Get()->~T();

    size_t tSlotToDelete = m_lstInternalData[_tIndex].SlotIdx;
    m_lstSlotStates[tSlotToDelete] = false;
    m_lstSparseSlots[tSlotToDelete].tGeneration++;

    // Readjust
    size_t tLastDenseIdx = m_tRegisteredItems - 1;
    for (size_t i = _tIndex; i < tLastDenseIdx; ++i)
    {
      TInternalData& rDestData = m_lstInternalData[i];
      TInternalData& rSrcData = m_lstInternalData[i + 1];

      new (rDestData.Data) T(std::move(*rSrcData.Get()));
      rSrcData.Get()->~T();

      rDestData.SlotIdx = rSrcData.SlotIdx;
      m_lstSparseSlots[rDestData.SlotIdx].tDenseIndex = i;
    }

    --m_tRegisteredItems;
    return true;
  }
}
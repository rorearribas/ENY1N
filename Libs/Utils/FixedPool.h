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
          TInternalData& rInternalData = m_lstInternalData[tSlotIdx];
          TSlotData& rSlot = m_lstSparseSlots[tSlotIdx];

          new (rInternalData.Data) _Type(std::forward<Args>(args)...);
          rInternalData.SlotIdx = tSlotIdx;

          size_t tNewDenseIdx = m_tRegisteredItems;
          m_lstDenseOrder[tNewDenseIdx] = tSlotIdx;

          rSlot.tDenseIndex = tNewDenseIdx;
          rSlot.tGeneration++;
          m_lstSlotStates[tSlotIdx] = true;

          m_tRegisteredItems++;
          _Type* pRawPtr = static_cast<_Type*>(rInternalData.Get());
          return CWeakPtr<_Type>(pRawPtr, &rSlot.tGeneration, rSlot.tGeneration);
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
      size_t tSlotIdx = m_lstDenseOrder[_tIndex];
      TSlotData& rSlot = m_lstSparseSlots[tSlotIdx];
      TInternalData& rInternalData = m_lstInternalData[tSlotIdx];
      return CWeakPtr<T>(rInternalData.Get(), &rSlot.tGeneration, rSlot.tGeneration);
    }

    inline const CWeakPtr<T> operator[](size_t _tIndex) const
    {
      if (_tIndex >= m_tRegisteredItems)
      {
        return CWeakPtr<T>();
      }
      size_t tSlotIdx = m_lstDenseOrder[_tIndex];
      const TSlotData& rSlot = m_lstSparseSlots[tSlotIdx];
      const TInternalData& rInternalData = m_lstInternalData[tSlotIdx];
      return CWeakPtr<T>(const_cast<T*>(rInternalData.Get()), const_cast<size_t*>(&rSlot.tGeneration), rSlot.tGeneration);
    }

    class CIterator
    {
    public:
      CIterator(CFixedPool* _pList, size_t _tIdx) : m_pList(_pList), m_tIndex(_tIdx) {}

      inline T* operator*() { return m_pList->m_lstInternalData[m_pList->m_lstDenseOrder[m_tIndex]].Get(); }
      inline T* operator->() { return m_pList->m_lstInternalData[m_pList->m_lstDenseOrder[m_tIndex]].Get(); }

      inline CIterator& operator++() { ++m_tIndex; return *this; }
      inline bool operator!=(const CIterator& _rOther) const { return m_tIndex != _rOther.m_tIndex; }

    private:
      CFixedPool* m_pList;
      size_t m_tIndex;
    };

    inline T* front() const { return m_tRegisteredItems > 0 ? const_cast<T*>(m_lstInternalData[m_lstDenseOrder[0]].Get()) : nullptr; }
    inline CIterator begin() { return CIterator(this, 0); }

    inline T* back() const { return m_tRegisteredItems > 0 ? const_cast<T*>(m_lstInternalData[m_lstDenseOrder[m_tRegisteredItems - 1]].Get()) : nullptr; }
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
    std::array<size_t, MAX_ITEMS> m_lstDenseOrder = std::array<size_t, MAX_ITEMS>();

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
        size_t tSlotIdx = m_lstDenseOrder[i];
        if (m_lstInternalData[tSlotIdx].Get() == _pItem_)
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

    for (size_t tIndex = 0; tIndex < m_tRegisteredItems; tIndex++)
    {
      size_t tSlotIdx = m_lstDenseOrder[tIndex];
      m_lstInternalData[tSlotIdx].Get()->~T();
      m_lstSparseSlots[tSlotIdx].tGeneration++;
    }

    m_lstSlotStates.reset();
    m_tRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  size_t CFixedPool<T, MAX_ITEMS>::FindIndex(const CWeakPtr<T>& _pItem)
  {
    if (!_pItem.IsValid())
    {
      return static_cast<size_t>(-1);
    }

    const char* pBlock = reinterpret_cast<const char*>(_pItem.GetPtr());
    const char* pBeginBlock = reinterpret_cast<const char*>(&m_lstInternalData[0]);

    ptrdiff_t tDiff = pBlock - pBeginBlock;
    ptrdiff_t tSlotIdx = tDiff / sizeof(TInternalData);

    if (tSlotIdx < 0 || tSlotIdx >= static_cast<ptrdiff_t>(MAX_ITEMS))
    {
      return static_cast<size_t>(-1);
    }

    if (!m_lstSlotStates[tSlotIdx])
    {
      return static_cast<size_t>(-1);
    }

    return m_lstSparseSlots[tSlotIdx].tDenseIndex;
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

    size_t tSlotIdxToDelete = m_lstDenseOrder[_tIndex];

    m_lstInternalData[tSlotIdxToDelete].Get()->~T();
    m_lstSlotStates[tSlotIdxToDelete] = false;
    m_lstSparseSlots[tSlotIdxToDelete].tGeneration++;

    for (size_t i = _tIndex; i < m_tRegisteredItems - 1; ++i)
    {
      m_lstDenseOrder[i] = m_lstDenseOrder[i + 1];
      size_t tMovedSlotIdx = m_lstDenseOrder[i];
      m_lstSparseSlots[tMovedSlotIdx].tDenseIndex = i;
    }

    --m_tRegisteredItems;
    return true;
  }
}
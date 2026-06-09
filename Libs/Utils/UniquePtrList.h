#pragma once
#include <array>
#include <cassert>
#include <bitset>
#include "WeakPtr.h"

namespace utils
{
  template<typename T, size_t MAX_ITEMS>
  class CUniquePtrList
  {
  private:
    struct TSlotData
    {
      size_t tDenseIndex = 0;
      size_t tGeneration = 0;
    };

    struct TInternalData
    {
      std::unique_ptr<T> uPtr = nullptr;
      size_t tSlotIndex = 0;
    };

  public:
    CUniquePtrList() { Init(); }
    ~CUniquePtrList() { Clear(); }

    template<typename _Type = T, typename ...Args>
    inline CWeakPtr<_Type> Create(Args&&... args)
    {
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
          TSlotData& rSlot = m_lstSparseSlots[tSlotIdx];

          // Create ptr
          rInternalData.uPtr = std::make_unique<_Type>(std::forward<Args>(args)...);
          rInternalData.tSlotIndex = tSlotIdx;

          // Set slot
          rSlot.tDenseIndex = tDenseIdx;
          rSlot.tGeneration++;
          m_lstSlotStates[tSlotIdx] = true;

          m_tRegisteredItems++;
          _Type* pRawPtr = static_cast<_Type*>(rInternalData.uPtr.get());
          return CWeakPtr<_Type>(pRawPtr, &rSlot.tGeneration, rSlot.tGeneration);
        }
      }

      return CWeakPtr<_Type>();
    }

    template<typename _Type = T, typename ...Args>
    inline CWeakPtr<_Type> Insert(std::unique_ptr<_Type> _pData)
    {
      if (m_tRegisteredItems >= MAX_ITEMS || !_pData)
      {
        return CWeakPtr<_Type>();
      }

      for (size_t tSlotIdx = 0; tSlotIdx < MAX_ITEMS; ++tSlotIdx)
      {
        if (!m_lstSlotStates[tSlotIdx])
        {
          size_t tDenseIdx = m_tRegisteredItems;
          TInternalData& rInternalData = m_lstInternalData[tDenseIdx];
          TSlotData& rSlot = m_lstSparseSlots[tSlotIdx];

          rInternalData.uPtr = std::move(_pData);
          rInternalData.tSlotIndex = tSlotIdx;

          rSlot.tDenseIndex = tDenseIdx;
          rSlot.tGeneration++;
          m_lstSlotStates[tSlotIdx] = true;

          m_tRegisteredItems++;
          _Type* pRawPtr = static_cast<_Type*>(rInternalData.uPtr.get());
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
      TInternalData& rInternalData = m_lstInternalData[_tIndex];
      TSlotData& rSlot = m_lstSparseSlots[rInternalData.tSlotIndex];
      return CWeakPtr<T>(rInternalData.uPtr.get(), &rSlot.tGeneration, rSlot.tGeneration);
    }

    inline const CWeakPtr<T> operator[](size_t _tIndex) const
    {
      if (_tIndex >= m_tRegisteredItems)
      {
        return CWeakPtr<T>();
      }
      const TInternalData& rInternalData = m_lstInternalData[_tIndex];
      const TSlotData& rSlot = m_lstSparseSlots[rInternalData.tSlotIndex];
      return CWeakPtr<T>(rInternalData.uPtr.get(), const_cast<size_t*>(&rSlot.tGeneration), rSlot.tGeneration);
    }

    class CIterator
    {
    public:
      CIterator(CUniquePtrList* _pList, size_t _tIdx) : 
      m_pList(_pList), m_tIndex(_tIdx) {}

      inline T* operator*() { return m_pList->m_lstInternalData[m_tIndex].uPtr.get(); }
      inline T* operator->() { return m_pList->m_lstInternalData[m_tIndex].uPtr.get(); }

      inline CIterator& operator++() { ++m_tIndex; return *this; }
      inline bool operator!=(const CIterator& _rOther) const { return m_tIndex != _rOther.m_tIndex; }

    private:
      CUniquePtrList* m_pList;
      size_t m_tIndex;
    };

    inline T* front() const { return m_tRegisteredItems > 0 ? m_lstInternalData[0].uPtr.get() : nullptr; }
    inline CIterator begin() { return CIterator(this, 0); }

    inline T* back() const { return m_tRegisteredItems > 0 ? m_lstInternalData[m_tRegisteredItems - 1].uPtr.get() : nullptr; }
    inline CIterator end() { return CIterator(this, m_tRegisteredItems); }

    size_t FindIndex(const CWeakPtr<T>& _pItem);
    bool Remove(const CWeakPtr<T>& _pItem);
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
    std::array<TSlotData, MAX_ITEMS>  m_lstSparseSlots = std::array<TSlotData, MAX_ITEMS>();

    std::bitset<MAX_ITEMS> m_lstSlotStates;
    size_t m_tRegisteredItems = 0;
  };

  template<typename T, size_t MAX_ITEMS>
  void CUniquePtrList<T, MAX_ITEMS>::Clear()
  {
    for (size_t tIndex = 0; tIndex < m_tRegisteredItems; ++tIndex)
    {
      m_lstInternalData[tIndex].uPtr.reset();
    }
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
    {
      m_lstSparseSlots[tIndex].tGeneration++;
    }
    m_lstSlotStates.reset();
    m_tRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  size_t utils::CUniquePtrList<T, MAX_ITEMS>::FindIndex(const CWeakPtr<T>& _pItem)
  {
    if (!_pItem.IsValid())
    {
      return static_cast<size_t>(-1);
    }

    const char* pBlock = reinterpret_cast<const char*>((_pItem.GetTargetGen()));
    const char* pEndBlock = pBlock - offsetof(TSlotData, tGeneration);
    const char* pBeginBlock = reinterpret_cast<const char*>(&m_lstSparseSlots[0]);

    ptrdiff_t tDiff = pEndBlock - pBeginBlock;
    ptrdiff_t tSlotIdx = tDiff / sizeof(TSlotData);

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
  bool CUniquePtrList<T, MAX_ITEMS>::Remove(const CWeakPtr<T>& _pItem)
  {
    if (!_pItem.IsValid())
    {
      return false;
    }

    for (size_t tIndex = 0; tIndex < m_tRegisteredItems; ++tIndex)
    {
      if (m_lstInternalData[tIndex].uPtr.get() == _pItem.GetPtr())
      {
        m_lstSlotStates[tIndex] = false;
        m_lstSparseSlots[tIndex].tGeneration++;
        m_lstInternalData[tIndex].uPtr.reset();

        size_t tLastDenseIdx = (m_tRegisteredItems - 1);
        for (size_t i = tIndex; i < tLastDenseIdx; ++i)
        {
          m_lstInternalData[i] = std::move(m_lstInternalData[i + 1]);
          m_lstSparseSlots[m_lstInternalData[i].tSlotIndex].tDenseIndex = i + 1;
        }

        --m_tRegisteredItems;
        return true;
      }
    }
    return false;
  }
}
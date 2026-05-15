#pragma once
#include <algorithm>
#include <array>
#include <memory>

namespace utils
{
  template<typename T>
  class CWeakPtr
  {
  public:
    CWeakPtr() = default;
    CWeakPtr(T* _pPtr, size_t* _uTargetGen, size_t _uCurrentGen) :
    m_pPtr(_pPtr), m_uTargetGen(_uTargetGen), m_uCurrentGen(_uCurrentGen) {}
    ~CWeakPtr() { m_pPtr = nullptr; }

    inline bool IsValid() const { return m_uTargetGen ? *m_uTargetGen == m_uCurrentGen : false; }
    inline T* GetPtr() const { return m_pPtr; }

    inline T* operator->() { return m_pPtr; }
    inline const T* operator->() const { return m_pPtr; }

  private:
    T* m_pPtr = nullptr;
    size_t* m_uTargetGen = nullptr;
    size_t m_uCurrentGen = 0;
  };

  template<typename T, size_t MAX_ITEMS>
  class CUniquePtrList
  {
  private:
    struct TInternalData
    {
      std::unique_ptr<T> uPtr = nullptr;
      size_t tGeneration = 0;
    };

  public:
    CUniquePtrList() {}
    ~CUniquePtrList() { Clear(); }

    template<typename _Type = T, typename ...Args>
    inline CWeakPtr<T> Create(Args&&... args)
    {
      for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
      {
        TInternalData& pData = m_lstInternalData[tIndex];
        if (pData.uPtr)
        {
          continue;
        }

        // Create data 
        pData.uPtr = std::make_unique<_Type>(std::forward<Args>(args)...);
        pData.tGeneration++;
        m_tRegisteredItems++;

        return CWeakPtr<T>(pData.uPtr.get(), &pData.tGeneration, pData.tGeneration);
      }

      // Return null weak ptr
      return CWeakPtr<T>();
    }

    // Insert memory
    inline CWeakPtr<T> Insert(std::unique_ptr<T> _pMem)
    {
      for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
      {
        TInternalData& pData = m_lstInternalData[tIndex];
        if (pData.uPtr)
        {
          continue;
        }

        // Register data
        pData.uPtr = std::move(_pMem);
        pData.tGeneration++;
        m_tRegisteredItems++;

        return CWeakPtr<T>(pData.uPtr.get(), &pData.tGeneration, pData.tGeneration);
      }

      // Return invalid ptr
      return CWeakPtr<T>();
    }

    inline CWeakPtr<T> operator[](size_t _tIndex)
    {
      if (_tIndex > (MAX_ITEMS - 1))
      {
        return CWeakPtr<T>();
      }
      TInternalData& pData = m_lstInternalData[_tIndex];
      T* pPtr = pData.uPtr.get();
      return CWeakPtr<T>(pPtr, pPtr ? &pData.tGeneration : nullptr, pData.tGeneration);
    }

    inline const CWeakPtr<T> operator[](size_t _tIndex) const
    {
      if (_tIndex > (MAX_ITEMS - 1))
      {
        return CWeakPtr<T>();
      }
      const TInternalData& pData = m_lstInternalData[_tIndex];
      T* pPtr = pData.uPtr.get();
      return CWeakPtr<T>(pPtr, pPtr ? const_cast<size_t*>(&pData.tGeneration) : nullptr, pData.tGeneration);
    }

    bool Remove(const CWeakPtr<T>& _pItem_);
    void Clear();

    inline const size_t& GetSize() const { return m_tRegisteredItems; }
    inline const bool IsEmpty() const { return m_tRegisteredItems == 0; }
    inline size_t GetMaxSize() const { return MAX_ITEMS; }

  private:
    std::array<TInternalData, MAX_ITEMS> m_lstInternalData = std::array<TInternalData, MAX_ITEMS>();
    size_t m_tRegisteredItems = 0;
  };

  template<typename T, size_t MAX_ITEMS>
  void CUniquePtrList<T, MAX_ITEMS>::Clear()
  {
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
    {
      TInternalData& pData = m_lstInternalData[tIndex];
      pData.uPtr.reset();
      pData.tGeneration++;
    }
    m_tRegisteredItems = 0;
  }

  template<typename T, size_t MAX_ITEMS>
  bool CUniquePtrList<T, MAX_ITEMS>::Remove(const CWeakPtr<T>& _pItem_)
  {
    if (!_pItem_.IsValid())
    {
      return false;
    }

    bool bRemoved = false;
    for (size_t tIndex = 0; tIndex < MAX_ITEMS; ++tIndex)
    {
      TInternalData& pData = m_lstInternalData[tIndex];
      if (pData.uPtr.get() == _pItem_.GetPtr())
      {
        // Remove ptr
        pData.uPtr.reset();
        pData.tGeneration++;

        bRemoved = true;
        break;
      }
    }
    return bRemoved;
  }
}

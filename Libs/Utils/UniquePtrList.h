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
    CWeakPtr(T* _pPtr, uint32_t* _uTargetGen, uint32_t _uCurrentGen) :
    m_pPtr(_pPtr), m_uTargetGen(_uTargetGen), m_uCurrentGen(_uCurrentGen) {}
    ~CWeakPtr() { m_pPtr = nullptr; }

    inline bool IsValid() const { return m_uTargetGen ? *m_uTargetGen == m_uCurrentGen : false; }
    inline T* GetPtr() const { return m_pPtr; }

    inline T* operator->() { return m_pPtr; }
    inline const T* operator->() const { return m_pPtr; }

  private:
    T* m_pPtr = nullptr;
    uint32_t* m_uTargetGen = nullptr;
    uint32_t m_uCurrentGen = 0;
  };

  template<typename T, uint32_t MAX_ITEMS>
  class CUniquePtrList
  {
  private:
    struct TInternalData
    {
      std::unique_ptr<T> uPtr = nullptr;
      uint32_t uGeneration = 0;
    };

  public:
    CUniquePtrList() {}
    ~CUniquePtrList() { Clear(); }

    template<typename _Type = T, typename ...Args>
    inline CWeakPtr<T> Create(Args&&... args)
    {
      if (m_uRegisteredItems >= MAX_ITEMS)
      {
        return CWeakPtr<T>();
      }

      for (uint32_t uI = 0; uI < MAX_ITEMS; ++uI)
      {
        TInternalData& pData = m_lstInternalData[uI];
        if (!pData.uPtr)
        {
          continue;
        }

        // Create ptr 
        pData.uPtr = std::make_unique<_Type>(std::forward<Args>(args)...);
        pData.uGeneration++;

        m_uRegisteredItems++;
        return CWeakPtr<T>(pData.uPtr.get(), &pData.uGeneration, pData.uGeneration);
      }

      // Return null weak ptr
      return CWeakPtr<T>();
    }

    // Insert memory
    inline CWeakPtr<T> Insert(std::unique_ptr<T> _pMem)
    {
      if (m_uRegisteredItems >= MAX_ITEMS)
      {
        return CWeakPtr<T>();
      }

      for (uint32_t uI = 0; uI < MAX_ITEMS; ++uI)
      {
        TInternalData& pData = m_lstInternalData[uI];
        if (pData.uPtr)
          continue;

        // Register data
        pData.uPtr = std::move(_pMem);
        pData.uGeneration++;

        m_uRegisteredItems++;
        return CWeakPtr<T>(pData.uPtr.get(), &pData.uGeneration, pData.uGeneration);
      }

      // Return null weak ptr
      return CWeakPtr<T>();
    }

    inline CWeakPtr<T> operator[](uint32_t _uIndex)
    {
      if (_uIndex > (m_uRegisteredItems - 1))
      {
        return CWeakPtr<T>();
      }
      TInternalData& pData = m_lstInternalData[_uIndex];
      return CWeakPtr<T>(pData.uPtr.get(), &pData.uGeneration, pData.uGeneration);
    }
    inline const CWeakPtr<T> operator[](uint32_t _uIndex) const
    {
      if (_uIndex > (m_uRegisteredItems - 1))
      {
        return CWeakPtr<T>();
      }
      TInternalData& pData = m_lstInternalData[_uIndex];
      return CWeakPtr<T>(pData.uPtr.get(), &pData.uGeneration, pData.uGeneration);
    }

    bool Remove(CWeakPtr<T>& _pItem_);
    void Clear();

    inline const uint32_t& GetSize() const { return m_uRegisteredItems; }
    inline const bool IsEmpty() const { return m_uRegisteredItems == 0; }
    inline uint32_t GetMaxSize() const { return MAX_ITEMS; }

  private:
    std::array<TInternalData, MAX_ITEMS> m_lstInternalData = std::array<TInternalData, MAX_ITEMS>();
    uint32_t m_uRegisteredItems = 0;
  };

  template<typename T, uint32_t MAX_ITEMS>
  void CUniquePtrList<T, MAX_ITEMS>::Clear()
  {
    for (uint32_t uI = 0; uI < MAX_ITEMS; ++uI)
    {
      TInternalData& pData = m_lstInternalData[uI];
      pData.uPtr.reset();
      pData.uGeneration++;
    }
    m_uRegisteredItems = 0;
  }

  template<typename T, uint32_t MAX_ITEMS>
  bool CUniquePtrList<T, MAX_ITEMS>::Remove(CWeakPtr<T>& _pItem_)
  {
    if (!_pItem_.IsValid())
    {
      return false;
    }

    for (uint32_t uI = 0; uI < MAX_ITEMS; ++uI)
    {
      TInternalData& pData = m_lstInternalData[uI];
      if (pData.uPtr.get() == _pItem_.GetPtr())
      {
        // Destroy data
        pData.uPtr.reset();
        // Increase generation and decrease items
        pData.uGeneration++, m_uRegisteredItems--;
        return true;
      }
    }
    return false;
  }
}

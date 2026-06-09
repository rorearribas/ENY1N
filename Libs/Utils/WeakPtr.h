#pragma once

namespace utils
{
  template<typename T>
  class CWeakPtr
  {
  public:
    CWeakPtr() = default;
    ~CWeakPtr() { m_pPtr = nullptr; }

    template<typename U>
    CWeakPtr(U* _pPtr, size_t* _tTargetGen, size_t _tCurrentGen) :
      m_pPtr(_pPtr),
      m_uTargetGen(_tTargetGen),
      m_uCurrentGen(_tCurrentGen)
    {}

    template<typename U>
    CWeakPtr(const CWeakPtr<U>& _rOther) :
      m_pPtr(_rOther.GetPtr()),
      m_uTargetGen(_rOther.GetTargetGen()),
      m_uCurrentGen(_rOther.GetCurrentGen())
    {}

    inline bool IsValid() const { return m_uTargetGen ? *m_uTargetGen == m_uCurrentGen : false; }
    size_t* GetTargetGen() const { return m_uTargetGen; }
    size_t GetCurrentGen() const { return m_uCurrentGen; }

    inline T* GetPtr() const { return IsValid() ? m_pPtr : nullptr; }
    inline T* operator->() { return GetPtr(); }
    inline const T* operator->() const { return GetPtr(); }

  private:
    T* m_pPtr = nullptr;
    size_t* m_uTargetGen = nullptr;
    size_t m_uCurrentGen = 0;
  };

  template<typename T>
  class CUniquePtr
  {
  public:
    CUniquePtr() = default;
    template<typename ...Args>

    CUniquePtr(Args&&... args)
    {
      Make_Unique(std::forward<Args>(args)...);
    }
    ~CUniquePtr() { Release(); }

    CUniquePtr(CUniquePtr&& _rOther) noexcept
    {
      m_pPtr = std::move(_rOther.m_pPtr);
      m_tGeneration = _rOther.m_tGeneration;
    }

    inline CUniquePtr& operator=(CUniquePtr&& _rOther) noexcept
    {
      if (this != &_rOther)
      {
        Release();
        m_pPtr = std::move(_rOther.m_pPtr);
        m_tGeneration = _rOther.m_tGeneration;
      }
      return *this;
    }

    CUniquePtr(const CUniquePtr& _rOther) = delete;
    CUniquePtr& operator=(const CUniquePtr& _rOther) = delete;

    template<typename ...Args>
    inline void Make_Unique(Args&&... args)
    {
      Release();
      m_pPtr = std::make_unique<T>(std::forward<Args>(args)...);
    }

    inline operator bool() const { return m_pPtr != nullptr; }
    inline T* operator->() { return m_pPtr.get(); }
    inline const T* operator->() const { return m_pPtr.get(); }

    inline void Release() { if (m_pPtr) { m_tGeneration++; m_pPtr.reset(); } }
    inline CWeakPtr<T> GetRef() { return m_pPtr ? CWeakPtr<T>(m_pPtr.get(), &m_tGeneration, m_tGeneration) : CWeakPtr<T>(); }

  private:
    std::unique_ptr<T> m_pPtr;
    size_t m_tGeneration = 1;
  };
}

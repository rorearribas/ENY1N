#pragma once
#include "Libs/Macros/GlobalMacros.h"
#include <mutex>

namespace utils
{
  template <typename T, bool Lazy = false>
  class CSingleton
  {
  public:
    static T* CreateSingleton();
    static T* GetInstance();

    static void DestroySingleton();
    inline static bool HasSingleton() { return m_pInstance; }

  protected:
    CSingleton() {}
    CSingleton(const CSingleton&) = delete;
    CSingleton& operator=(const CSingleton&) = delete;
    ~CSingleton() {}

  private:
    inline static T* m_pInstance = nullptr;
    inline static std::mutex m_mutex;
  };

  template <typename T, bool Lazy>
  T* CSingleton<T, Lazy>::GetInstance()
  {
    return (Lazy && !m_pInstance) ? CreateSingleton() : m_pInstance;
  }

  template <typename T, bool Lazy>
  T* CSingleton<T, Lazy>::CreateSingleton()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_pInstance)
    {
      m_pInstance = new T();
      SUCCESS_LOG("Singleton created! -> " << typeid(T).name());
    }
    return m_pInstance;
  }

  template <typename T, bool Lazy>
  void CSingleton<T, Lazy>::DestroySingleton()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_pInstance)
    {
      delete m_pInstance;
      m_pInstance = nullptr;
    }
  }
}
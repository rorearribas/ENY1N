#pragma once
#include "Engine/Global/GlobalResources.h"
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
    static bool HasSingleton() { return m_pInstance; }

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
  T* utils::CSingleton<T, Lazy>::GetInstance()
  {
    return Lazy ? CreateSingleton() : m_pInstance;
  }

  template <typename T, bool Lazy>
  T* utils::CSingleton<T, Lazy>::CreateSingleton()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_pInstance)
    {
      m_pInstance = new T();
      //LOG("Singleton created! -> " << typeid(T).name());
    }
    return m_pInstance;
  }

  template <typename T, bool Lazy>
  void utils::CSingleton<T, Lazy>::DestroySingleton()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    global::ReleaseObject(m_pInstance);
  }
}
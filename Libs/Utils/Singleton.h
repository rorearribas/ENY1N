#pragma once
#include <mutex>

namespace utils
{
  template <typename T>
  class CSingleton
  {
  public:
    static T* CreateSingleton();
    static void DestroySingleton();
    static T* GetInstance();

  protected:
    CSingleton() {}
    CSingleton(const CSingleton&) = delete;
    CSingleton& operator=(const CSingleton&) = delete;
    ~CSingleton() {}

  private:
    inline static T* m_pInstance = nullptr;
    inline static std::mutex m_mutex;
  };

  template <typename T>
  T* utils::CSingleton<T>::GetInstance()
  {
    return m_pInstance ? m_pInstance : CreateSingleton();
  }

  template <typename T>
  T* utils::CSingleton<T>::CreateSingleton()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_pInstance)
    {
      m_pInstance = new T();
    }
    return m_pInstance;
  }

  template <typename T>
  void utils::CSingleton<T>::DestroySingleton()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_pInstance)
    {
      delete m_pInstance;
      m_pInstance = nullptr;
    }
  }
}
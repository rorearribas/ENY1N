#pragma once

namespace utils
{
  template <typename T>
  class CSingleton
  {
  public:
    static T* GetInstance()
    {
      static T* m_pInstance = new T();
      return m_pInstance;
    }
  protected:
    CSingleton() {}
    ~CSingleton() {}
  };
}
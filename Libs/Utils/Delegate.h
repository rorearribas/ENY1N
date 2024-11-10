#pragma once
#include <functional>
#include <utility>
#include <type_traits>

namespace utils
{
  template<class...Args>
  class CDelegate;

  template <typename RETURN_TYPE, typename... Args>
  class CDelegate<RETURN_TYPE(Args...)>
  {
  public:
    CDelegate() {};
    template<typename Object>
    CDelegate(Object* objectPtr, RETURN_TYPE(Object::* Method)(Args...))
    {
      Bind(objectPtr, Method);
    }

    inline RETURN_TYPE Execute(Args... args) const
    {
      if (m_oFunction)
      {
        if constexpr (!std::is_void_v<RETURN_TYPE>)
        {
          return m_oFunction(std::forward<Args>(args)...);
        }
        else
        {
          m_oFunction(std::forward<Args>(args)...);
        }
      }
    }

    template<typename Object>
    inline void Bind(RETURN_TYPE(Object::* Method)(Args...), Object* _pObject)
    {
      m_oFunction = [_pObject, Method](Args... args) -> RETURN_TYPE 
      {
        return (_pObject->*Method)(std::forward<Args>(args)...);
      };
    }
    inline void Bind(RETURN_TYPE(*func)(Args...))
    {
      m_oFunction = func;
    }

    inline const bool IsValid() const { return static_cast<bool>(m_oFunction); }
    inline void Clear()
    {
      m_oFunction = nullptr;
    }

  private:
    std::function<RETURN_TYPE(Args...)> m_oFunction;
  };
}

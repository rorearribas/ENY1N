#pragma once
#include <functional>
#include <type_traits>

namespace reflection
{
  template<class...Args> class CFunction;
  template <class Object, typename RETURN_TYPE, typename... Args>
  class CFunction<RETURN_TYPE(Object::*)(Args...)>;

  // Declare function base
  class CFunctionBase
  {
  public:
    CFunctionBase(void* pInternalPtr, void* /*_pPtrClass*/, const char* _sFunctionName)
      : m_pInternalFunction(pInternalPtr), m_sFunctionName(_sFunctionName) {
    }
    virtual ~CFunctionBase() = default;

    template <class Object, typename RETURN_TYPE, typename... Args>
    RETURN_TYPE Execute(Args&&... args)
    {
      auto* pFunction = (CFunction<RETURN_TYPE(Object::*)(Args...)>*)(m_pInternalFunction);
      if (pFunction)
      {
        if constexpr (!std::is_void_v<RETURN_TYPE>)
        {
          return pFunction->Execute(std::forward<Args>(args)...);
        }
        else
        {
          pFunction->Execute(std::forward<Args>(args)...);
        }
      }
    }
  protected:
    const char* m_sFunctionName = nullptr;
    void* m_pInternalFunction = nullptr;
  };

  // Function
  template<class...Args> class CFunction;
  template <class Object, typename RETURN_TYPE, typename... Args>
  class CFunction<RETURN_TYPE(Object::*)(Args...)> : public CFunctionBase
  {
  public:
    // Constructor
    typedef RETURN_TYPE(Object::* FunctionPointer)(Args...);
    CFunction(Object* _ObjectPtr, const char* _sFunctionName) :
      m_pObjectPtr(_ObjectPtr), m_pMemberFunction(nullptr),
      CFunctionBase(this, _ObjectPtr, _sFunctionName) {
    }
    // Bind function
    void Bind(FunctionPointer _pMethod)
    {
      m_pMemberFunction = _pMethod;
    }
    // Callback
    RETURN_TYPE Execute(Args... args)
    {
      if constexpr (!std::is_void_v<RETURN_TYPE>)
      {
        return (m_pObjectPtr->*m_pMemberFunction)(std::forward<Args>(args)...);
      }
      else
      {
        (m_pObjectPtr->*m_pMemberFunction)(std::forward<Args>(args)...);
      }
    }
  private:
    FunctionPointer m_pMemberFunction = nullptr; // Member function
    Object* m_pObjectPtr = nullptr; // Object ptr
  };
}

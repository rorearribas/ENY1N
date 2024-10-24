#pragma once
#include <functional>
#include <type_traits>

// Pre-declaramos la clase template
template<class...Args> class CFunction;
template <class Object, typename RETURN_TYPE, typename... Args>
class CFunction<RETURN_TYPE(Object::*)(Args...)>;

// Declaramos la function base
class CFunctionBase
{
public:
  CFunctionBase(void* pInternalPtr, void* _pPtrClass, const char* _sFunctionName)
  : m_pInternalFunction(pInternalPtr), m_sFunctionName(_sFunctionName) {}
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

// Clase variadic para crear delegados
template<class...Args> class CFunction;
template <class Object, typename RETURN_TYPE, typename... Args>
class CFunction<RETURN_TYPE(Object::*)(Args...)> : public CFunctionBase
{
public:
  // Typedef para puntero a función miembro
  typedef RETURN_TYPE(Object::* FunctionPointer)(Args...);

  // Constructor que recibe un puntero al objeto y el nombre de la función
  CFunction(Object* _ObjectPtr, const char* _sFunctionName) : 
  m_pObjectPtr(_ObjectPtr), m_pMemberFunction(nullptr), 
  CFunctionBase(this, _ObjectPtr, _sFunctionName) {}

  // Método para enlazar un método miembro
  void Bind(FunctionPointer _oMethodPtr)
  {
    m_pMemberFunction = _oMethodPtr; // Guardar el puntero a la función miembro
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
  FunctionPointer m_pMemberFunction = nullptr; // Puntero a función miembro
  Object* m_pObjectPtr = nullptr; // Puntero al objeto
};
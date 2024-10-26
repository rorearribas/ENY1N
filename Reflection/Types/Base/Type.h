#pragma once
#include <array>
#include <vector>
#include "Reflection/Types/Functions/Functions.h"
#include "Reflection/Types/Property/Property.h"

// CType
class CType 
{
public:
  template<typename _Class = void>
  explicit CType(_Class* _pClassPtr, const char* _sClassName) : m_pPtr(_pClassPtr)
  {
    m_sTypeName = _sClassName;
    RegisterType(this);
  }

  const std::vector<CProperty*>& GetPropertyList() const { return m_vctProperties; }
  const std::vector<CFunctionBase*>& GetFunctionList() const { return m_vctFunctions; }

  const char* GetTypeName() const { return m_sTypeName; }
  template<typename _Type>
  const _Type* TryGetValue() const { return (_Type*)(m_pPtr); }

protected:
  template<class Object, class RETURN_TYPE, class... Args>
  inline void RegisterFunction(CFunction<RETURN_TYPE(Object::*)(Args...)>& _oFunction) 
  {
    m_vctFunctions.push_back(&_oFunction); 
  }
  void RegisterProperty(CProperty* _pProperty);
  void RegisterType(CType* _pType);

  void* m_pPtr = nullptr;
  const char* m_sTypeName = nullptr;

  // Reflection
  std::vector<CProperty*> m_vctProperties = {};
  std::vector<CFunctionBase*> m_vctFunctions = {};
};

#define DECLARE_CLASS_IN_REFLECTION(TYPE) \
struct CTypeReflection : public CType \
{ \
  CTypeReflection(); \
}; \
public: \
 const CType* GetType() { return &m_oType; } \
private: \
CTypeReflection m_oType;

#define IMPLEMENT_CLASS_BEGIN(TYPE) \
TYPE::CTypeReflection::CTypeReflection() : CType(this, #TYPE) { \
TYPE* pType = (TYPE*)(m_pPtr); 

#define ADD_PROPERTY(PROPERTY) \
CProperty* PROPERTY = new CProperty(&pType->PROPERTY, #PROPERTY); \
RegisterProperty(PROPERTY);

#define ADD_FUNCTION(FUNCTION, BASE, OUTPUT, ...) \
CFunction<OUTPUT(BASE::*)(__VA_ARGS__)> Register_##FUNCTION(pType, #FUNCTION); \
Register_##FUNCTION.Bind(&BASE::FUNCTION); \
RegisterFunction<BASE, OUTPUT, __VA_ARGS__>(Register_##FUNCTION);

#define IMPLEMENT_CLASS_END() } 


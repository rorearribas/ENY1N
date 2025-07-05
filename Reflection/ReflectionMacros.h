#pragma once
#include "Reflection/Base/Class.h"
#include "Reflection/Variable/Variable.h"
#include "Reflection/Function/Function.h"

#define DECLARE_CLASS_IN_REFLECTION \
struct CStaticClass : public reflection::CClass \
{ \
  CStaticClass(); \
}; \
public: \
 const reflection::CClass* GetClass() { return &m_oStaticClass; } \
private: \
CStaticClass m_oStaticClass; \

#define IMPLEMENT_CLASS_BEGIN(CLASS) \
CLASS::CStaticClass::CStaticClass() : reflection::CClass(this, #CLASS) { \
CLASS* pType = (CLASS*)(m_pPtr); 

#define ADD_VARIABLE(VAR) \
reflection::CVariable* pVariable = new reflection::CVariable(&pType->VAR, #VAR); \
RegisterVariable(pVariable);

#define ADD_FUNCTION(FUNCTION, BASE, OUTPUT, ...) \
reflection::CFunction<OUTPUT(BASE::*)(__VA_ARGS__)> Register_##FUNCTION(pType, #FUNCTION); \
Register_##FUNCTION.Bind(&BASE::FUNCTION); \
RegisterFunction<BASE, OUTPUT, __VA_ARGS__>(Register_##FUNCTION);

#define IMPLEMENT_CLASS_END } 
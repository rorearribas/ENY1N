#pragma once
#include "Reflection/Base/Type.h"
#include "Reflection/Variable/Variable.h"
#include "Reflection/Function/Function.h"

#define DECLARE_CLASS_IN_REFLECTION \
struct CStaticType : public reflection::CType \
{ \
  CStaticType(); \
}; \
public: \
 static const reflection::CType* GetStaticType() { return &m_oStaticType; } \
private: \
static CStaticType m_oStaticType; \

#define IMPLEMENT_CLASS_BEGIN(CLASS) \
CLASS::CStaticType CLASS::m_oStaticType;\
CLASS::CStaticType::CStaticType() : reflection::CType(this, #CLASS) { \
CLASS* pType = reinterpret_cast<CLASS*>(m_pPtr); 

#define ADD_VARIABLE(VAR) \
{ \
using Class = std::remove_reference_t<decltype(*pType)>; \
reflection::CVariable* pVariable = new reflection::CVariable(offsetof(Class, VAR), #VAR); \
RegisterVariable(pVariable); \
} \

#define ADD_FUNCTION(FUNCTION, BASE, OUTPUT, ...) \
reflection::CFunction<OUTPUT(BASE::*)(__VA_ARGS__)> Register_##FUNCTION(pType, #FUNCTION); \
Register_##FUNCTION.Bind(&BASE::FUNCTION); \
RegisterFunction<BASE, OUTPUT, __VA_ARGS__>(Register_##FUNCTION);

#define IMPLEMENT_CLASS_END } 
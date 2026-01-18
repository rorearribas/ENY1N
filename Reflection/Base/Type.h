#pragma once
#include "Reflection/TypeManager.h"
#include "Reflection/Function/Function.h"
#include "Reflection/Variable/Variable.h"
#include "Libs/Macros/GlobalMacros.h"

namespace reflection
{

  // CType
  class CType
  {
  public:
    typedef std::vector<CVariable*> TVariables;
    typedef std::vector<CFunctionBase*> TFunctions;

  public:
    template<typename TYPE = void>
    explicit CType(TYPE* _pType, const char* _sTypeId)
      : m_pPtr(_pType), m_sType(_sTypeId)
    {
      reflection::CTypeManager::GetInstance()->RegisterType(this);
    }

    inline const char* GetTypeName() const { return m_sType; }
    inline const TVariables& GetVariables() const { return m_lstVariables; }
    inline const TFunctions& GetFunctions() const { return m_lstFunctions; }

  protected:
    template<class Object, class RETURN_TYPE, class... Args>
    void RegisterFunction(CFunction<RETURN_TYPE(Object::*)(Args...)>& _oFunction);
    void RegisterVariable(CVariable* _pVariable);

    const char* m_sType = nullptr;
    void* m_pPtr = nullptr;

  private:
    // Reflection
    TVariables m_lstVariables = TVariables();
    TFunctions m_lstFunctions = TFunctions();
  };

  template<class Object, class RETURN_TYPE, class... Args>
  void reflection::CType::RegisterFunction(CFunction<RETURN_TYPE(Object::*)(Args...)>& _oFunction)
  {
    m_lstFunctions.emplace_back(&_oFunction);
  }
}
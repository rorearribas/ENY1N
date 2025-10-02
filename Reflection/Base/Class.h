#pragma once
#include "Reflection/TypeManager.h"
#include "Reflection/Function/Function.h"
#include "Reflection/Variable/Variable.h"
#include "Libs/Macros/GlobalMacros.h"

namespace reflection
{
  // CType
  class CClass
  {
  public:
    template<typename _Class = void>
    explicit CClass(_Class* _pClassPtr, const char* _sClassName)
      : m_pPtr(_pClassPtr), m_sClassId(_sClassName)
    {
      reflection::CTypeManager::GetInstance()->RegisterClass(this);
    }

    inline const std::vector<CVariable*>& GetVariables() const { return m_vctVariables; }
    inline const std::vector<CFunctionBase*>& GetFunctions() const { return m_vctFunctions; }

    inline const char* GetClassId() const { return m_sClassId; }
    template<typename _Type>
    inline const _Type* TryGetValue() const { return reinterpret_cast<_Type*>(m_pPtr); }

  protected:
    template<class Object, class RETURN_TYPE, class... Args>
    inline void RegisterFunction(CFunction<RETURN_TYPE(Object::*)(Args...)>& _oFunction)
    {
      m_vctFunctions.emplace_back(&_oFunction);
    }
    inline void RegisterVariable(CVariable* _pVariable)
    {
      auto it = std::find(m_vctVariables.begin(), m_vctVariables.end(), _pVariable);
      if (it == m_vctVariables.end())
      {
        m_vctVariables.emplace_back(_pVariable);
      }
    }

    void* m_pPtr = nullptr;
    const char* m_sClassId = nullptr;

    // Reflection
    std::vector<CVariable*> m_vctVariables = {};
    std::vector<CFunctionBase*> m_vctFunctions = {};
  };
}
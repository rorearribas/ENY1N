#include "TypeManager.h"
#include "Reflection/Base/Type.h"

namespace reflection
{
  void CTypeManager::RegisterType(CType* _pType)
  {
    if (m_uRegisteredTypes == m_lstTypes.max_size())
    {
      std::cout << "Increase the size!" << std::endl;
      return;
    }
    m_lstTypes[m_uRegisteredTypes] = _pType;
    m_uRegisteredTypes++;
  }
  // ------------------------------------
  CType* const CTypeManager::FindType(const char* _sTypeName)
  {
    for (unsigned int uIndex = 0; uIndex < m_lstTypes.size(); uIndex++)
    {
      CType* pType = m_lstTypes[uIndex];
      if (pType->GetTypeName() == _sTypeName)
      {
        return pType;
      }
    }
    return nullptr;
  }
  // ------------------------------------
  void CTypeManager::ClearTypes()
  {
    uint32_t uIndex = 0;
    {
      m_lstTypes[uIndex] = nullptr;
      uIndex++;
    }
    m_uRegisteredTypes = 0;
  }
}

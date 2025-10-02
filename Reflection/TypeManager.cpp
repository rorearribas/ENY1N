#include "TypeManager.h"
#include "Reflection/Base/Class.h"

namespace reflection
{
  void CTypeManager::RegisterClass(CClass* _pType)
  {
    if (m_iRegisteredTypes == m_vctTypes.max_size())
    {
      std::cout << "Increase the size!" << std::endl;
      return;
    }
    m_vctTypes[m_iRegisteredTypes] = _pType;
    m_iRegisteredTypes++;
  }
  // ------------------------------------
  CClass* const CTypeManager::FindClass(const char* _sTypeName)
  {
    for (unsigned int uIndex = 0; uIndex < m_vctTypes.size(); uIndex++)
    {
      CClass* const pType = m_vctTypes[uIndex];
      if (pType && pType->GetClassId() == _sTypeName)
      {
        return pType;
      }
    }
    return nullptr;
  }
  // ------------------------------------
  void CTypeManager::ClearAllTypes()
  {
    std::fill(std::begin(m_vctTypes), std::end(m_vctTypes), nullptr);
  }
}

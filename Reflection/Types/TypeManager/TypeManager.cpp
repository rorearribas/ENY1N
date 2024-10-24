#include "TypeManager.h"
#include "../Base/Type.h"

void CTypeManager::RegisterType(CType* _pType)
{
  if (m_iRegisteredTypesCount == m_vctTypeList.max_size()) return;
  m_vctTypeList[m_iRegisteredTypesCount] = _pType;
  m_iRegisteredTypesCount++;
}

CType* CTypeManager::FindType(const char* _sTypeName)
{
  for (unsigned int uIndex = 0; uIndex < m_vctTypeList.size(); uIndex++)
  {
    CType* pType = m_vctTypeList[uIndex];
    if (pType && pType->GetTypeName() == _sTypeName) { return pType; }
  }
  return nullptr;
}
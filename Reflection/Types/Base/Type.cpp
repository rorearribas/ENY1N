#include "Type.h"
#include "../TypeManager/TypeManager.h"

void CType::RegisterType(CType* _pType)
{
  CTypeManager& oTypeManager = CTypeManager::GetInstance();
  oTypeManager.RegisterType(_pType);
}

void CType::RegisterProperty(CProperty* _pProperty)
{
  auto it = std::find(m_vctProperties.begin(), m_vctProperties.end(), _pProperty);
  if (it == m_vctProperties.end()) { m_vctProperties.push_back(_pProperty); }
}
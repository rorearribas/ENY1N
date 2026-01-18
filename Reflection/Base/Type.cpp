#include "Type.h"
#include "Reflection/TypeManager.h"
#include "Libs/Macros/GlobalMacros.h"

namespace reflection
{
  void CType::RegisterVariable(CVariable* _pVariable)
  {
    auto it = std::find(m_lstVariables.begin(), m_lstVariables.end(), _pVariable);
    if (it == m_lstVariables.end())
    {
      m_lstVariables.emplace_back(_pVariable);
    }
  }
}

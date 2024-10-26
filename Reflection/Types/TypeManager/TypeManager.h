#pragma once
#include <array>
#include "Libs/Utils/Singleton.h"

class CType;
class CTypeManager : public utils::CSingleton<CTypeManager>
{
public:
  CTypeManager() {}
  ~CTypeManager() { ClearAllTypes(); }

  void RegisterType(CType* _pType);
  CType* FindType(const char* _sTypeName);

private:
  inline void ClearAllTypes() { std::fill(std::begin(m_vctTypeList), std::end(m_vctTypeList), nullptr); }
  std::array<CType*, 1000> m_vctTypeList = {};
  int m_iRegisteredTypesCount = 0;
};

#pragma once
#include <array>

class CType;
class CTypeManager 
{
public:
  static CTypeManager& GetInstance()
  {
    static CTypeManager m_oInstance;
    return m_oInstance;
  }

  CTypeManager(const CTypeManager&) = delete;
  CTypeManager& operator=(const CTypeManager&) = delete;

  void RegisterType(CType* _pType);
  CType* FindType(const char* _sTypeName);

private:
  CTypeManager() {}
  ~CTypeManager() { ClearAllTypes(); }

  inline void ClearAllTypes() { std::fill(std::begin(m_vctTypeList), std::end(m_vctTypeList), nullptr); }

  std::array<CType*, 1000> m_vctTypeList = {};
  int m_iRegisteredTypesCount = 0;
};

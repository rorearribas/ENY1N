#pragma once
#include <array>
#include "Libs/Utils/Singleton.h"
#include "Libs/Utils/FixedPool.h"

namespace reflection { class CClass; }
namespace reflection
{
  class CTypeManager : public utils::CSingleton<CTypeManager>
  {
  private:
    static constexpr int s_iMaxTypes = 1000;
    typedef std::array<CClass*, s_iMaxTypes> TTypes;

  public:
    CTypeManager() {}
    ~CTypeManager() { ClearAllTypes(); }

    void RegisterClass(CClass*);
    CClass* const FindClass(const char* _sType);

  private:
    void ClearAllTypes();

  private:
    TTypes m_vctTypes = TTypes();
    int m_iRegisteredTypes = 0;
  };
}


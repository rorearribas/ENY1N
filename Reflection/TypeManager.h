#pragma once
#include "Libs/Utils/Singleton.h"
#include <array>

namespace reflection { class CType; }
namespace reflection
{
  class CTypeManager : public utils::CSingleton<CTypeManager, true>
  {
  private:
    static constexpr int s_iMaxTypes = 1000;
    typedef std::array<reflection::CType*, s_iMaxTypes> TTypes;

  public:
    CTypeManager() {}
    ~CTypeManager() { ClearTypes(); }

    void RegisterType(CType* _pType);
    CType* const FindType(const char* _sType);

  private:
    void ClearTypes();

  private:
    TTypes m_lstTypes = TTypes();
    uint32_t m_uRegisteredTypes = 0;
  };
}


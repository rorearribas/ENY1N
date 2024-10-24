#include "Entity.h"
#include <iostream>

IMPLEMENT_CLASS_BEGIN(CEntity)
  ADD_PROPERTY(m_fLife)
  ADD_PROPERTY(m_sEntityName)
  ADD_FUNCTION(Callback, CEntity, void)
  ADD_FUNCTION(Callback2, CEntity, void, const char*)
IMPLEMENT_CLASS_END()

void CEntity::Callback()
{
  std::cout << "Callback1" << std::endl;
}

void CEntity::Callback2(const char* _sCallback)
{
  std::cout << _sCallback << std::endl;
}

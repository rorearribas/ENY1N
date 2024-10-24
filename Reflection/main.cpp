#include <iostream>
#include <Windows.h>
#include <vector>
#include <cmath>

#include "Types/Base/Type.h"
#include "Types/Game/Entity.h"
#include "Types/Functions/Functions.h"

int main()
{
  CEntity* pEntity = new CEntity("Ruben");
  const std::vector<CFunctionBase*>& oFunctionList = pEntity->GetType()->GetFunctionList();
  oFunctionList[0]->Execute<CEntity, void>();
  oFunctionList[1]->Execute<CEntity, void, const char*>("Hello World");

  return 0;
}
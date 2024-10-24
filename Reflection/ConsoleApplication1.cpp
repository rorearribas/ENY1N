#include <iostream>
#include <Windows.h>
#include <vector>
#include <cmath>
#include "Type.h"
#include "Entity.h"
#include "Functions.h"

int main()
{
  CEntity* pEntity = new CEntity("Ruben");
  const std::vector<CFunctionBase*>& oFunctionList = pEntity->GetType()->GetFunctionList();
  oFunctionList[0]->Execute<CEntity, void>();
  oFunctionList[1]->Execute<CEntity, void, const char*>("Hello World");

  return 0;
}
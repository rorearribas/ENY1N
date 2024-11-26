#pragma once
#include "Libs/Utils/Singleton.h"

class CResourceManager : public utils::CSingleton<CResourceManager>
{
public:

  [[nodiscard]]char* LoadResource(const char* _sPath);


private:
};


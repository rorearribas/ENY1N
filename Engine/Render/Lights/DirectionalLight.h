#pragma once
#include "Libs/Maths/Transform.h"
#include "Light.h"

namespace render
{
  namespace lights
  {
    class CDirectionalLight : public CLight
    {
    public:
      CDirectionalLight();
      virtual ~CDirectionalLight() {}

    private:
      void InitDefaultLight();
    };
  }
}



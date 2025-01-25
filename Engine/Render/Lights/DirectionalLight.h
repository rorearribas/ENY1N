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
      CDirectionalLight() : CLight(std::string()) {}
      CDirectionalLight(const std::string& _sLightName) : CLight(_sLightName) {}
      virtual ~CDirectionalLight() {}
    };
  }
}



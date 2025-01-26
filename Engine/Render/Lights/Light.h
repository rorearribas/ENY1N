#pragma once
#include <string>
#include "Libs/Maths/Transform.h"

namespace render
{
  namespace lights
  {
    enum ELightType { DIRECTIONAL_LIGHT, POINT_LIGHT, SPOT_LIGHT };

    // Base class
    class CLight
    {
    public:
      CLight(ELightType _eLightType) : m_eLightType(_eLightType) {}
      virtual ~CLight() {}

      virtual void UpdateLight() {} 

    private:
      maths::CTransform m_oTransform;
      ELightType m_eLightType = DIRECTIONAL_LIGHT;
    };
  }
}



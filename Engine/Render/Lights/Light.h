#pragma once
#include <string>
#include <cassert>
#include "Libs/Maths/Transform.h"

namespace render
{
  namespace lights
  {
    enum ELightType { DIRECTIONAL_LIGHT, POINT_LIGHT, SPOT_LIGHT, INVALID };

    class CLight {
    public:
      CLight(ELightType _eLightType) : m_eLightType(_eLightType) { assert(m_eLightType != INVALID); }
      virtual ~CLight() {}

      virtual void UpdateLight() {} 
      const ELightType& GetLightType() const { return m_eLightType; }
      const maths::CTransform& GetTransform() const { return m_oTransform; }

    private:
      maths::CTransform m_oTransform;
      ELightType m_eLightType = INVALID;
    };
  }
}



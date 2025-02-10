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

      virtual void UpdateLight () {}

      const ELightType& GetLightType() const { return m_eLightType; }
      void SetPosition(const maths::CVector3& _v3Position) { m_oLightTransform.SetPosition(_v3Position); }
      const maths::CVector3& GetPosition() const { return m_oLightTransform.GetPosition(); }
      void SetRotation(const maths::CVector3& _v3Rot) { m_oLightTransform.SetRotation(_v3Rot); }
      const maths::CVector3& GetRotation() const { return m_oLightTransform.GetRotation(); }

    private:
      maths::CTransform m_oLightTransform;
      ELightType m_eLightType = INVALID;
    };
  }
}



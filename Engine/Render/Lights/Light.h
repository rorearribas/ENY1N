#pragma once
#include <string>
#include <cassert>
#include "Libs/Math/Transform.h"

namespace render
{
  namespace lights
  {
    enum ELightType
    {
      DIRECTIONAL_LIGHT,
      POINT_LIGHT,
      SPOT_LIGHT,
      INVALID
    };

    class CBaseLight
    {
    public:
      CBaseLight(ELightType _eLightType) : m_eLightType(_eLightType) { assert(m_eLightType != INVALID); }
      virtual ~CBaseLight() {}

      const ELightType& GetLightType() const { return m_eLightType; }
      void SetPosition(const math::CVector3& _v3Position) { m_oLightTransform.SetPosition(_v3Position); }
      const math::CVector3& GetPosition() const { return m_oLightTransform.GetPosition(); }
      void SetRotation(const math::CVector3& _v3Rot) { m_oLightTransform.SetRotation(_v3Rot); }
      const math::CVector3& GetRotation() const { return m_oLightTransform.GetRotation(); }

    private:
      math::CTransform m_oLightTransform;
      ELightType m_eLightType = INVALID;
    };
  }
}



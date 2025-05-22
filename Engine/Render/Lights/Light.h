#pragma once
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
      CBaseLight(ELightType _eLightType) : m_eLightType(_eLightType) {}
      virtual ~CBaseLight() {}

      const ELightType& GetLightType() const { return m_eLightType; }
      void SetPosition(const math::CVector3& _v3Pos) { m_v3Position = _v3Pos; }
      const math::CVector3& GetPosition() const { return m_v3Position; }
      void SetDirection(const math::CVector3& _v3Dir);
      const math::CVector3& GetDirection() const { return m_v3Direction; }
      void SetColor(const math::CVector3& _v3Color);
      const math::CVector3& GetColor() const { return m_v3Color; }

    private:
      // Type
      ELightType m_eLightType = INVALID;

      // Properties
      math::CVector3 m_v3Position = math::CVector3::Zero;
      math::CVector3 m_v3Direction = math::CVector3::Zero;
      math::CVector3 m_v3Color = math::CVector3::One;
    };
  }
}



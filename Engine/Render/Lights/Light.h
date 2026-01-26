#pragma once
#include "Libs/Math/Transform.h"
#include "Engine/Render/RenderTypes.h"

namespace render
{
  namespace lights
  {
    class CLight
    {
    public:
      CLight(ELight _eLightType) : m_eLightType(_eLightType) {}
      virtual ~CLight() {}

      inline const ELight& GetLightType() const { return m_eLightType; }
      void SetPos(const math::CVector3& _v3Pos) { m_v3Pos = _v3Pos; }
      inline const math::CVector3& GetPos() const { return m_v3Pos; }
      void SetDir(const math::CVector3& _v3Dir);
      inline const math::CVector3& GetDir() const { return m_v3Dir; }
      void SetColor(const math::CVector3& _v3Color);
      inline const math::CVector3& GetColor() const { return m_v3Color; }

    private:
      // Properties
      math::CVector3 m_v3Pos = math::CVector3::Zero;
      math::CVector3 m_v3Dir = math::CVector3::Zero;
      math::CVector3 m_v3Color = math::CVector3::One;

      // Type
      ELight m_eLightType = ELight::INVALID;
    };
  }
}



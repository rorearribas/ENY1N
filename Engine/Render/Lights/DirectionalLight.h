#pragma once
#include "Light.h"
#include "Libs/Math/Transform.h"

namespace render
{
  namespace lights
  {
    class CDirectionalLight : public CLight
    {
    public:
      static math::CVector3 s_vDefaultDirectionalColor;
      static math::CVector3 s_vDefaultDirection;

    public:
      CDirectionalLight() : CLight(ELightType::DIRECTIONAL_LIGHT) { SetDirection(s_vDefaultDirection); }
      virtual ~CDirectionalLight() {}

      // Color
      void SetColor(const math::CVector3& _v3Color) { m_v3LightColor = _v3Color; }
      const math::CVector3& GetColor() const { return m_v3LightColor; }
      // Intensity
      void SetIntensity(float _fIntensity) { m_fIntensity = _fIntensity; }
      const float& GetIntensity() const { return m_fIntensity; }

    private:
      // Properties
      math::CVector3 m_v3LightColor = s_vDefaultDirectionalColor;
      float m_fIntensity = 1.0f;
    };
  }
}



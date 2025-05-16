#pragma once
#include "Light.h"
#include "../ConstantBuffer/ConstantBuffer.h"

namespace render
{
  namespace lights
  {
    class CPointLight : public CLight {
    public:
      CPointLight() : CLight(POINT_LIGHT) {}
      virtual ~CPointLight() {}

      // Color
      void SetColor(const math::CVector3& _v3Color) { m_v3LightColor = _v3Color; }
      const math::CVector3& GetColor() const { return m_v3LightColor; }
      // Range
      void SetRange(float _fRange) { m_fRange = _fRange; }
      const float& GetRange() const { return m_fRange; }
      // Intensity
      void SetIntensity(float _fIntensity) { m_fIntensity = _fIntensity; }
      const float& GetIntensity() const { return m_fIntensity; }

    private:
      math::CVector3 m_v3LightColor = math::CVector3::One;
      float m_fRange = 10.0f;
      float m_fIntensity = 1.0f;
    };
  }
}


#pragma once
#include "Light.h"

namespace render
{
  namespace lights
  {
    class CPointLight : public CLight
    { 
    public:
      CPointLight() : CLight() {}
      virtual ~CPointLight() {}

      // Color
      void SetColor(const maths::CVector3& _v3Color) { m_v3LightColor = _v3Color; }
      const maths::CVector3& GetColor() { return m_v3LightColor; }
      // Attenuation
      void SetAttenuation(float _fAttenuation) { m_fAttenuation = _fAttenuation; }
      const float& GetAttenuation() { return m_fAttenuation; }
      // Range
      void SetRange(float _fRange) { m_fRange = _fRange;}
      const float& GetRange() { return m_fRange; }
      // Intensity
      void SetIntensity(float _fIntensity) { m_fIntensity = _fIntensity; }
      const float& GetIntensity() { return m_fIntensity; }

    private:
      maths::CVector3 m_v3LightColor = maths::CVector3::One;
      float m_fAttenuation = 1.0f;
      float m_fRange = 10.0f;
      float m_fIntensity = 1.0f;
    };
  }

}


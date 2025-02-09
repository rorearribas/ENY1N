#pragma once
#include "Light.h"

namespace render
{
  namespace lights
  {
    class CSpotLight : public CLight
    {
    public:
      CSpotLight() : CLight(SPOT_LIGHT) {}
      virtual ~CSpotLight() {}

      // Direction
      void SetDirection(const maths::CVector3& _v3Dir) { m_v3Direction = _v3Dir; }
      const maths::CVector3& GetDirection() const { return m_v3Direction; }
      // Color
      void SetColor(const maths::CVector3& _v3Color) { m_v3LightColor = _v3Color; }
      const maths::CVector3& GetColor() { return m_v3LightColor; }
      // CutOffAngle
      void SetCutOffAngle(float _fValue) { m_fCutOffAngle = _fValue; }
      const float& GetCutOffAngle() const { return m_fCutOffAngle; }
      // Range
      void SetRange(float _fRange) { m_fRange = _fRange; }
      const float& GetRange() const { return m_fRange; }
      // Intensity
      void SetIntensity(float _fIntensity) { m_fIntensity = _fIntensity; }
      const float& GetIntensity() const { return m_fIntensity; }

    private:
      maths::CVector3 m_v3LightColor = maths::CVector3::One;
      maths::CVector3 m_v3Direction = maths::CVector3::Up;
      float m_fCutOffAngle = 45.0f;
      float m_fRange = 10.0f;
      float m_fIntensity = 1.0f;
    };
  }
}


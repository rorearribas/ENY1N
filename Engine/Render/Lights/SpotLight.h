#pragma once
#include "Light.h"

namespace render
{
  namespace lights
  {
    class CSpotLight : public CLight
    {
    public:
      static math::CVector3 s_vDefaultDirection;

    public:
      CSpotLight() : CLight(ELight::SPOT_LIGHT) { SetDir(s_vDefaultDirection); }
      virtual ~CSpotLight() {}

      // Intensity
      inline void SetIntensity(float _fIntensity) { m_fIntensity = _fIntensity; }
      inline const float& GetIntensity() const { return m_fIntensity; }

      // Range
      inline void SetRange(float _fRange) { m_fRange = _fRange; }
      inline const float& GetRange() const { return m_fRange; }

    private:
      // Properties
      float m_fRange = 10.0f;
      float m_fIntensity = 1.0f;
    };
  }
}


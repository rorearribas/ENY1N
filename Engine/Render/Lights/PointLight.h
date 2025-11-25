#pragma once
#include "Light.h"
#include "Libs/Math/Vector3.h"

namespace render
{
  namespace lights
  {
    class CPointLight : public CLight
    {
    public:
      CPointLight() : CLight(ELight::POINT_LIGHT) {}
      virtual ~CPointLight() {}

      // Range
      inline void SetRange(float _fRange) { m_fRange = _fRange; }
      inline const float& GetRange() const { return m_fRange; }
      // Intensity
      inline void SetIntensity(float _fIntensity) { m_fIntensity = _fIntensity; }
      inline const float& GetIntensity() const { return m_fIntensity; }

    private:
      // Properties
      float m_fRange = 10.0f;
      float m_fIntensity = 1.0f;
    };
  }
}


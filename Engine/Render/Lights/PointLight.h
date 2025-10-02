#pragma once
#include "Light.h"
#include "Libs/Math/Vector3.h"

namespace render
{
  namespace lights
  {
    class CPointLight : public CBaseLight
    {
    public:
      CPointLight() : CBaseLight(POINT_LIGHT) {}
      virtual ~CPointLight() {}

      // Range
      void SetRange(float _fRange) { m_fRange = _fRange; }
      const float& GetRange() const { return m_fRange; }
      // Intensity
      void SetIntensity(float _fIntensity) { m_fIntensity = _fIntensity; }
      const float& GetIntensity() const { return m_fIntensity; }

    private:
      // Properties
      float m_fRange = 10.0f;
      float m_fIntensity = 1.0f;
    };
  }
}


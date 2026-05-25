#pragma once
#include "Light.h"
#include "Libs/Math/Transform.h"

namespace render
{
  namespace lights
  {
    class CDirectionalLight final : public CLight
    {
    public:
      CDirectionalLight();
      virtual ~CDirectionalLight() = default;

      // Intensity
      inline void SetIntensity(float _fIntensity) { m_fIntensity = _fIntensity; }
      inline const float& GetIntensity() const { return m_fIntensity; }

    private:
      // Properties
      float m_fIntensity;
    };
  }
}



#pragma once
#include "Libs/Maths/Transform.h"
#include "Light.h"

namespace render
{
  namespace lights
  {
    class CDirectionalLight : public CLight
    {
    public:
      CDirectionalLight();
      virtual ~CDirectionalLight() {}

      // Color
      void SetColor(const maths::CVector3& _v3Color) { m_v3LightColor = _v3Color; }
      const maths::CVector3& GetColor() { return m_v3LightColor; }
      // Intensity
      void SetIntensity(float _fIntensity) { m_fIntensity = _fIntensity; }
      const float& GetIntensity() { return m_fIntensity; }

    private:
      // Properties
      maths::CVector3 m_v3LightColor = maths::CVector3::One;
      float m_fIntensity = 1.0f;

      void InitDefaultLight();
    };
  }
}



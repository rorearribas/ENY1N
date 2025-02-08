#pragma once
#include "Light.h"
#include "Libs/Maths/Transform.h"

namespace render
{
  namespace lights
  {
    class CDirectionalLight : public CLight
    {
    public:
      static maths::CVector3 s_vDefaultDirectionalColor;
      static maths::CVector3 s_vDefaultDirection;

    public:

      CDirectionalLight() : CLight(DIRECTIONAL_LIGHT) {}
      virtual ~CDirectionalLight() {}

      virtual void UpdateLight() override;

      // Direction
      void SetDirection(const maths::CVector3& _v3Dir) { m_v3Direction = _v3Dir; }
      const maths::CVector3& GetDirection() const { return m_v3Direction; }
      // Color
      void SetColor(const maths::CVector3& _v3Color) { m_v3LightColor = _v3Color; }
      const maths::CVector3& GetColor() const { return m_v3LightColor; }
      // Intensity
      void SetIntensity(float _fIntensity) { m_fIntensity = _fIntensity; }
      const float& GetIntensity() const { return m_fIntensity; }

    private:
      // Properties
      maths::CVector3 m_v3Direction = s_vDefaultDirection;
      maths::CVector3 m_v3LightColor = s_vDefaultDirectionalColor;
      float m_fIntensity = 1.0f;
    };
  }
}



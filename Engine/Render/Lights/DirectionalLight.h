#pragma once
#include "Light.h"
#include "Libs/Maths/Transform.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"

namespace render
{
  namespace lights
  {
    class CDirectionalLight : public CLight
    {
    public:
      CDirectionalLight();
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
      void InitDefaultLight();

      // Properties
      maths::CVector3 m_v3Direction = maths::CVector3::Forward;
      maths::CVector3 m_v3LightColor = maths::CVector3::One;
      float m_fIntensity = 1.0f;

      // Buffer
      CConstantBuffer<SLightningData> m_oConstantBuffer;
    };
  }
}



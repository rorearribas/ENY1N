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

      void SetIntensity(float _fIntensity) { m_fIntensity = _fIntensity; }
      const float& GetIntensity() { return m_fIntensity; }
      void SetRange(float _fRange) { m_fRange = _fRange;}
      const float& GetRange() { return m_fRange; }

    private:
      float m_fIntensity = 1.0f;
      float m_fRange = 10.0f;
    };
  }

}


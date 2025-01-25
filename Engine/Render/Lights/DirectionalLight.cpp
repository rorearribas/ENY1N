#include "DirectionalLight.h"

namespace render
{
  namespace lights
  {
    CDirectionalLight::CDirectionalLight() : CLight()
    {
      InitDefaultLight();
    }

    void CDirectionalLight::InitDefaultLight()
    {
      SetColor(maths::CVector3(1.0f, 0.9f, 0.6f));
    }
  }
}

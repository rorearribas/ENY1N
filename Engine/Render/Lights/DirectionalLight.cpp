#include "DirectionalLight.h"
#include <cassert>
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace lights
  {
    // Static values
    static math::CVector3 s_vDefaultColor(0.95f, 1.0f, 0.75f);
    static math::CVector3 s_vDefaultDirection(0.0f, -1.0f, -0.5f);

    // ------------------------------------
    CDirectionalLight::CDirectionalLight() : CLight(ELight::DIRECTIONAL_LIGHT), m_fIntensity(1.0f)
    {
      SetDir(s_vDefaultDirection);
      SetColor(s_vDefaultColor);
    }
  }
}

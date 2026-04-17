#include "SpotLight.h"

namespace render
{
  namespace lights
  {
    // Static values
    static math::CVector3 s_vDefaultDir(0.0f, -1.0f, 0.0f);

    // ------------------------------------
    CSpotLight::CSpotLight() : CLight(ELight::SPOT_LIGHT)
    {
      SetDir(s_vDefaultDir);
    }
  }
}

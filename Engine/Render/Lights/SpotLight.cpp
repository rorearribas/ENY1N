#include "SpotLight.h"
#include "Libs/Math/Math.h"

namespace render
{
  namespace lights
  {
    // Static values
    static math::CVector3 s_vDefaultDirection(0.0f, -1.0f, 0.0f);

    // ------------------------------------
    CSpotLight::CSpotLight() : CLight(ELight::SPOT_LIGHT)
    {
      SetDir(s_vDefaultDirection);
    }
  }
}

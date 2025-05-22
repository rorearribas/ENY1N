#include "DirectionalLight.h"
#include <cassert>
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace lights
  {
    // Static values
    math::CVector3 CDirectionalLight::s_vDefaultDirectionalColor(0.95f, 1.0f, 0.75f);
    math::CVector3 CDirectionalLight::s_vDefaultDirection(1.0f, 1.0f, 1.0f);
    // ------------------------------------
  }
}

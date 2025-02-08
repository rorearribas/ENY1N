#include "DirectionalLight.h"
#include <cassert>
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace lights
  {
    // Static values
    maths::CVector3 CDirectionalLight::s_vDefaultDirectionalColor(1.0f, 0.9f, 0.6f);
    maths::CVector3 CDirectionalLight::s_vDefaultDirection(0.8f, -1.0f, 0.6f);
    // ------------------------------------
    void CDirectionalLight::UpdateLight(/*CConstantBuffer<SLightningData>& _oConstantBuffer*/)
    {
      //// Update buffer
      //_oConstantBuffer.GetData().DirectionalLight = this;
    }
  }
}

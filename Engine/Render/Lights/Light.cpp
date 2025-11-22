#include "Light.h"
#include "Libs/Math/Math.h"

namespace render
{
  namespace lights
  {
    const float s_fMaxValue(1.0f);
    const float s_fMinValue(0.0f);
    // ------------------------------------
    void CLight::SetDir(const math::CVector3& _v3Dir)
    {
      m_v3Direction.x = math::Clamp(_v3Dir.x, -s_fMaxValue, s_fMaxValue);
      m_v3Direction.y = math::Clamp(_v3Dir.y, -s_fMaxValue, s_fMaxValue);
      m_v3Direction.z = math::Clamp(_v3Dir.z, -s_fMaxValue, s_fMaxValue);
    }
    // ------------------------------------
    void CLight::SetColor(const math::CVector3& _v3Color)
    {
      m_v3Color.x = math::Clamp(_v3Color.x, s_fMinValue, s_fMaxValue);
      m_v3Color.y = math::Clamp(_v3Color.y, s_fMinValue, s_fMaxValue);
      m_v3Color.z = math::Clamp(_v3Color.z, s_fMinValue, s_fMaxValue);
    }
  }
}
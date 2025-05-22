#include "Light.h"
#include "Libs/Math/Math.h"

namespace render
{
	namespace lights
	{
    const float s_fMaxValue(1.0f);
    const float s_fMinValue(0.0f);
    // ------------------------------------
    void CBaseLight::SetDirection(const math::CVector3& _v3Dir)
    {
      m_v3Direction.X = math::Clamp(_v3Dir.X, -s_fMaxValue, s_fMaxValue);
      m_v3Direction.Y = math::Clamp(_v3Dir.Y, -s_fMaxValue, s_fMaxValue);
      m_v3Direction.Z = math::Clamp(_v3Dir.Z, -s_fMaxValue, s_fMaxValue);
    }
    // ------------------------------------
    void CBaseLight::SetColor(const math::CVector3& _v3Color)
    {
      m_v3Color.X = math::Clamp(_v3Color.X, s_fMinValue, s_fMaxValue);
      m_v3Color.Y = math::Clamp(_v3Color.Y, s_fMinValue, s_fMaxValue);
      m_v3Color.Z = math::Clamp(_v3Color.Z, s_fMinValue, s_fMaxValue);
    }
  }
}
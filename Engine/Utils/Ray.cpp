#include "Ray.h"

namespace physics
{
  math::CVector3 CRay::CalculatePoint(float _fDistance) const
  {
    return m_v3Origin + (m_v3Dir * _fDistance);
  }
  // ------------------------------------
  math::CVector3 CRay::CalculatePoint(const CRay& _oRay, float _fDist)
  {
    return _oRay.CalculatePoint(_fDist);
  }
}


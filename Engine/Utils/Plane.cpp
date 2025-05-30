#include "Plane.h"

namespace math
{
  float CPlane::DistanceToPoint(const math::CVector3& _v3Point) const
  {
    return math::CVector3(_v3Point - m_v3Pos).Dot(m_v3Normal);
  }
  float CPlane::DistanceToPoint(const CPlane& _oPlane, const math::CVector3& _v3Point)
  {
    return _oPlane.DistanceToPoint(_v3Point);
  }
}



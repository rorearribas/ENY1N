#include "Vector2.h"
#include <cmath>
#include "Math.h"

namespace math
{
  math::CVector2 CVector2::Zero(0.0f, 0.0f);
  math::CVector2 CVector2::Right(1.0f, 0.0f);
  math::CVector2 CVector2::Up(0.0f, 1.0f);
  // ------------------------------------
  float CVector2::DotProduct(const CVector2& _v2) const
  {
    return (this->x * _v2.x) + (this->y * _v2.y);
  }
  // ------------------------------------
  float CVector2::DotProduct(const CVector2& _vA, const CVector2& _vB)
  {
    return _vA.DotProduct(_vB);
  }
  // ------------------------------------
  float CVector2::Cross(const CVector2& _v2) const
  {
    return (this->x * _v2.y) - (this->y * _v2.x);
  }
  // ------------------------------------
  float CVector2::Cross(const CVector2& _vA, const CVector2& _vB)
  {
    return _vA.Cross(_vB);
  }
  // ------------------------------------
  float CVector2::Magnitude() const
  {
    return sqrt((x * x) + (y * y));
  }
  // ------------------------------------
  float CVector2::Magnitude(const CVector2& _v2)
  {
    return _v2.Magnitude();
  }
  // ------------------------------------
  void CVector2::Normalize()
  {
    float fMagnitude = Magnitude();
    if (fMagnitude > math::s_fEpsilon7)
    {
      x /= fMagnitude;
      y /= fMagnitude;
    }
  }
  // ------------------------------------
  math::CVector2 CVector2::Normalize(const CVector2& _v2)
  {
    math::CVector2 v2Normalized = _v2;
    v2Normalized.Normalize();
    return v2Normalized;
  }
}


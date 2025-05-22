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
    return (this->X * _v2.X) + (this->Y * _v2.Y);
  }
  // ------------------------------------
  float CVector2::DotProduct(const CVector2& _vA, const CVector2& _vB)
  {
    return _vA.DotProduct(_vB);
  }
  // ------------------------------------
  float CVector2::Cross(const CVector2& _v2) const
  {
    return (this->X * _v2.Y) - (this->Y * _v2.X);
  }
  // ------------------------------------
  float CVector2::Cross(const CVector2& _vA, const CVector2& _vB)
  {
    return _vA.Cross(_vB);
  }
  // ------------------------------------
  float CVector2::Magnitude() const
  {
    return sqrt((X * X) + (Y * Y));
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
      X /= fMagnitude;
      Y /= fMagnitude;
    }
  }
  // ------------------------------------
  math::CVector2 CVector2::Normalize(const CVector2& _v2)
  {
    math::CVector2 v2Dir = _v2;
    v2Dir.Normalize();
    return v2Dir;
  }
}


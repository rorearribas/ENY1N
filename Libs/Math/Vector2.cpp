#include "Vector2.h"
#include <cmath>

namespace math
{
  math::CVector2 CVector2::Zero(0.0f, 0.0f);
  math::CVector2 CVector2::Right(1.0f, 0.0f);
  math::CVector2 CVector2::Up(0.0f, 1.0f);

  // ------------------------------------
  float CVector2::DotProduct(const CVector2& _v2) const
  {
    return (X * _v2.X) + (Y * _v2.Y);
  }
  // ------------------------------------
  float CVector2::CrossProduct(const CVector2& _v2) const
  {
    return X * _v2.Y - Y * _v2.X;
  }
  // ------------------------------------
  void CVector2::Normalize()
  {
    float fMagnitude = sqrt((X * X) + (Y * Y));
    if (fMagnitude > 0) 
    {
      X /= fMagnitude;
      Y /= fMagnitude;
    }
  }

}


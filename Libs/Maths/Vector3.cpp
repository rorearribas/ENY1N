#include "Vector3.h"
#include <cmath>

namespace maths
{
  maths::CVector3 CVector3::Zero(0.0f, 0.0f, 0.0f);
  maths::CVector3 CVector3::One(1.0f, 1.0f, 1.0f);
  maths::CVector3 CVector3::Forward(0.0f, 0.0f, 1.0f);
  maths::CVector3 CVector3::Backward(0.0f, 0.0f, -1.0f);
  maths::CVector3 CVector3::Right(1.0f, 0.0f, 0.0f);
  maths::CVector3 CVector3::Up(0.0f, 1.0f, 0.0f);
  // ------------------------------------
  float CVector3::DotProduct(const CVector3& _v3) const
  {
    return (this->X * _v3.X) + (this->Y * _v3.Y) + (this->Z * _v3.Z);
  }
  // ------------------------------------
  float CVector3::Distance(const CVector3& _vDest, const CVector3& _vOrigin)
  {
    maths::CVector3 v3Substract = _vDest - _vOrigin;
    return sqrt(v3Substract.X * v3Substract.X + v3Substract.Y * v3Substract.Y + v3Substract.Z * v3Substract.Z);
  }
  // ------------------------------------
  bool CVector3::Equal(const maths::CVector3& _v3, float _fEpsilon) const
  {
    return (fabs(X - _v3.X) < _fEpsilon) && (fabs(Y - _v3.Y) < _fEpsilon) && (fabs(Z - _v3.Z) < _fEpsilon);
  }
  // ------------------------------------
  void CVector3::Abs()
  {
    X = abs(X);
    Z = abs(Y);
    Y = abs(Z);
  }
  // ------------------------------------
  CVector3 CVector3::Abs(const CVector3& _v3)
  {
    return CVector3(abs(_v3.X), abs(_v3.Y), abs(_v3.Z));
  }
  // ------------------------------------
  CVector3 CVector3::CrossProduct(const CVector3& _v3) const
  {
    return CVector3
    (
      (this->Y * _v3.Z - this->Z * _v3.Y), // X
      (this->Z * _v3.X - this->X * _v3.Z), // Y
      (this->X * _v3.Y - this->Y * _v3.X)  // Z
    );
  }
  // ------------------------------------
  maths::CVector3 CVector3::Normalize() const
  {
    float fMagnitude = sqrt((X * X) + (Y * Y) + (Z * Z));
    if (fMagnitude > 0)
    {
      return CVector3(X / fMagnitude, Y / fMagnitude, Z / fMagnitude);
    }
    return CVector3::Zero;
  }
}

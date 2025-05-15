#include "Vector3.h"
#include "Math.h"

namespace math
{
  math::CVector3 CVector3::Zero(0.0f, 0.0f, 0.0f);
  math::CVector3 CVector3::One(1.0f, 1.0f, 1.0f);
  math::CVector3 CVector3::Forward(0.0f, 0.0f, 1.0f);
  math::CVector3 CVector3::Backward(0.0f, 0.0f, -1.0f);
  math::CVector3 CVector3::Right(1.0f, 0.0f, 0.0f);
  math::CVector3 CVector3::Up(0.0f, 1.0f, 0.0f);
  // ------------------------------------
  float CVector3::Dot(const CVector3& _v3) const
  {
    return (this->X * _v3.X) + (this->Y * _v3.Y) + (this->Z * _v3.Z);
  }
  // ------------------------------------
  float CVector3::Dot(const CVector3& _vA, const CVector3& _vB)
  {
    return _vA.Dot(_vB);
  }
  // ------------------------------------
  float CVector3::Distance(const CVector3& _vDest, const CVector3& _vOrigin)
  {
    math::CVector3 v3Substract = _vDest - _vOrigin;
    return sqrt(v3Substract.X * v3Substract.X + v3Substract.Y * v3Substract.Y + v3Substract.Z * v3Substract.Z);
  }
  // ------------------------------------
  float CVector3::Distance(const CVector3& _v3Dest) const
  {
    return Distance(_v3Dest, *this);
  }
  // ------------------------------------
  bool CVector3::Equal(const math::CVector3& _v3, float _fEpsilon) const
  {
    return (fabs(X - _v3.X) < _fEpsilon) && (fabs(Y - _v3.Y) < _fEpsilon) && (fabs(Z - _v3.Z) < _fEpsilon);
  }
  // ------------------------------------
  float CVector3::Length(const CVector3& _v3)
  {
    return _v3.Length();
  }
  // ------------------------------------
  float CVector3::Length() const
  {
    return sqrt(X * X + Y * Y + Z * Z);
  }

  bool CVector3::operator==(const CVector3& _v3) const
  {
    return std::fabs(X - _v3.X) < math::s_fEpsilon5 &&
      std::fabs(Y - _v3.Y) < math::s_fEpsilon5 &&
      std::fabs(Z - _v3.Z) < math::s_fEpsilon5;
  }
  // ------------------------------------
  bool CVector3::IsZero(const CVector3& _v3)
  {
    return _v3.IsZero();
  }
  // ------------------------------------
  bool CVector3::IsZero() const
  {
    return abs(X) <= s_fEpsilon2 && abs(Y) <= s_fEpsilon2 && abs(Z) <= s_fEpsilon2;
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
  CVector3 CVector3::Cross(const CVector3& _v3) const
  {
    return CVector3
    (
      (this->Y * _v3.Z - this->Z * _v3.Y), // X
      (this->Z * _v3.X - this->X * _v3.Z), // Y
      (this->X * _v3.Y - this->Y * _v3.X)  // Z
    );
  }
  // ------------------------------------
  math::CVector3 CVector3::Cross(const CVector3& _vA, const CVector3& _vB)
  {
    return _vA.Cross(_vB);
  }
  // ------------------------------------
  void CVector3::Normalize()
  {
    float fMagnitude = sqrt((X * X) + (Y * Y) + (Z * Z));
    if (fMagnitude > math::s_fEpsilon7)
    {
      X /= fMagnitude;
      Y /= fMagnitude;
      Z /= fMagnitude;
    }
  }
  // ------------------------------------
  math::CVector3 CVector3::Normalize(const CVector3& _v3)
  {
    float fMagnitude = sqrt((_v3.X * _v3.X) + (_v3.Y * _v3.Y) + (_v3.Z * _v3.Z));
    return fMagnitude > 0.0f ? CVector3(_v3.X / fMagnitude, _v3.Y / fMagnitude, _v3.Z / fMagnitude) : CVector3::Zero;
  }
}

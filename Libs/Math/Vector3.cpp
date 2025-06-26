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
    return v3Substract.Magnitude();
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
  float CVector3::Magnitude(const CVector3& _v3)
  {
    return _v3.Magnitude();
  }
  // ------------------------------------
  float CVector3::Magnitude() const
  {
    return sqrt((X * X) + (Y * Y) + (Z * Z));
  }
  // ------------------------------------
  bool CVector3::operator==(const CVector3& _v3) const
  {
    return std::fabs(this->X - _v3.X) < math::s_fEpsilon5 &&
      std::fabs(this->Y - _v3.Y) < math::s_fEpsilon5 &&
      std::fabs(this->Z - _v3.Z) < math::s_fEpsilon5;
  }
  // ------------------------------------
  float CVector3::operator[](uint32_t _uIndex) const
  {
    switch (_uIndex)
    {
      case 0: return this->X;
      case 1: return this->Y;
      case 2: return this->Z;
      default: return FLT_MAX;
    }
  }
  // ------------------------------------
  bool CVector3::IsZero(const CVector3& _v3)
  {
    return _v3.IsZero();
  }
  // ------------------------------------
  bool CVector3::IsZero() const
  {
    return std::abs(X) <= s_fEpsilon2 && std::abs(Y) <= s_fEpsilon2 && std::abs(Z) <= s_fEpsilon2;
  }
  // ------------------------------------
  void CVector3::Abs()
  {
    this->X = std::abs(this->X);
    this->Z = std::abs(this->Y);
    this->Y = std::abs(this->Z);
  }
  // ------------------------------------
  CVector3 CVector3::Abs(const CVector3& _v3)
  {
    return CVector3(std::abs(_v3.X), std::abs(_v3.Y), std::abs(_v3.Z));
  }
  // ------------------------------------
  CVector3 CVector3::Cross(const CVector3& _v3) const
  {
    return CVector3
    (
      (this->Y * _v3.Z) - (this->Z * _v3.Y), // X
      (this->Z * _v3.X) - (this->X * _v3.Z), // Y
      (this->X * _v3.Y) - (this->Y * _v3.X)  // Z
    );
  }
  // ------------------------------------
  math::CVector3 CVector3::Cross(const CVector3& _vA, const CVector3& _vB)
  {
    return _vA.Cross(_vB);
  }
  // ------------------------------------
  CVector3 CVector3::Normalize()
  {
    CVector3 v3 = *this;
    float fMagnitude = this->Magnitude();
    if (fMagnitude > math::s_fEpsilon7)
    {
      v3.X /= fMagnitude;
      v3.Y /= fMagnitude;
      v3.Z /= fMagnitude;
    }
    return v3;
  }
  // ------------------------------------
  math::CVector3 CVector3::Normalize(const CVector3& _v3)
  {
    CVector3 v3Dir = _v3;
    v3Dir.Normalize();
    return v3Dir;
  }
}

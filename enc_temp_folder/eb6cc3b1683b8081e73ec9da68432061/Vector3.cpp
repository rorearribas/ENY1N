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
  void CVector3::Normalize()
  {
    float fMagnitude = this->Magnitude();
    if (fMagnitude > math::s_fEpsilon7)
    {
      this->x /= fMagnitude;
      this->y /= fMagnitude;
      this->z /= fMagnitude;
    }
    else 
    {
      this->x = 0.0f;
      this->y = 0.0f;
      this->z = 0.0f;
    }
  }
  // ------------------------------------
  math::CVector3 CVector3::Normalize(const CVector3& _v3)
  {
    CVector3 v3Normalized = _v3;
    v3Normalized.Normalize();
    return v3Normalized;
  }
  // ------------------------------------
  void CVector3::Abs()
  {
    this->x = std::abs(this->x);
    this->z = std::abs(this->y);
    this->y = std::abs(this->z);
  }
  // ------------------------------------
  CVector3 CVector3::Abs(const CVector3& _v3)
  {
    return CVector3(std::abs(_v3.x), std::abs(_v3.y), std::abs(_v3.z));
  }
  // ------------------------------------
  float CVector3::Dot(const CVector3& _v3) const
  {
    return (this->x * _v3.x) + (this->y * _v3.y) + (this->z * _v3.z);
  }
  // ------------------------------------
  float CVector3::Dot(const CVector3& _vA, const CVector3& _vB)
  {
    return _vA.Dot(_vB);
  }
  // ------------------------------------
  CVector3 CVector3::Cross(const CVector3& _v3) const
  {
    return CVector3
    (
      (this->y * _v3.z) - (this->z * _v3.y), // X
      (this->z * _v3.x) - (this->x * _v3.z), // Y
      (this->x * _v3.y) - (this->y * _v3.x)  // Z
    );
  }
  // ------------------------------------
  math::CVector3 CVector3::Cross(const CVector3& _vA, const CVector3& _vB)
  {
    return _vA.Cross(_vB);
  }
  // ------------------------------------
  float CVector3::Distance(const CVector3& _v3Dest, const CVector3& _v3Origin)
  {
    math::CVector3 v3Substract = _v3Dest - _v3Origin;
    return v3Substract.Magnitude();
  }
  // ------------------------------------
  float CVector3::Distance(const CVector3& _v3Dest) const
  {
    return Distance(_v3Dest, *this);
  }
  // ------------------------------------
  float CVector3::SqrDist(const CVector3& _v3Dest, const CVector3& _v3Origin)
  {
    math::CVector3 v3Substract = _v3Dest - _v3Origin;
    return v3Substract.GetSqrDist();
  }
  // ------------------------------------
  float CVector3::SqrDist(const CVector3& _v3Dest) const
  {
    return SqrDist(_v3Dest, *this);
  }
  // ------------------------------------
  bool CVector3::Equal(const math::CVector3& _v3, float _fEpsilon) const
  {
    return (fabs(x - _v3.x) < _fEpsilon) && (fabs(y - _v3.y) < _fEpsilon) && (fabs(z - _v3.z) < _fEpsilon);
  }
  // ------------------------------------
  float CVector3::Magnitude(const CVector3& _v3)
  {
    return _v3.Magnitude();
  }
  // ------------------------------------
  float CVector3::Magnitude() const
  {
    return sqrt((x * x) + (y * y) + (z * z));
  }
  // ------------------------------------
  float CVector3::GetSqrDist(const CVector3& _v3)
  {
    return _v3.GetSqrDist();
  }
  // ------------------------------------
  float CVector3::GetSqrDist() const
  {
    return (x * x) + (y * y) + (z * z);
  }
  // ------------------------------------
  bool CVector3::operator==(const CVector3& _v3) const
  {
    return std::fabs(this->x - _v3.x) < math::s_fEpsilon5 &&
      std::fabs(this->y - _v3.y) < math::s_fEpsilon5 &&
      std::fabs(this->z - _v3.z) < math::s_fEpsilon5;
  }
  // ------------------------------------
  float CVector3::operator[](uint32_t _uIndex) const
  {
    switch (_uIndex)
    {
    case 0: return this->x;
    case 1: return this->y;
    case 2: return this->z;
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
    return std::abs(x) <= s_fEpsilon2 && std::abs(y) <= s_fEpsilon2 && std::abs(z) <= s_fEpsilon2;
  }
  // ------------------------------------
  float CVector3::AngleBetween(const CVector3& _v3) const
  {
    return AngleBetween(*this, _v3);
  }
  // ------------------------------------
  float CVector3::AngleBetween(const CVector3& _vA, const CVector3& _vB)
  {
    // Cos
    float fCos = _vA.Dot(_vB) / (_vA.Magnitude() * _vB.Magnitude());
    fCos = math::Clamp(fCos, -1.0f, 1.0f); // Clamp

    // Radians
    return std::acos(fCos);
  }
}

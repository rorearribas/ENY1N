#pragma once
#include <functional>

namespace maths
{
  class CVector3
  {
  public:
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

  public:
    static CVector3 Zero, One;
    static CVector3 Forward, Backward, Right, Up;

    CVector3() = default;
    CVector3(float _x, float _y, float _z) : X(_x), Y(_y), Z(_z) {}
    ~CVector3() {}

    inline void operator+=(const CVector3& _v3) { X += _v3.X; Y += _v3.Y; Z += _v3.Z; }
    inline void operator-=(const CVector3& _v3) { X -= _v3.X; Y -= _v3.Y; Z -= _v3.Z; }

    inline void operator *=(float _fValue) { X *= _fValue; Y *= _fValue; Z *= _fValue;}
    inline void operator /=(float _fValue) { X /= _fValue; Y /= _fValue; Z /= _fValue; }

    inline CVector3 operator*(float _fValue) const { return CVector3(X * _fValue, Y * _fValue, Z * _fValue); }
    inline CVector3 operator*(const CVector3& _v3) const { return CVector3(X * _v3.X, Y * _v3.Y, Z * _v3.Z); }

    inline CVector3 operator/(float _fValue) const {  return CVector3(X / _fValue, Y / _fValue, Z / _fValue); }
    inline CVector3 operator+(const CVector3& _other) const { return CVector3(X + _other.X, Y + _other.Y, Z + _other.Z); }

    inline CVector3 operator-(const CVector3& _other) const { return CVector3(X - _other.X, Y - _other.Y, Z - _other.Z); }
    inline CVector3 operator-(float _fValue) const { return CVector3(X - _fValue, Y - _fValue, Z - _fValue); }

    inline CVector3 operator-() const { return CVector3(-X, -Y, -Z); }

    inline bool operator<(const CVector3& _other) const { return this->DotProduct(*this) < _other.DotProduct(_other); }
    inline bool operator==(const CVector3& _v3) const
    {
      constexpr float EPSILON = 1e-5f;
      return std::fabs(X - _v3.X) < EPSILON && std::fabs(Y - _v3.Y) < EPSILON && std::fabs(Z - _v3.Z) < EPSILON;
    }
    bool operator!=(const CVector3& other) const { return !(*this == other); }

    static float Distance(const CVector3& _vDest, const CVector3& _vOrigin);

    void Abs();
    static CVector3 Abs(const CVector3& _v3);

    bool Equal(const maths::CVector3& _v3, float _fEpsilon = 0.0001f) const;
    float DotProduct(const CVector3& _v3) const;
    CVector3 CrossProduct(const CVector3& _v3) const;
    CVector3 Normalize() const;

    float Length() const
    {
      return std::sqrt(X * X + Y * Y + Z * Z);
    }

  };
}

namespace std
{
  template<>
  struct std::hash<maths::CVector3>
  {
    std::size_t operator()(const maths::CVector3& v) const
    {
      size_t hx = std::hash<float>()(v.X);
      size_t hy = std::hash<float>()(v.Y);
      size_t hz = std::hash<float>()(v.Z);
      return hx ^ (hy << 1) ^ (hz << 2);
    }
  };
}




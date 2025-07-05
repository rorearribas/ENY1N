#pragma once
#include <functional>

namespace math
{
  class CVector3
  {
  public:
    static CVector3 Zero, One;
    static CVector3 Forward, Backward, Right, Up;

    float X;
    float Y;
    float Z;

  public:
    CVector3() : X(0.0f), Y(0.0f), Z(0.0f) {}
    CVector3(float _x, float _y, float _z) : X(_x), Y(_y), Z(_z) {}
    ~CVector3() {}

    inline CVector3 operator+(const CVector3& _v3) const { return CVector3(X + _v3.X, Y + _v3.Y, Z + _v3.Z); }
    inline CVector3 operator-(const CVector3& _v3) const { return CVector3(X - _v3.X, Y - _v3.Y, Z - _v3.Z); }
    inline CVector3 operator*(const CVector3& _v3) const { return CVector3(X * _v3.X, Y * _v3.Y, Z * _v3.Z); }
    inline CVector3 operator/(const CVector3& _v3) const { return CVector3(X / _v3.X, Y / _v3.Y, Z / _v3.Z); }

    inline CVector3 operator+(float _fValue) const { return CVector3(X + _fValue, Y + _fValue, Z + _fValue); }
    inline CVector3 operator-(float _fValue) const { return CVector3(X - _fValue, Y - _fValue, Z - _fValue); }
    inline CVector3 operator*(float _fValue) const { return CVector3(X * _fValue, Y * _fValue, Z * _fValue); }
    inline CVector3 operator/(float _fValue) const { return CVector3(X / _fValue, Y / _fValue, Z / _fValue); }

    inline void operator+=(const CVector3& _v3) { X += _v3.X; Y += _v3.Y; Z += _v3.Z; }
    inline void operator-=(const CVector3& _v3) { X -= _v3.X; Y -= _v3.Y; Z -= _v3.Z; }
    inline void operator*=(const CVector3& _v3) { X *= _v3.X; Y *= _v3.Y; Z *= _v3.Z; }
    inline void operator/=(const CVector3& _v3) { X /= _v3.X; Y /= _v3.Y; Z /= _v3.Z; }

    inline void operator +=(float _fValue) { X += _fValue; Y += _fValue; Z += _fValue; }
    inline void operator -=(float _fValue) { X -= _fValue; Y -= _fValue; Z -= _fValue; }
    inline void operator *=(float _fValue) { X *= _fValue; Y *= _fValue; Z *= _fValue; }
    inline void operator /=(float _fValue) { X /= _fValue; Y /= _fValue; Z /= _fValue; }

    inline CVector3 operator-() const { return CVector3(-X, -Y, -Z); }
    inline CVector3 operator+() const { return CVector3(+X, +Y, +Z); }

    inline bool operator<(const CVector3& _other) const { return this->Dot(*this) < _other.Dot(_other); }
    bool operator!=(const CVector3& other) const { return !(*this == other); }
    bool operator==(const CVector3& _v3) const;

    float operator[] (uint32_t _uIndex) const;

    static bool IsZero(const CVector3& _v3);
    bool IsZero() const;

    static CVector3 Abs(const CVector3& _v3);
    void Abs();

    static CVector3 Normalize(const CVector3& _v3);
    CVector3 Normalize() const;

    static float Dot(const CVector3& _vA, const CVector3& _vB);
    float Dot(const CVector3& _v3) const;

    static CVector3 Cross(const CVector3& _vA, const CVector3& _vB);
    CVector3 Cross(const CVector3& _v3) const;

    static float Distance(const CVector3& _v3Dest, const CVector3& _v3Origin);
    float Distance(const CVector3& _v3Dest) const;

    bool Equal(const CVector3& _v3, float _fEpsilon = 0.0001f) const;

    static float Magnitude(const CVector3& _v3);
    float Magnitude() const;
  };
}

namespace std
{
  template<>
  struct std::hash<math::CVector3>
  {
    std::size_t operator()(const math::CVector3& v) const
    {
      size_t hx = std::hash<float>()(v.X);
      size_t hy = std::hash<float>()(v.Y);
      size_t hz = std::hash<float>()(v.Z);
      return hx ^ (hy << 1) ^ (hz << 2);
    }
  };
}




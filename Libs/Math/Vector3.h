#pragma once
#include <functional>

namespace math
{
  class CVector3
  {
  public:
    static CVector3 Zero, One;
    static CVector3 Forward, Backward, Right, Up;

    float x;
    float y;
    float z;

  public:
    CVector3() : x(0.0f), y(0.0f), z(0.0f) {}
    CVector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    ~CVector3() {}

    inline CVector3 operator+(const CVector3& _v3) const { return CVector3(x + _v3.x, y + _v3.y, z + _v3.z); }
    inline CVector3 operator-(const CVector3& _v3) const { return CVector3(x - _v3.x, y - _v3.y, z - _v3.z); }
    inline CVector3 operator*(const CVector3& _v3) const { return CVector3(x * _v3.x, y * _v3.y, z * _v3.z); }
    inline CVector3 operator/(const CVector3& _v3) const { return CVector3(x / _v3.x, y / _v3.y, z / _v3.z); }

    inline CVector3 operator+(float _fValue) const { return CVector3(x + _fValue, y + _fValue, z + _fValue); }
    inline CVector3 operator-(float _fValue) const { return CVector3(x - _fValue, y - _fValue, z - _fValue); }
    inline CVector3 operator*(float _fValue) const { return CVector3(x * _fValue, y * _fValue, z * _fValue); }
    inline CVector3 operator/(float _fValue) const { return CVector3(x / _fValue, y / _fValue, z / _fValue); }

    inline void operator+=(const CVector3& _v3) { x += _v3.x; y += _v3.y; z += _v3.z; }
    inline void operator-=(const CVector3& _v3) { x -= _v3.x; y -= _v3.y; z -= _v3.z; }
    inline void operator*=(const CVector3& _v3) { x *= _v3.x; y *= _v3.y; z *= _v3.z; }
    inline void operator/=(const CVector3& _v3) { x /= _v3.x; y /= _v3.y; z /= _v3.z; }

    inline void operator +=(float _fValue) { x += _fValue; y += _fValue; z += _fValue; }
    inline void operator -=(float _fValue) { x -= _fValue; y -= _fValue; z -= _fValue; }
    inline void operator *=(float _fValue) { x *= _fValue; y *= _fValue; z *= _fValue; }
    inline void operator /=(float _fValue) { x /= _fValue; y /= _fValue; z /= _fValue; }

    inline CVector3 operator-() const { return CVector3(-x, -y, -z); }
    inline CVector3 operator+() const { return CVector3(+x, +y, +z); }

    inline bool operator<(const CVector3& _other) const { return this->Dot(*this) < _other.Dot(_other); }
    bool operator!=(const CVector3& other) const { return !(*this == other); }
    bool operator==(const CVector3& _v3) const;

    float operator[] (uint32_t _uIndex) const;

    static CVector3 Abs(const CVector3& _v3);
    void Abs();

    static CVector3 Normalize(const CVector3& _v3);
    void Normalize();

    static float Dot(const CVector3& _vA, const CVector3& _vB);
    float Dot(const CVector3& _v3) const;

    static CVector3 Cross(const CVector3& _vA, const CVector3& _vB);
    CVector3 Cross(const CVector3& _v3) const;

    static float Distance(const CVector3& _v3Dest, const CVector3& _v3Origin);
    float Distance(const CVector3& _v3Dest) const;

    static float SqrDist(const CVector3& _v3Dest, const CVector3& _v3Origin);
    float SqrDist(const CVector3& _v3Dest) const;

    static float Magnitude(const CVector3& _v3);
    float Magnitude() const;

    static float GetSqrDist(const CVector3& _v3);
    float GetSqrDist() const;

    float AngleBetween(const CVector3& _v3) const;
    static float AngleBetween(const CVector3& _vA, const CVector3& _vB);

    bool Equal(const CVector3& _v3, float _fEpsilon = 0.0001f) const;
    static bool IsZero(const CVector3& _v3);
    bool IsZero() const;
  };
}

namespace std
{
  template<>
  struct std::hash<math::CVector3>
  {
    std::size_t operator()(const math::CVector3& v) const
    {
      size_t hx = std::hash<float>()(v.x);
      size_t hy = std::hash<float>()(v.y);
      size_t hz = std::hash<float>()(v.z);
      return hx ^ (hy << 1) ^ (hz << 2);
    }
  };
}




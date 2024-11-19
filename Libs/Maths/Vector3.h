#pragma once

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

    inline CVector3 operator*(const float& _fValue) const { return CVector3(X * _fValue, Y * _fValue, Z * _fValue); }
    inline CVector3 operator+(const CVector3& _v3) const { return CVector3(X + _v3.X, Y + _v3.Y, Z + _v3.Z); }
    inline CVector3 operator-(const CVector3& _v3) const { return CVector3(X - _v3.X, Y - _v3.Y, Z - _v3.Z); }
    inline CVector3 operator-() const { return CVector3(-X, -Y, -Z); }

    inline bool operator==(const CVector3& _v3) const { return X == _v3.X && Y == _v3.Y && Z == _v3.Z; }
    inline bool operator!=(const CVector3& _v3)  const { return X != _v3.X && Y != _v3.Y && Z != _v3.Z; }


    CVector3 CrossProduct(const CVector3& _v3) const;
    float DotProduct(const CVector3& _v3) const;
    CVector3 Normalized() const;
  };

}



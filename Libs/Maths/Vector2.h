#pragma once

namespace maths
{
  class CVector2
  {
  public:
    float X = 0.0f;
    float Y = 0.0f;

  public:
    static CVector2 Zero;
    static CVector2 Right;
    static CVector2 Up;

    CVector2(float _x, float _y) : X(_x), Y(_y) {}
    ~CVector2() {}

    inline void operator+=(const CVector2& _v2) { X += _v2.X; Y += _v2.Y; }
    inline void operator-=(const CVector2& _v2) { X -= _v2.X; Y -= _v2.Y; }

    inline void operator*=(const float _fValue) { X *= _fValue; Y *= _fValue; }
    inline CVector2 operator*(const float _fValue) const { return CVector2(X * _fValue, Y * _fValue); }

    inline CVector2 operator+(const CVector2& _v2) const { return CVector2(X + _v2.X, Y + _v2.Y); }
    inline CVector2 operator-(const CVector2& _v2) const { return CVector2(X - _v2.X, Y - _v2.Y); }
    inline CVector2 operator*(const CVector2& _v2) const { return CVector2(X * _v2.X, Y * _v2.Y); }

    inline bool operator==(const CVector2& _v2) const { return X == _v2.X && Y == _v2.Y; }
    inline bool operator!=(const CVector2& _v2) const { return X != _v2.X && Y != _v2.Y; }

    float DotProduct(const CVector2& _v2) const;
    float CrossProduct(const CVector2& _v2) const;
    void Normalize();
  };
}




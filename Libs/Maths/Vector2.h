#pragma once
#include <functional>

namespace maths
{
  class CVector2
  {
  public:
    static CVector2 Zero;
    static CVector2 Right;
    static CVector2 Up;

    float X = 0.0f;
    float Y = 0.0f;

  public:
    CVector2() = default;
    CVector2(float _x, float _y) : X(_x), Y(_y) {}
    ~CVector2() {}

    inline void operator+=(const CVector2& _v2) { X += _v2.X; Y += _v2.Y; }
    inline void operator-=(const CVector2& _v2) { X -= _v2.X; Y -= _v2.Y; }
    inline void operator*=(const CVector2& _v2) { X *= _v2.X; Y *= _v2.Y; }
    inline void operator/=(const CVector2& _v2) { X /= _v2.X; Y /= _v2.Y; }

    inline void operator+=(float _fValue) { X += _fValue; Y += _fValue; }
    inline void operator-=(float _fValue) { X -= _fValue; Y -= _fValue; }
    inline void operator*=(float _fValue) { X *= _fValue; Y *= _fValue; }
    inline void operator/=(float _fValue) { X /= _fValue; Y /= _fValue; }

    inline CVector2 operator+(const CVector2& _v2) const { return CVector2(X + _v2.X, Y + _v2.Y); }
    inline CVector2 operator-(const CVector2& _v2) const { return CVector2(X - _v2.X, Y - _v2.Y); }
    inline CVector2 operator*(const CVector2& _v2) const { return CVector2(X * _v2.X, Y * _v2.Y); }
    inline CVector2 operator/(const CVector2& _v2) const { return CVector2(X / _v2.X, Y / _v2.Y); }

    inline CVector2 operator+(float _fValue) const { return CVector2(X + _fValue, Y + _fValue); }
    inline CVector2 operator-(float _fValue) const { return CVector2(X - _fValue, Y - _fValue); }
    inline CVector2 operator*(float _fValue) const { return CVector2(X * _fValue, Y * _fValue); }
    inline CVector2 operator/(float _fValue) const { return CVector2(X / _fValue, Y / _fValue); }

    inline CVector2 operator-() const { return CVector2(-X, -Y); }
    inline CVector2 operator+() const { return CVector2(+X, +Y); }

    inline bool operator<(const CVector2& _other) const { return X < _other.X && Y < _other.Y; }
    inline bool operator==(const CVector2& _v2) const 
    { 
      constexpr float EPSILON = 1e-5f;
      return std::fabs(X - _v2.X) < EPSILON && std::fabs(Y - _v2.Y) < EPSILON;
    }
    bool operator!=(const CVector2& other) const { return !(*this == other); }

    float DotProduct(const CVector2& _v2) const;
    float CrossProduct(const CVector2& _v2) const;
    void Normalize();
  };
}

namespace std
{
  template<>
  struct std::hash<maths::CVector2>
  {
    std::size_t operator()(const maths::CVector2& v) const
    {
      size_t hx = std::hash<float>()(v.X);
      size_t hy = std::hash<float>()(v.Y);
      return hx ^ (hy << 1);
    }
  };
}




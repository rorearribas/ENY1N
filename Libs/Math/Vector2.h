#pragma once
#include <functional>

namespace math
{
  class CVector2
  {
  public:
    static CVector2 Zero;
    static CVector2 Right;
    static CVector2 Up;

    float x;
    float y;

  public:
    CVector2() : x(0.0f), y(0.0f) {}
    CVector2(float _x, float _y) : x(_x), y(_y) {}
    ~CVector2() {}

    inline void operator+=(const CVector2& _v2) { x += _v2.x; y += _v2.y; }
    inline void operator-=(const CVector2& _v2) { x -= _v2.x; y -= _v2.y; }
    inline void operator*=(const CVector2& _v2) { x *= _v2.x; y *= _v2.y; }
    inline void operator/=(const CVector2& _v2) { x /= _v2.x; y /= _v2.y; }

    inline void operator+=(float _fValue) { x += _fValue; y += _fValue; }
    inline void operator-=(float _fValue) { x -= _fValue; y -= _fValue; }
    inline void operator*=(float _fValue) { x *= _fValue; y *= _fValue; }
    inline void operator/=(float _fValue) { x /= _fValue; y /= _fValue; }

    inline CVector2 operator+(const CVector2& _v2) const { return CVector2(x + _v2.x, y + _v2.y); }
    inline CVector2 operator-(const CVector2& _v2) const { return CVector2(x - _v2.x, y - _v2.y); }
    inline CVector2 operator*(const CVector2& _v2) const { return CVector2(x * _v2.x, y * _v2.y); }
    inline CVector2 operator/(const CVector2& _v2) const { return CVector2(x / _v2.x, y / _v2.y); }

    inline CVector2 operator+(float _fValue) const { return CVector2(x + _fValue, y + _fValue); }
    inline CVector2 operator-(float _fValue) const { return CVector2(x - _fValue, y - _fValue); }
    inline CVector2 operator*(float _fValue) const { return CVector2(x * _fValue, y * _fValue); }
    inline CVector2 operator/(float _fValue) const { return CVector2(x / _fValue, y / _fValue); }

    inline CVector2 operator-() const { return CVector2(-x, -y); }
    inline CVector2 operator+() const { return CVector2(+x, +y); }

    inline bool operator>(const CVector2& _v2) const { return x > _v2.x && y >_v2.y; }
    inline bool operator<(const CVector2& _v2) const { return x < _v2.x && y < _v2.y; }
    bool operator==(const CVector2& _v2) const;
    bool operator!=(const CVector2& _v2) const { return !(*this == _v2); }

    float DotProduct(const CVector2& _v2) const;
    static float DotProduct(const CVector2& _vA, const CVector2& _vB);

    float Cross(const CVector2& _v2) const;
    static float Cross(const CVector2& _vA, const CVector2& _vB);

    static float Magnitude(const CVector2& _v2);
    float Magnitude() const;

    static CVector2 Normalize(const CVector2& _v2);
    void Normalize();
  };
}

namespace std
{
  template<>
  struct std::hash<math::CVector2>
  {
    std::size_t operator()(const math::CVector2& v) const
    {
      size_t hx = std::hash<float>()(v.x);
      size_t hy = std::hash<float>()(v.y);
      return hx ^ (hy << 1);
    }
  };
}




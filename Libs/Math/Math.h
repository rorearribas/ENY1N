#pragma once
#include "Vector2.h"
#include "Vector3.h"

namespace math
{
  // Static values
  static const double s_fPI = 3.14159265358979323846;

  // Epsilon
  static const float s_fEpsilon1 = 1e-1f;
  static const float s_fEpsilon2 = 1e-2f;
  static const float s_fEpsilon3 = 1e-3f;
  static const float s_fEpsilon4 = 1e-4f;
  static const float s_fEpsilon5 = 1e-5f;
  static const float s_fEpsilon6 = 1e-6f;
  static const float s_fEpsilon7 = 1e-7f;

  // Functions
  inline float lerp(float a, float b, float f)
  {
    return static_cast<float>(a * (1.0 - f) + (b * f));
  }

  template <typename T>
  T Clamp(T value, T min, T max)
  {
    if (value < min) return min;
    if (value > max) return max;
    return value;
  }

  template <typename T>
  T Max(const T& Object1, const T& Object2)
  {
    return Object1 > Object2 ? Object1 : Object2;
  }

  template <typename T>
  T Min(const T& Object1, const T& Object2)
  {
    return Object1 < Object2 ? Object1 : Object2;
  }

  inline float DegreesToRadians(float _fDegree)
  {
    return _fDegree * static_cast<float>((s_fPI / 180.0f));
  }

  inline math::CVector3 DegreesToRadians(const math::CVector3& _v3)
  {
    float fX = DegreesToRadians(_v3.X);
    float fY = DegreesToRadians(_v3.Y);
    float fZ = DegreesToRadians(_v3.Z);
    return math::CVector3(fX, fY, fZ);
  }

  inline math::CVector2 DegreesToRadians(const math::CVector2& _v2)
  {
    float fX = DegreesToRadians(_v2.X);
    float fY = DegreesToRadians(_v2.Y);
    return math::CVector2(fX, fY);
  }

  inline float RadiansToDegrees(float _fRadians)
  {
    return _fRadians * static_cast<float>((180.0f / s_fPI));
  }

  inline math::CVector3 RadiansToDegrees(const math::CVector3& _v3)
  {
    float fX = RadiansToDegrees(_v3.X);
    float fY = RadiansToDegrees(_v3.Y);
    float fZ = RadiansToDegrees(_v3.Z);
    return math::CVector3(fX, fY, fZ);
  }

  inline math::CVector2 RadiansToDegrees(const math::CVector2& _v2)
  {
    float fX = RadiansToDegrees(_v2.X);
    float fY = RadiansToDegrees(_v2.Y);
    return  math::CVector2(fX, fY);
  }
}

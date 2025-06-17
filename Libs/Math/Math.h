#pragma once
#include "Vector2.h"
#include "Vector3.h"

namespace math
{
  // Static values
  static constexpr double s_fPI = 3.14159265358979323846;
  static constexpr double s_fHalfPI = s_fPI / 2.0f;

  // Epsilon
  static constexpr float s_fEpsilon1 = 1e-1f;
  static constexpr float s_fEpsilon2 = 1e-2f;
  static constexpr float s_fEpsilon3 = 1e-3f;
  static constexpr float s_fEpsilon4 = 1e-4f;
  static constexpr float s_fEpsilon5 = 1e-5f;
  static constexpr float s_fEpsilon6 = 1e-6f;
  static constexpr float s_fEpsilon7 = 1e-7f;

  // Functions
  inline float Lerp(float a, float b, float f)
  {
    return static_cast<float>(a * (1.0 - f) + (b * f));
  }

  template <typename T>
  inline T Clamp(T Value, T Min, T Max)
  {
    return Value < Min ? Min : Value > Max ? Max : Value;
  }

  template <typename T>
  inline T Max(const T& Object1, const T& Object2)
  {
    return Object1 > Object2 ? Object1 : Object2;
  }

  template <typename T>
  inline T Min(const T& Object1, const T& Object2)
  {
    return Object1 < Object2 ? Object1 : Object2;
  }

  inline float DegreesToRadians(float _fDegrees)
  {
    return _fDegrees * static_cast<float>(s_fPI / 180.0f);
  }

  inline float RadiansToDegrees(float _fRadians)
  {
    return _fRadians * static_cast<float>(180.0f / s_fPI);
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

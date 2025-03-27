#pragma once
#include "Vector2.h"
#include "Vector3.h"

namespace maths
{
  // Static values
  static const double s_fPI = 3.14159265358979323846;
  static const double s_fEpsilon = 1.192092896e-07F;

  // Functions
  inline float lerp(float a, float b, float f)
  {
    return static_cast<float>(a * (1.0 - f) + (b * f));
  }

  template <typename T>
  T clamp(T value, T min, T max)
  {
    if (value < min) return min;
    if (value > max) return max;
    return value;
  }

  inline float DegreesToRadians(float _fDegree)
  {
    return _fDegree * static_cast<float>((s_fPI / 180.0f));
  }

  inline maths::CVector3 DegreesToRadians(const maths::CVector3& _v3Degrees)
  {
    float fX = DegreesToRadians(_v3Degrees.X);
    float fY = DegreesToRadians(_v3Degrees.Y);
    float fZ = DegreesToRadians(_v3Degrees.Z);
    return maths::CVector3(fX, fY, fZ);
  }

  inline maths::CVector2 DegreesToRadians(const maths::CVector2& _v2Degrees)
  {
    float fX = DegreesToRadians(_v2Degrees.X);
    float fY = DegreesToRadians(_v2Degrees.Y);
    return maths::CVector2(fX, fY);
  }

  inline float RadiansToDegrees(float _fRadians)
  {
    return _fRadians * static_cast<float>((180.0f / s_fPI));
  }

  inline maths::CVector3 RadiansToDegrees(const maths::CVector3& _v3Delta)
  {
    float fX = RadiansToDegrees(_v3Delta.X);
    float fY = RadiansToDegrees(_v3Delta.Y);
    float fZ = RadiansToDegrees(_v3Delta.Z);
    return maths::CVector3(fX, fY, fZ);
  }

  inline maths::CVector2 RadiansToDegrees(const maths::CVector2& _v2Delta)
  {
    float fX = RadiansToDegrees(_v2Delta.X);
    float fY = RadiansToDegrees(_v2Delta.Y);
    return  maths::CVector2(fX, fY);
  }
}

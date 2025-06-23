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

  //@Note these functions have been extracted from real time collision detection book
  inline bool SeparateAxisTheorem
  (
    const std::vector<math::CVector3>& _vctExtentsA, const std::vector<math::CVector3>& _vctExtentsB,
    const math::CVector3& _v3Axis, math::CVector3& v3ImpactPoint_, math::CVector3& v3Normal_, float& fDepth_
  )
  {
    if (_v3Axis.IsZero())
      return false;

    float fMinA = FLT_MAX, fMaxA = -FLT_MAX;
    float fMinB = FLT_MAX, fMaxB = -FLT_MAX;

    // Get MinA/MaxA
    math::CVector3 v3MinA = math::CVector3::Zero;
    for (const math::CVector3& v3Vertex : _vctExtentsA)
    {
      float fDot = math::CVector3::Dot(v3Vertex, _v3Axis);
      if (fDot < fMinA) 
      { 
        v3MinA = v3Vertex; 
      }
      fMinA = math::Min(fMinA, fDot);
      fMaxA = math::Max(fMaxA, fDot);
    }

    // Get MinB/MaxB
    math::CVector3 v3MinB = math::CVector3::Zero;
    for (const math::CVector3& v3Vertex : _vctExtentsB)
    {
      float fDot = math::CVector3::Dot(v3Vertex, _v3Axis);
      if (fDot < fMinB) 
      {
        v3MinB = v3Vertex; 
      }
      fMinB = math::Min(fMinB, fDot);
      fMaxB = math::Max(fMaxB, fDot);
    }

    // Check separation
    if (fMinA > fMaxB || fMinB > fMaxA)
    {
      return true; // No collision
    }

    // Compute depth
    float fOverlapA = fMaxB - fMinA;
    float fOverlapB = fMaxA - fMinB;
    float fCurrentDepth = math::Min(fOverlapA, fOverlapB);

    // Update current depth
    if (fCurrentDepth < fDepth_)
    {
      // Set depth
      fDepth_ = fCurrentDepth;

      // Set normal
      v3Normal_ = (fDepth_ == fOverlapA) ? _v3Axis : -_v3Axis;

      // Calculate impact point
      math::CVector3 v3Offset = v3Normal_ * (fDepth_ * 0.5f);
      v3ImpactPoint_ = (fDepth_ == fOverlapA) ? v3MinA + v3Offset : v3MinB - v3Offset;
    }

    return false; // Valid
  }

  inline float SqDistPointSegment(math::CVector3 a, math::CVector3 b, math::CVector3 c)
  {
    math::CVector3 ab = b - a;
    math::CVector3 ac = c - a;
    math::CVector3 bc = c - b;

    float e = math::CVector3::Dot(ac, ab);
    // Handle cases where c projects outside ab
    if (e <= 0.0f) return math::CVector3::Dot(ac, ac);
    float f = math::CVector3::Dot(ab, ab);
    if (e >= f) return math::CVector3::Dot(bc, bc);
    // Handle cases where c projects onto ab
    return math::CVector3::Dot(ac, ac) - e * e / f;
  }

  inline float ClosestPtSegmentSegment(math::CVector3 p1, math::CVector3 q1, math::CVector3 p2, math::CVector3 q2,
  float& s, float& t, math::CVector3& c1, math::CVector3& c2)
  {
    math::CVector3 d1 = q1 - p1; // Direction vector of segment S1
    math::CVector3 d2 = q2 - p2; // Direction vector of segment S2
    math::CVector3 r = p1 - p2; 
    float a = math::CVector3::Dot(d1, d1); // Squared length of segment S1, always nonnegative
    float e = math::CVector3::Dot(d2, d2); // Squared length of segment S2, always nonnegative
    float f = math::CVector3::Dot(d2, r);
    // Check if either or both segments degenerate into points
    if (a <= s_fEpsilon3 && e <= s_fEpsilon3) 
    {
      // Both segments degenerate into points
      s = t = 0.0f;
      c1 = p1;
      c2 = p2;
      return math::CVector3::Dot(c1 - c2, c1 - c2);
    }
    if (a <= s_fEpsilon3) 
    {
      // First segment degenerates into a point
      s = 0.0f;
      t = f / e; // s = 0 => t = (b*s + f) / e = f / e
      t = Clamp(t, 0.0f, 1.0f);
    }
    else
    {
      float c = math::CVector3::Dot(d1, r);
      if (e <= s_fEpsilon3) 
      {
        // Second segment degenerates into a point
        t = 0.0f;
        s = Clamp(-c / a, 0.0f, 1.0f); // t = 0 => s = (b*t - c) / a = -c / a
      }
      else 
      {
        // The general nondegenerate case starts here
        float b = math::CVector3::Dot(d1, d2);
        float denom = a * e - b * b; // Always nonnegative
        // If segments not parallel, compute closest point on L1 to L2 and
        // clamp to segment S1. Else pick arbitrary s (here 0)
        if (denom != 0.0f) 
        {
          s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
        }
        else s = 0.0f;
        // Compute point on L2 closest to S1(s) using
        // t = Dot((P1 + D1*s) - P2,D2) / Dot(D2,D2) = (b*s + f) / e
        t = (b * s + f) / e;
        // If t in [0,1] done. Else clamp t, recompute s for the new value
        // of t using s = Dot((P2 + D2*t) - P1,D1) / Dot(D1,D1)= (t*b - c) / a
        // and clamp s to [0, 1]
        if (t < 0.0f) {
          t = 0.0f;
          s = Clamp(-c / a, 0.0f, 1.0f);
        }
        else if (t > 1.0f) {
          t = 1.0f;
          s = Clamp((b - c) / a, 0.0f, 1.0f);
        }
      }
    }
    c1 = p1 + d1 * s;
    c2 = p2 + d2 * t;
    return math::CVector3::Dot(c1 - c2, c1 - c2);
  }
}

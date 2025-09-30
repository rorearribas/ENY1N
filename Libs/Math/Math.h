#pragma once
#include "Engine/Utils/Plane.h"
#include "Engine/Utils/Ray.h"
#include "Vector2.h"
#include "Vector3.h"

namespace math
{
  // Static values
  static constexpr float s_fPI = 3.14159265358979323846f;
  static constexpr float s_fHalfPI = s_fPI / 2.0f;
  static constexpr float s_fMaxEuler = 360.0f;

  // Epsilon
  static constexpr float s_fEpsilon1 = 1e-1f;
  static constexpr float s_fEpsilon2 = 1e-2f;
  static constexpr float s_fEpsilon3 = 1e-3f;
  static constexpr float s_fEpsilon4 = 1e-4f;
  static constexpr float s_fEpsilon5 = 1e-5f;
  static constexpr float s_fEpsilon6 = 1e-6f;
  static constexpr float s_fEpsilon7 = 1e-7f;

  // Functions
  template <typename T>
  inline T Lerp(const T& _ObjectA, const T& _ObjectB, float _fDelta)
  {
    return _ObjectA + (_ObjectB - _ObjectA) * _fDelta;
  }

  template <typename T>
  inline T Clamp(const T& Value, const T& Min, const T& Max)
  {
    return Value < Min ? Min : Value > Max ? Max : Value;
  }

  template <typename T>
  inline T Min(const T& _Object1, const T& _Object2)
  {
    return _Object1 < _Object2 ? _Object1 : _Object2;
  }

  template <typename T>
  inline T Max(const T& _Object1, const T& _Object2)
  {
    return _Object1 > _Object2 ? _Object1 : _Object2;
  }

  inline float CalculateEulerAngle(float fAngle)
  {
    fAngle = fmod(fAngle, s_fMaxEuler);
    fAngle = (fAngle > s_fMaxEuler / 2.0f) ? (fAngle - s_fMaxEuler) : (fAngle < -(s_fMaxEuler / 2.0f) ? (fAngle + s_fMaxEuler) : fAngle);
    return fAngle;
  }

  inline float Deg2Radians(float _fDegrees)
  {
    return _fDegrees * static_cast<float>(s_fPI / 180.0f);
  }

  inline float Rad2Degrees(float _fRadians)
  {
    return _fRadians * static_cast<float>(180.0f / s_fPI);
  }

  inline math::CVector3 Deg2Radians(const math::CVector3& _v3)
  {
    return math::CVector3(Deg2Radians(_v3.X), Deg2Radians(_v3.Y), Deg2Radians(_v3.Z));
  }

  inline math::CVector2 Deg2Radians(const math::CVector2& _v2)
  {
    return math::CVector2(Deg2Radians(_v2.X), Deg2Radians(_v2.Y));
  }

  inline math::CVector3 Rad2Degrees(const math::CVector3& _v3)
  {
    return math::CVector3(Rad2Degrees(_v3.X), Rad2Degrees(_v3.Y), Rad2Degrees(_v3.Z));
  }

  inline math::CVector2 Rad2Degrees(const math::CVector2& _v2)
  {
    return math::CVector2(Rad2Degrees(_v2.X), Rad2Degrees(_v2.Y));
  }

  //@Note these functions have been extracted from real time collision detection book
  inline bool RayPlaneIntersection(math::CPlane& _oPlane, const physics::CRay& _oRay, math::CVector3& _v3IntersectPoint_)
  {
    // Get denominator
    float fDenom = math::CVector3::Dot(_oPlane.GetNormal(), _oRay.GetDir());
    if (fabs(fDenom) < s_fEpsilon6)
    {
      return false;
    }

    float fDot = math::CVector3::Dot(_oPlane.GetPos() - _oRay.GetOrigin(), _oPlane.GetNormal()) / fDenom;
    _v3IntersectPoint_ = _oRay.GetOrigin() + (_oRay.GetDir() * fDot);
    return fDot > 0.0f;
  }

  inline bool SeparateAxisTheorem
  (
    const std::vector<math::CVector3>& _vctPointsA, const std::vector<math::CVector3>& _vctPointsB,
    const math::CVector3& _v3Axis, math::CVector3& _v3ImpactPoint_, math::CVector3& _v3Normal_, float& _fDepth_
  )
  {
    if (_v3Axis.IsZero())
    {
      return false;
    }

    // Get A Values
    float fMinA = FLT_MAX, fMaxA = -FLT_MAX;
    math::CVector3 v3ClosestPointA = math::CVector3::Zero;
    for (const math::CVector3& v3Vertex : _vctPointsA)
    {
      float fDot = math::CVector3::Dot(v3Vertex, _v3Axis);
      if (fDot < fMinA)
      {
        v3ClosestPointA = v3Vertex;
      }
      fMinA = math::Min(fMinA, fDot);
      fMaxA = math::Max(fMaxA, fDot);
    }

    // Get B Values
    float fMinB = FLT_MAX, fMaxB = -FLT_MAX;
    math::CVector3 v3ClosestPointB = math::CVector3::Zero;
    for (const math::CVector3& v3Vertex : _vctPointsB)
    {
      float fDot = math::CVector3::Dot(v3Vertex, _v3Axis);
      if (fDot < fMinB)
      {
        v3ClosestPointB = v3Vertex;
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
    if (fCurrentDepth < _fDepth_)
    {
      // Set depth
      _fDepth_ = fCurrentDepth;

      // Set normal
      _v3Normal_ = (_fDepth_ == fOverlapA) ? _v3Axis : -_v3Axis;

      // Calculate impact point
      math::CVector3 v3Offset = _v3Normal_ * (_fDepth_ * 0.5f);
      _v3ImpactPoint_ = (_fDepth_ == fOverlapA) ? v3ClosestPointA + v3Offset : v3ClosestPointB - v3Offset;
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

  inline float ClosestPtSegmentSegment(const math::CVector3& p1, const math::CVector3& q1, const math::CVector3& p2, const math::CVector3& q2,
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

  inline float ClosestPtRaySegment(const physics::CRay& _oRay, const math::CVector3& p1, const math::CVector3& q1,
    float& s, float& t, math::CVector3& c1, math::CVector3& c2)
  {
    math::CVector3 d1 = _oRay.GetDir(); // Direction of ray
    math::CVector3 d2 = q1 - p1; // Direction of segment
    math::CVector3 r = _oRay.GetOrigin() - p1;

    float a = math::CVector3::Dot(d1, d1); // Should be > 0
    float e = math::CVector3::Dot(d2, d2); // Should be > 0
    float b = math::CVector3::Dot(d1, d2);
    float c = math::CVector3::Dot(d1, r);
    float f = math::CVector3::Dot(d2, r);

    float denom = a * e - b * b;

    // If not parallel
    if (fabs(denom) > s_fEpsilon3)
    {
      s = (b * f - c * e) / denom;
      s = Max(s, 0.0f); // Clamp ray to s >= 0 (it's a ray, not a line)
    }
    else
    {
      s = 0.0f; // Parallel: pick arbitrary point on ray
    }

    t = (b * s + f) / e;
    t = Clamp(t, 0.0f, 1.0f); // Clamp t to segment

    // Recompute s if t was clamped
    if (t <= 0.0f || t >= 1.0f)
    {
      t = Clamp(t, 0.0f, 1.0f);
      math::CVector3 cp = p1 + d2 * t;
      s = math::CVector3::Dot(cp - _oRay.GetOrigin(), d1) / a;
      s = Max(s, 0.0f); // Again, ray only
    }

    c1 = _oRay.GetOrigin() + d1 * s;
    c2 = p1 + d2 * t;

    return math::CVector3::SqrDist(c1, c2);
  }

  inline bool ClosestPtRaySphere(const physics::CRay& _oRay, const math::CVector3& _v3SphereCenter,
    const float _fSphereRadius, math::CVector3& _v3ClosestPoint_, float& _fDist_)
  {
    math::CVector3 v3Diff = math::CVector3(_oRay.GetOrigin() - _v3SphereCenter);
    float fDotA = math::CVector3::Dot(v3Diff, _oRay.GetDir());
    float fDotB = math::CVector3::Dot(v3Diff, v3Diff) - (_fSphereRadius * _fSphereRadius);

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
    if (fDotB > 0.0f && fDotA > 0.0f)
    {
      return false;
    }

    // A negative discriminant corresponds to ray missing sphere
    float fDiscr = fDotA * fDotA - fDotB;
    if (fDiscr < 0.0f)
    {
      return false;
    }

    // Compute distance
    _fDist_ = -fDotA - sqrt(fDiscr);
    _fDist_ = _fDist_ >= 0.0f ? _fDist_ : 0.0f; // Inside of sphere
    _v3ClosestPoint_ = _oRay.GetOrigin() + (_oRay.GetDir() * _fDist_);
    return true;
  }
}

#include "CapsuleCollider.h"
#include "Engine/Engine.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/Math/Math.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Libs/Math/Vector3.h"
#include <cassert>

namespace collision
{
  namespace internal_capsule_collider
  {
    static constexpr uint32_t s_uDebugSubv = 12;
  }

  bool CCapsuleCollider::CheckCollision(const CCollider& _oCollider, THitEvent& _oHitEvent_)
  {
    const EColliderType& eColliderType = _oCollider.GetType();
#ifdef _DEBUG
    assert(eColliderType != EColliderType::INVALID);
#endif // DEBUG
    switch (eColliderType)
    {
    case EColliderType::CAPSULE_COLLIDER:
    {
      const CCapsuleCollider& oCapsuleCollider = static_cast<const CCapsuleCollider&>(_oCollider);
#ifdef _DEBUG
      assert(&oCapsuleCollider);
#endif // DEBUG
      return CheckCapsuleCollision(&oCapsuleCollider, _oHitEvent_);
    }
    case EColliderType::BOX_COLLIDER:
    {
      const CBoxCollider& oBoxCollider = static_cast<const CBoxCollider&>(_oCollider);
#ifdef _DEBUG
      assert(&oBoxCollider);
#endif // DEBUG
      return oBoxCollider.IsOBB() ? CheckOBBCollision(&oBoxCollider, _oHitEvent_) : CheckBoxCollision(&oBoxCollider, _oHitEvent_);
    }
    case EColliderType::SPHERE_COLLIDER:
    {
      const CSphereCollider& oCapsuleCollider = static_cast<const CSphereCollider&>(_oCollider);
#ifdef _DEBUG
      assert(&oCapsuleCollider);
#endif // DEBUG
      return CheckSphereCollision(&oCapsuleCollider, _oHitEvent_);
    }
    }
    return false;
  }
  // ------------------------------------
  void CCapsuleCollider::RecalculateCollider()
  {
    // Calculate axis directors
    math::CMatrix4x4 mRot = math::CMatrix4x4::Rotation(GetRotation());
    math::CVector3 v3TargetAxis = mRot * m_v3OrientedAxis;

    // Set segment points
    float fSize = GetHeight() - (m_fRadius * 2.0f);
    float fHalfSize = fSize * 0.5f;

    const math::CVector3& v3WorldPos = GetWorldPos();
    m_v3EndSegmentPoint = v3WorldPos + (v3TargetAxis * fHalfSize);
    m_v3StartSegmentPoint = v3WorldPos - (v3TargetAxis * fHalfSize);
    m_v3SegmentDir = math::CVector3::Normalize(m_v3EndSegmentPoint - m_v3StartSegmentPoint);
  }
  // ------------------------------------
  void CCapsuleCollider::SetRadius(float _fRadius)
  {
    m_fRadius = _fRadius;
    RecalculateCollider();
  }
  // ------------------------------------
  void CCapsuleCollider::SetHeight(float _fHeight)
  {
    m_fHeight = _fHeight;
    RecalculateCollider();
  }
  // ------------------------------------
  void CCapsuleCollider::DrawDebug()
  {
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    const math::CVector3& v3CurrentPos = GetWorldPos(); // Get center
    const math::CVector3& v3CurrentRot = GetRotation(); // Get rotation

    pEngine->DrawSphere(m_v3EndSegmentPoint, 0.05f, 8, 8, math::CVector3::Forward, render::ERenderMode::WIREFRAME);
    pEngine->DrawSphere(m_v3StartSegmentPoint, 0.05f, 8, 8, math::CVector3::Right, render::ERenderMode::WIREFRAME);
    pEngine->DrawLine(m_v3EndSegmentPoint, m_v3StartSegmentPoint, math::CVector3::Up);

    pEngine->DrawCapsule
    (
      v3CurrentPos, // Pos
      v3CurrentRot, // Rot
      math::CVector3::One, // Color
      m_fRadius, // Radius
      m_fHeight, // Height
      internal_capsule_collider::s_uDebugSubv, // Horizontal
      internal_capsule_collider::s_uDebugSubv, // Vertical
      render::ERenderMode::WIREFRAME
    );
  }
  // ------------------------------------
  bool CCapsuleCollider::IntersectRay(const physics::CRay& _oRay, THitEvent& _oHitEvent_, const float& _fMaxDistance)
  {
    bool bCalculateSegment = true;
    float fClosestDist = FLT_MAX, fTempDist = FLT_MAX;
    math::CVector3 v3ClosestPoint, v3TempClosestPoint;

    // Check top sphere
    if (math::ClosestPtRaySphere(_oRay, GetEndSegmentPoint(), m_fRadius, v3TempClosestPoint, fTempDist) && fTempDist <= _fMaxDistance)
    {
      // Update closest point
      float fDot = math::CVector3::Dot(v3TempClosestPoint - GetEndSegmentPoint(), GetSegmentDir());
      if (fDot >= 0.0f && fTempDist <= fClosestDist)
      {
        fClosestDist = fTempDist;
        v3ClosestPoint = v3TempClosestPoint;
        bCalculateSegment = false;
      }
    }

    // Check bottom sphere
    if (math::ClosestPtRaySphere(_oRay, GetStartSegmentPoint(), m_fRadius, v3TempClosestPoint, fTempDist) && fTempDist <= _fMaxDistance)
    {
      // Update closest point
      float fDot = math::CVector3::Dot(GetStartSegmentPoint() - v3TempClosestPoint, GetSegmentDir());
      if (fDot >= 0.0f && fTempDist <= fClosestDist)
      {
        fClosestDist = fTempDist;
        v3ClosestPoint = v3TempClosestPoint;
        bCalculateSegment = false;
      }
    }

    // Check segment!
    if (bCalculateSegment)
    {
      float s = 0.0f, t = 0.0f;
      math::CVector3 v3ClosestToRay, v3ClosestToSegment;
      float fSqrDist = math::ClosestPtRaySegment(_oRay, GetStartSegmentPoint(), GetEndSegmentPoint(), s, t, v3ClosestToRay, v3ClosestToSegment);
      if (fSqrDist <= (GetRadius() * GetRadius()) && s >= 0.0f && s <= _fMaxDistance)
      {
        math::ClosestPtRaySphere(_oRay, v3ClosestToSegment, m_fRadius, v3TempClosestPoint, fTempDist);
        v3ClosestPoint = v3TempClosestPoint;
      }
      else
      {
        return false;
      }
    }

    //fDepth = math::Clamp((fSqrRadius - fDist) * 2.0f, 0.0f, fRadius);
    //math::CVector3 v3Diff /*= v3ClosestToSegment - v3ClosestToRay*/;
    //math::CVector3 v3Normal = v3Diff.Normalize();
    ///* float fDot = math::CVector3::Dot(v3Diff, v3Normal);
    // UNUSED_VAR(fDot);*/

   /* _oHitEvent_.Normal = v3Normal;
    _oHitEvent_.Depth = fDepth;*/
    _oHitEvent_.ImpactPoint = v3ClosestPoint;
    _oHitEvent_.Object = GetOwner();
    return true;
  }
  // ------------------------------------
  bool CCapsuleCollider::CheckCapsuleCollision(const CCapsuleCollider* _pOther, THitEvent& _oHitEvent_) const
  {
    const math::CVector3 v3Center = GetWorldPos();
    math::CVector3 v3Offset = v3Center - _pOther->GetWorldPos();
    math::CVector3 v3Dir = math::CVector3::Normalize(v3Offset);

    float s = 0.0f, t = 0.0f;
    math::CVector3 v3OutPointA, v3OutPointB;
    float fDist = math::ClosestPtSegmentSegment
    (
      GetStartSegmentPoint(), GetEndSegmentPoint(),
      _pOther->GetStartSegmentPoint(), _pOther->GetEndSegmentPoint(),
      s, t, v3OutPointA, v3OutPointB
    );
    float fRadiusSum = GetRadius() + _pOther->GetRadius();
    float fRadiusSquared = fRadiusSum * fRadiusSum;

    if (fDist <= fRadiusSquared)
    {
      _oHitEvent_.Normal = v3Dir;
      _oHitEvent_.Depth = fRadiusSum - sqrtf(fDist);
      _oHitEvent_.ImpactPoint = v3Center + (_oHitEvent_.Normal * GetRadius());
      return true;
    }

    return false;
  }
  // ------------------------------------
  bool CCapsuleCollider::CheckOBBCollision(const CBoxCollider* _pOther, THitEvent& _oHitEvent_) const
  {
    float fCapsuleRadius = GetRadius();
    math::CVector3 v3HalfSize = _pOther->GetHalfSize();
    std::vector<math::CVector3> v3AxisDirectors = _pOther->GetAxisDirectors();

    // Values
    float fMinDist = FLT_MAX;
    bool bCollision = false;

    static constexpr uint32_t uMaxOBBIterations = 16; // Max iterations between segments points
    for (uint32_t uI = 0; uI <= uMaxOBBIterations; uI++)
    {
      float fDelta = (static_cast<float>(uI) / static_cast<float>(uMaxOBBIterations));
      math::CVector3 v3SegmentPoint = math::Lerp(GetStartSegmentPoint(), GetEndSegmentPoint(), fDelta);
      math::CVector3 v3Diff = v3SegmentPoint - _pOther->GetCenter();

      // Calculate closest point
      math::CVector3 v3ClosestPoint = _pOther->GetCenter();
      for (uint32_t uJ = 0; uJ < v3AxisDirectors.size(); uJ++)
      {
        const math::CVector3& v3Axis = v3AxisDirectors[uJ];
        float fDot = math::CVector3::Dot(v3Diff, v3Axis);
        fDot = math::Clamp(fDot, -v3HalfSize[uJ], v3HalfSize[uJ]);
        v3ClosestPoint += v3Axis * fDot;
      }

      v3Diff = v3SegmentPoint - v3ClosestPoint;
      float fDist = v3Diff.Magnitude();
      if ((fDist <= fCapsuleRadius) && (fDist <= fMinDist))
      {
        _oHitEvent_.Depth = fCapsuleRadius - fDist;
        _oHitEvent_.Normal = math::CVector3::Normalize(v3Diff);
        _oHitEvent_.ImpactPoint = v3ClosestPoint;

        fMinDist = fDist;
        bCollision = true;
      }
    }
    return bCollision;
  }
  // ------------------------------------
  bool CCapsuleCollider::CheckBoxCollision(const CBoxCollider* _pOther, THitEvent& _oHitEvent_) const
  {
    //@Note: Review this!
    float fDist = math::SqDistPointSegment(GetEndSegmentPoint(), GetStartSegmentPoint(), _pOther->GetCenter());
    float fCapsuleWidth = GetRadius() + GetRadius();
    if (fDist <= fCapsuleWidth)
    {
      _oHitEvent_.Normal = math::CVector3::Normalize(GetWorldPos() - _pOther->GetCenter());
      _oHitEvent_.Depth = fCapsuleWidth - sqrtf(fDist);
      _oHitEvent_.ImpactPoint = GetWorldPos() + (_oHitEvent_.Normal * GetRadius());
      return true;
    }
    return false;
  }
  // ------------------------------------
  bool CCapsuleCollider::CheckSphereCollision(const CSphereCollider* _pOther, THitEvent& _oHitEvent_) const
  {
    // Compute (squared) distance between sphere center and capsule line segment
    float fDist = math::SqDistPointSegment(GetEndSegmentPoint(), GetStartSegmentPoint(), _pOther->GetCenter());
    // If (squared) distance smaller than (squared) sum of radii, they collide
    float fRadiusSum = _pOther->GetRadius() + this->GetRadius();
    float fRadiusSquared = fRadiusSum * fRadiusSum;

    if (fDist <= fRadiusSquared)
    {
      const math::CVector3 v3Center = GetWorldPos();
      math::CVector3 v3Offset = v3Center - _pOther->GetCenter();
      math::CVector3 v3Dir = math::CVector3::Normalize(v3Offset);

      _oHitEvent_.Normal = v3Dir;
      _oHitEvent_.Depth = fRadiusSum - sqrtf(fDist);
      _oHitEvent_.ImpactPoint = v3Center + (_oHitEvent_.Normal * GetRadius());

      return true;
    }

    return false;
  }
}
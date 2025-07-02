#include "CapsuleCollider.h"
#include "Engine/Base/Engine.h"
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
    const int s_iMaxSubvH(12);
    const int s_iMaxSubvV(12);
  }

  bool CCapsuleCollider::CheckCollision(const CCollider& _oCollider, SHitEvent& _oHitEvent_)
  {
    const EColliderType& eColliderType = _oCollider.GetType();
    assert(eColliderType != EColliderType::INVALID);
    switch (eColliderType)
    {
    case EColliderType::CAPSULE_COLLIDER:
    {
      const CCapsuleCollider& oCapsuleCollider = static_cast<const CCapsuleCollider&>(_oCollider);
      assert(&oCapsuleCollider);
      return CheckCapsuleCollision(&oCapsuleCollider, _oHitEvent_);
    }
    case EColliderType::BOX_COLLIDER:
    {
      const CBoxCollider& oBoxCollider = static_cast<const CBoxCollider&>(_oCollider);
      assert(&oBoxCollider);
      return oBoxCollider.IsOBBEnabled() ? CheckOBBCollision(&oBoxCollider, _oHitEvent_) : CheckBoxCollision(&oBoxCollider, _oHitEvent_);
    }
    case EColliderType::SPHERE_COLLIDER:
    {
      const CSphereCollider& oCapsuleCollider = static_cast<const CSphereCollider&>(_oCollider);
      assert(&oCapsuleCollider);
      return CheckSphereCollision(&oCapsuleCollider, _oHitEvent_);
    }
    }
    return false;
  }
  // ------------------------------------
  bool CCapsuleCollider::IntersectRay(const physics::CRay& /*_oRay*/, SHitEvent& /*_oHitEvent_*/, const float& /*_fMaxDistance*/)
  {
    return false;
  }
  // ------------------------------------
  void CCapsuleCollider::RecalculateCollider()
  {
    // Calculate axis directors
    math::CMatrix4x4 mRot = math::CMatrix4x4::Rotation(GetRotation());
    mRot = math::CMatrix4x4::Transpose(mRot);
    math::CVector3 v3TargetAxis = mRot * m_v3OrientedAxis;

    // Set segment points
    float fSize = GetHeight() - (m_fRadius * 2.0f);
    float fHalfSize = fSize * 0.5f;

    m_v3PointA = GetWorldPos() + (v3TargetAxis * fHalfSize);
    m_v3PointB = GetWorldPos() - (v3TargetAxis * fHalfSize);
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

    pEngine->DrawSphere(m_v3PointA, 0.125f, 8, 8, math::CVector3::Forward);
    pEngine->DrawSphere(m_v3PointB, 0.125f, 8, 8, math::CVector3::Right);
    pEngine->DrawLine(m_v3PointA, m_v3PointB, math::CVector3::Up);

    pEngine->DrawCapsule
    (
      v3CurrentPos, // Pos
      v3CurrentRot, // Rot
      math::CVector3::One, // Color
      m_fRadius, // Radius
      m_fHeight, // Height
      internal_capsule_collider::s_iMaxSubvH, // Horizontal
      internal_capsule_collider::s_iMaxSubvV // Vertical
    );
  }
  // ------------------------------------
  bool CCapsuleCollider::CheckCapsuleCollision(const CCapsuleCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    const math::CVector3 v3Center = GetWorldPos();
    math::CVector3 v3Offset = v3Center - _pOther->GetWorldPos();
    math::CVector3 v3Dir = math::CVector3::Normalize(v3Offset);

    float s = 0.0f, t = 0.0f;
    math::CVector3 v3OutPointA, v3OutPointB;
    float fDist = math::ClosestPtSegmentSegment(GetPointA(), GetPointB(), _pOther->GetPointA(), _pOther->GetPointB(), s, t, v3OutPointA, v3OutPointB);
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
  bool CCapsuleCollider::CheckOBBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    float fCapsuleRadius = GetRadius();
    math::CVector3 v3HalfSize = _pOther->GetHalfSize();
    std::vector<math::CVector3> v3AxisDirectors = _pOther->GetAxisDirectors();

    // Values
    float fMinDist = FLT_MAX;
    bool bCollision = false;

    static constexpr uint32_t uMaxIterations = 16; // Max iterations between segments points
    for (uint32_t uI = 0; uI <= uMaxIterations; uI++)
    {
      float fDelta = (static_cast<float>(uI) / static_cast<float>(uMaxIterations));
      math::CVector3 v3SegmentPoint = math::Lerp(GetPointA(), GetPointB(), fDelta);
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
  bool CCapsuleCollider::CheckBoxCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    //@Note: Review this!
    float fDist = math::SqDistPointSegment(GetPointA(), GetPointB(), _pOther->GetCenter());
    float fCapsuleWidth = GetRadius() + GetRadius();
    if (fDist <= fCapsuleWidth)
    {
      _oHitEvent_.Normal = (GetWorldPos() - _pOther->GetCenter()).Normalize();
      _oHitEvent_.Depth = fCapsuleWidth - sqrtf(fDist);
      _oHitEvent_.ImpactPoint = GetWorldPos() + (_oHitEvent_.Normal * GetRadius());
      return true;
    }
    return false;
  }
  // ------------------------------------
  bool CCapsuleCollider::CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    // Compute (squared) distance between sphere center and capsule line segment
    float fDist = math::SqDistPointSegment(GetPointA(), GetPointB(), _pOther->GetCenter());
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
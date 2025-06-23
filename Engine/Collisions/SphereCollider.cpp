#include "SphereCollider.h"
#include "Engine/Base/Engine.h"
#include "BoxCollider.h"
#include "Libs/Math/Math.h"
#include "Libs/Macros/GlobalMacros.h"

namespace collision
{
  namespace internal_sphere_collider
  {
    static const int s_iMaxSubvH(8);
    static const int s_iMaxSubvV(8);
  }

  bool CSphereCollider::CheckCollision(const CCollider& _oCollider, SHitEvent& _oHitEvent_)
  {
    const EColliderType& eColliderType = _oCollider.GetType();
    assert(eColliderType != EColliderType::INVALID);

    switch (eColliderType)
    {
    case EColliderType::BOX_COLLIDER:
    {
      const CBoxCollider& oBoxCollider = static_cast<const CBoxCollider&>(_oCollider);
      assert(&oBoxCollider);
      return oBoxCollider.IsOBBEnabled() ? CheckOBBCollision(&oBoxCollider, _oHitEvent_) : CheckAABBCollision(&oBoxCollider, _oHitEvent_);
    }
    case EColliderType::SPHERE_COLLIDER:
    {
      const CSphereCollider& oSphereCollider = static_cast<const CSphereCollider&>(_oCollider);
      assert(&oSphereCollider);
      return CheckSphereCollision(&oSphereCollider, _oHitEvent_);
    }
    }
    return false;
  }
  // ------------------------------------
  bool CSphereCollider::IntersectRay(const physics::CRay& _oRay, SHitEvent& _oHitEvent_, const float& _fMaxDistance)
  {
    math::CVector3 v3Diff = math::CVector3(_oRay.GetOrigin() - m_v3Center);
    float fDotA = math::CVector3::Dot(v3Diff, _oRay.GetDir());
    float fDotB = math::CVector3::Dot(v3Diff, v3Diff) - (this->m_fRadius * this->m_fRadius);

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

    // Get distance
    float fDist = -fDotA - sqrt(fDiscr);
    fDist = math::Clamp(fDist, 0.0f, _fMaxDistance + math::s_fEpsilon3);
    if (fDist >= (_fMaxDistance + math::s_fEpsilon3))
    {
      return false;
    }

    _oHitEvent_.Object = GetOwner();
    _oHitEvent_.ImpactPoint = _oRay.GetOrigin() + (_oRay.GetDir() * fDist);
    _oHitEvent_.Normal = math::CVector3::Normalize(_oHitEvent_.ImpactPoint - m_v3Center);
    _oHitEvent_.Distance = fDist;

    return true;
  }
  // ------------------------------------
  void CSphereCollider::RecalculateCollider()
  {
    // Set world center
    m_v3Center = GetPosition();
  }
  // ------------------------------------
  bool CSphereCollider::CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    const math::CVector3 v3Center = GetCenter();
    math::CVector3 v3Offset = v3Center - _pOther->GetCenter();
    math::CVector3 v3Dir = math::CVector3::Normalize(v3Offset);

    // Calculate values
    float fDistanceSquared = (v3Offset.X * v3Offset.X) + (v3Offset.Y * v3Offset.Y) + (v3Offset.Z * v3Offset.Z);
    float fRadiusSum = GetRadius() + _pOther->GetRadius();
    float fRadiusSquared = fRadiusSum * fRadiusSum;

    // Valid collision
    if (fDistanceSquared <= fRadiusSquared)
    {
      _oHitEvent_.Normal = v3Dir;
      _oHitEvent_.Depth = fRadiusSum - sqrtf(fDistanceSquared);
      _oHitEvent_.ImpactPoint = v3Center + (_oHitEvent_.Normal * GetRadius());
      return true;
    }

    return false;
  }
  // ------------------------------------
  bool CSphereCollider::CheckOBBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    // Get OBB data
    const math::CVector3& v3OBBCenter = _pOther->GetCenter();
    const math::CVector3 v3HalfSize = _pOther->GetHalfSize();
    const std::vector<math::CVector3> v3Axis = _pOther->GetAxisDirectors();

    // Calculate dir
    const math::CVector3& v3SphereCenter = GetCenter();
    math::CVector3 v3Offset = v3SphereCenter - v3OBBCenter;

    // Project dir using axis directors from box collider
    float fProjX = math::CVector3::Dot(v3Offset, v3Axis[0]); // Axis X
    float fProjY = math::CVector3::Dot(v3Offset, v3Axis[1]); // Axis Y
    float fProjZ = math::CVector3::Dot(v3Offset, v3Axis[2]); // Axis Z

    // Clamp axis
    float fClampedX = math::Clamp(fProjX, -v3HalfSize.X, v3HalfSize.X);
    float fClampedY = math::Clamp(fProjY, -v3HalfSize.Y, v3HalfSize.Y);
    float fClampedZ = math::Clamp(fProjZ, -v3HalfSize.Z, v3HalfSize.Z);

    // Compute closest point
    math::CVector3 v3ClosestPoint =
    {
      (v3OBBCenter + v3Axis[0] * fClampedX) + // X
      (v3Axis[1] * fClampedY) + // Y
      (v3Axis[2] * fClampedZ) // Z
    };

    // Check distance
    float fRadius = GetRadius();
    float fDistance = math::CVector3::Distance(v3ClosestPoint, v3SphereCenter);
    if (fDistance <= fRadius)
    {
      _oHitEvent_.ImpactPoint = v3ClosestPoint;
      _oHitEvent_.Depth = std::abs(fRadius - fDistance);
      _oHitEvent_.Normal = math::CVector3::Normalize(v3SphereCenter - v3ClosestPoint);
      return true;
    }
    return false;
  }
  // ------------------------------------
  bool CSphereCollider::CheckAABBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    // We have to find the closest point.
    const math::CVector3& v3Center = GetCenter();
    float fClosestX = math::Max(_pOther->GetMin().X, math::Min(v3Center.X, _pOther->GetMax().X));
    float fClosestY = math::Max(_pOther->GetMin().Y, math::Min(v3Center.Y, _pOther->GetMax().Y));
    float fClosestZ = math::Max(_pOther->GetMin().Z, math::Min(v3Center.Z, _pOther->GetMax().Z));

    // We have to calculate the squared distance between the center of the sphere and the nearest point.
    float fSquaredDist = (fClosestX - v3Center.X) * (fClosestX - v3Center.X) +
      (fClosestY - v3Center.Y) * (fClosestY - v3Center.Y) +
      (fClosestZ - v3Center.Z) * (fClosestZ - v3Center.Z);

    // We check if the distance is less than or equal to the radius squared of the sphere.
    const float& fRadius = GetRadius();
    float fSquareRadius = fRadius * fRadius;
    if (fSquaredDist <= fSquareRadius)
    {
      math::CVector3 v3ImpactPoint(fClosestX, fClosestY, fClosestZ);
      _oHitEvent_.ImpactPoint = v3ImpactPoint;
      _oHitEvent_.Depth = fRadius - sqrt(fSquaredDist);
      _oHitEvent_.Normal = math::CVector3::Normalize(v3Center - v3ImpactPoint);
      return true;
    }
    return false;
  }
  // ------------------------------------
  void CSphereCollider::DrawDebug()
  {
    // Draw sphere
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    pEngine->DrawSphere
    (
      GetPosition(),
      GetRadius(),
      internal_sphere_collider::s_iMaxSubvH,
      internal_sphere_collider::s_iMaxSubvV,
      math::CVector3::One
    );
  }
}

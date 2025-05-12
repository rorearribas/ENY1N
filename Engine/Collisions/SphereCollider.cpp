#include "SphereCollider.h"
#include "BoxCollider.h"
#include <cassert>
#include <algorithm>
#include "Libs/Maths/Maths.h"
#include <iostream>
#include "Libs/Macros/GlobalMacros.h"

namespace collisions
{
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
      return oBoxCollider.IsOBBEnabled() ? CheckOBBCollision(&oBoxCollider, _oHitEvent_) : CheckBoxCollision(&oBoxCollider, _oHitEvent_);
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
  void CSphereCollider::RecalculateCollider()
  {
    // Set world center
    m_v3Center = GetPosition();
  }
  // ------------------------------------
  bool CSphereCollider::CheckOBBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    // Get OBB data
    const maths::CVector3& v3OBBCenter = _pOther->GetCenter();
    const maths::CVector3 v3HalfSize = _pOther->GetHalfSize();
    const std::vector<maths::CVector3> v3Axis = _pOther->GetAxisDirectors(); 

    // Calculate dir
    const maths::CVector3& v3SphereCenter = GetCenter();
    maths::CVector3 v3Dir = v3SphereCenter - v3OBBCenter;

    // Project dir using axis directors from box collider
    float fProjX = maths::CVector3::Dot(v3Dir, v3Axis[0]); // Axis X
    float fProjY = maths::CVector3::Dot(v3Dir, v3Axis[1]); // Axis Y
    float fProjZ = maths::CVector3::Dot(v3Dir, v3Axis[2]); // Axis Z

    // Clamp axis
    float fClampedX = maths::Clamp(fProjX, -v3HalfSize.X, v3HalfSize.X);
    float fClampedY = maths::Clamp(fProjY, -v3HalfSize.Y, v3HalfSize.Y);
    float fClampedZ = maths::Clamp(fProjZ, -v3HalfSize.Z, v3HalfSize.Z);

    // Compute closest point
    maths::CVector3 v3ClosestPoint = (v3OBBCenter + v3Axis[0] * fClampedX) + (v3Axis[1] * fClampedY) + (v3Axis[2] * fClampedZ);

    // Check distance
    float fRadius = GetRadius();
    float fDistance = maths::CVector3::Distance(v3ClosestPoint, v3SphereCenter);
    if (fDistance <= fRadius)
    {
      _oHitEvent_.ImpactPoint = v3ClosestPoint;
      _oHitEvent_.Depth = std::abs(fRadius - fDistance);
      _oHitEvent_.Normal = maths::CVector3::Normalize(v3SphereCenter - v3ClosestPoint);
      return true;
    }
    return false;
  }
  // ------------------------------------
  bool CSphereCollider::CheckBoxCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    // We have to find the closest point.
    const maths::CVector3& v3Center = GetCenter();
    float fClosestX = maths::Max(_pOther->GetMin().X, maths::Min(v3Center.X, _pOther->GetMax().X));
    float fClosestY = maths::Max(_pOther->GetMin().Y, maths::Min(v3Center.Y, _pOther->GetMax().Y));
    float fClosestZ = maths::Max(_pOther->GetMin().Z, maths::Min(v3Center.Z, _pOther->GetMax().Z));

    // We have to calculate the squared distance between the center of the sphere and the nearest point.
    float fSquaredDist = (fClosestX - v3Center.X) * (fClosestX - v3Center.X) +
      (fClosestY - v3Center.Y) * (fClosestY - v3Center.Y) +
      (fClosestZ - v3Center.Z) * (fClosestZ - v3Center.Z);

    // We check if the distance is less than or equal to the radius squared of the sphere.
    const float& fRadius = GetRadius();
    float fSquareRadius = fRadius * fRadius;
    if (fSquaredDist <= fSquareRadius)
    {
      maths::CVector3 v3ImpactPoint(fClosestX, fClosestY, fClosestZ);
      _oHitEvent_.ImpactPoint = v3ImpactPoint;
      _oHitEvent_.Depth = fRadius - sqrt(fSquaredDist);
      _oHitEvent_.Normal = maths::CVector3::Normalize(v3Center - v3ImpactPoint);
      return true;
    }
    return false;
  }
  // ------------------------------------
  bool CSphereCollider::CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    const maths::CVector3 v3Center = GetCenter();
    maths::CVector3 vDist = v3Center - _pOther->GetCenter();
    maths::CVector3 v3Dir = maths::CVector3::Normalize(vDist);

    // Calculate values
    float fDistanceSquared = (vDist.X * vDist.X) + (vDist.Y * vDist.Y) + (vDist.Z * vDist.Z);
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
}

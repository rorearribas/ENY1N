#include "SphereCollider.h"
#include "BoxCollider.h"
#include <cassert>
#include <algorithm>

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
      return CheckBoxCollision(&oBoxCollider, _oHitEvent_);
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
    m_v3Center = GetPosition();
  }
  // ------------------------------------
  bool CSphereCollider::CheckBoxCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    // We have to find the closest point.
    float fClosestX = std::max(_pOther->GetMin().X, std::min(GetCenter().X, _pOther->GetMax().X));
    float fClosestY = std::max(_pOther->GetMin().Y, std::min(GetCenter().Y, _pOther->GetMax().Y));
    float fClosestZ = std::max(_pOther->GetMin().Z, std::min(GetCenter().Z, _pOther->GetMax().Z));

    // We calculate the squared distance between the center of the sphere and the nearest point.
    float fDist = (fClosestX - GetCenter().X) * (fClosestX - GetCenter().X) +
      (fClosestY - GetCenter().Y) * (fClosestY - GetCenter().Y) +
      (fClosestZ - GetCenter().Z) * (fClosestZ - GetCenter().Z);

    // We check if the distance is less than or equal to the radius squared of the sphere.
    float fSquareRadius = GetRadius() * GetRadius();
    if (fDist <= fSquareRadius)
    {
      maths::CVector3 vImpactPoint(fClosestX, fClosestY, fClosestZ);
      _oHitEvent_.ImpactPoint = vImpactPoint;
      _oHitEvent_.Depth = fSquareRadius - fDist;
      _oHitEvent_.Normal = (vImpactPoint - GetCenter()).Normalize();
      return true;
    }
    return false;
  }
  // ------------------------------------
  bool CSphereCollider::CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    maths::CVector3 vDist = _pOther->GetCenter() - GetCenter();
    maths::CVector3 vNormalize = vDist.Normalize();

    float fDistanceSquared = (vDist.X * vDist.X) + (vDist.Y * vDist.Y) + (vDist.Z * vDist.Z);
    float fRadiusSum = GetRadius() + _pOther->GetRadius();
    float fRadiusSquared = fRadiusSum * fRadiusSum;

    // Valid collision
    if (fDistanceSquared <= fRadiusSquared)
    {
      float fDistance = sqrt(fDistanceSquared);
      if (fDistance > 0.0f)
      {
        _oHitEvent_.Normal = vNormalize;
      }
      _oHitEvent_.Depth = fRadiusSum - fDistance;
      _oHitEvent_.ImpactPoint = GetCenter() + (_oHitEvent_.Normal * _oHitEvent_.Depth);
      return true;
    }

    return false;
  }
}

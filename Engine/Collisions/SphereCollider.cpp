#include "SphereCollider.h"
#include "BoxCollider.h"
#include <cassert>
#include <algorithm>

namespace collisions
{
  bool CSphereCollider::CheckCollision(const CCollider& _oCollider)
  {
    const EColliderType& eColliderType = _oCollider.GetType();
    assert(eColliderType != EColliderType::INVALID);

    switch (eColliderType)
    {
    case EColliderType::BOX_COLLIDER:
    {
      const CBoxCollider& oBoxCollider = static_cast<const CBoxCollider&>(_oCollider);
      assert(&oBoxCollider);
      return CheckBoxCollision(&oBoxCollider);
    }
    case EColliderType::SPHERE_COLLIDER:
    {
      const CSphereCollider& oSphereCollider = static_cast<const CSphereCollider&>(_oCollider);
      assert(&oSphereCollider);
      return CheckSphereCollision(&oSphereCollider);
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
  bool CSphereCollider::CheckBoxCollision(const CBoxCollider* _pOther) const
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
    return fDist <= GetRadius() * GetRadius();
  }
  // ------------------------------------
  bool CSphereCollider::CheckSphereCollision(const CSphereCollider* _pOther) const
  {
    float fDist = maths::CVector3::Distance(_pOther->GetCenter(), GetCenter());
    float fRadiusSquared = (GetRadius() + _pOther->GetRadius()) * (GetRadius() + _pOther->GetRadius());
    return fDist <= fRadiusSquared;
  }
}

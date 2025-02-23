#include "BoxCollider.h"
#include <cassert>
#include "SphereCollider.h"
#include <algorithm>

namespace physics
{
  bool CBoxCollider::CheckCollision(const CCollider& _oCollider)
  {
    const EColliderType& eColliderType = _oCollider.GetType();
    assert(eColliderType == EColliderType::INVALID);

    switch (_oCollider.GetType())
    {
    case EColliderType::BOX_COLLIDER:
    {
      const CBoxCollider& oCollider = static_cast<const CBoxCollider&>(_oCollider);
      assert(&oCollider);
      return CheckBoxCollision(oCollider);
    }
    case EColliderType::SPHERE_COLLIDER:
    {
      const CSphereCollider& oCollider = static_cast<const CSphereCollider&>(_oCollider);
      assert(&oCollider);
      return CheckSphereCollision(oCollider);
    }
    default:
      return false;
    }
  }
  // ------------------------------------
  bool CBoxCollider::CheckBoxCollision(const CBoxCollider& _other)
  {
    // AABB collision
    return (m_vMin.X <= _other.m_vMax.X && m_vMax.X >= _other.m_vMin.X) &&
      (m_vMin.Y <= _other.m_vMax.Y && m_vMax.Y >= _other.m_vMin.Y) &&
      (m_vMin.Z <= _other.m_vMax.Z && m_vMax.Z >= _other.m_vMin.Z);
  }
  // ------------------------------------
  bool CBoxCollider::CheckSphereCollision(const CSphereCollider& _other)
  {
    // We have to find the closest point.
    float fClosestX = std::max(m_vMin.X, std::min(_other.GetCenter().X, m_vMax.X));
    float fClosestY = std::max(m_vMin.Y, std::min(_other.GetCenter().Y, m_vMax.Y));
    float fClosestZ = std::max(m_vMin.Z, std::min(_other.GetCenter().Z, m_vMax.Z));

    // We calculate the squared distance between the center of the sphere and the nearest point.
    float fDist = (fClosestX - _other.GetCenter().X) * (fClosestX - _other.GetCenter().X) +
      (fClosestY - _other.GetCenter().Y) * (fClosestY - _other.GetCenter().Y) +
      (fClosestZ - _other.GetCenter().Z) * (fClosestZ - _other.GetCenter().Z);

    // We check if the distance is less than or equal to the radius squared of the sphere.
    return fDist <= (_other.GetRadius() * _other.GetRadius());
  }

}
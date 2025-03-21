#include "BoxCollider.h"
#include <cassert>
#include "SphereCollider.h"
#include <algorithm>
#include <iostream>

namespace collisions
{
  bool CBoxCollider::CheckCollision(const CCollider& _oCollider, SHitEvent& _oHitEvent_)
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
  void CBoxCollider::RecalculateCollider()
  {
    ComputeMinMax();
    ComputeExtents();
  }
  // ------------------------------------
  void CBoxCollider::SetSize(const maths::CVector3& _v3Size)
  {
    m_vSize = _v3Size;
    RecalculateCollider();
  }
  // ------------------------------------
  bool CBoxCollider::CheckBoxCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    bool bCollision = (m_vMin.X <= _pOther->m_vMax.X && m_vMax.X >= _pOther->m_vMin.X) &&
      (m_vMin.Y <= _pOther->m_vMax.Y && m_vMax.Y >= _pOther->m_vMin.Y) &&
      (m_vMin.Z <= _pOther->m_vMax.Z && m_vMax.Z >= _pOther->m_vMin.Z);

    if (bCollision)
    {
      // Not implemented yet
      return true;
    }

    return false;
  }
  // ------------------------------------
  bool CBoxCollider::CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    // We have to find the closest point.
    float fClosestX = std::max(m_vMin.X, std::min(_pOther->GetCenter().X, m_vMax.X));
    float fClosestY = std::max(m_vMin.Y, std::min(_pOther->GetCenter().Y, m_vMax.Y));
    float fClosestZ = std::max(m_vMin.Z, std::min(_pOther->GetCenter().Z, m_vMax.Z));

    // We calculate the squared distance between the center of the sphere and the nearest point.
    float fDist = (fClosestX - _pOther->GetCenter().X) * (fClosestX - _pOther->GetCenter().X) +
      (fClosestY - _pOther->GetCenter().Y) * (fClosestY - _pOther->GetCenter().Y) +
      (fClosestZ - _pOther->GetCenter().Z) * (fClosestZ - _pOther->GetCenter().Z);

    // We check if the distance is less than or equal to the radius squared of the sphere.
    float fSquareRadius = _pOther->GetRadius() * _pOther->GetRadius();
    if (fDist <= fSquareRadius)
    {
      maths::CVector3 vImpactPoint(fClosestX, fClosestY, fClosestZ);
      _oHitEvent_.ImpactPoint = vImpactPoint;
      _oHitEvent_.Depth = fSquareRadius - fDist;
      _oHitEvent_.Normal = (_pOther->GetCenter() - vImpactPoint).Normalize();
      return true;
    }
    return false;
  }
  // ------------------------------------
  void CBoxCollider::ComputeExtents()
  {
    m_v3Extents =
    {
      m_vMin,
      maths::CVector3(m_vMin.X, m_vMin.Y, m_vMax.Z),
      maths::CVector3(m_vMin.X, m_vMax.Y, m_vMin.Z),
      maths::CVector3(m_vMin.X, m_vMax.Y, m_vMax.Z),
      maths::CVector3(m_vMax.X, m_vMin.Y, m_vMin.Z),
      maths::CVector3(m_vMax.X, m_vMin.Y, m_vMax.Z),
      maths::CVector3(m_vMax.X, m_vMax.Y, m_vMin.Z),
      m_vMax
    };
  }
  // ------------------------------------
  void CBoxCollider::ComputeMinMax()
  {
    m_vMin = GetPosition() - (m_vSize * 0.5f);
    m_vMax = GetPosition() + (m_vSize * 0.5f);
  }
}
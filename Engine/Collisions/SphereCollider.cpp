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
    UNUSED_VARIABLE(_oHitEvent_);

    // Data about OBB
    const std::vector<maths::CVector3>& v3Extents = _pOther->GetExtents();
    UNUSED_VARIABLE(v3Extents);
    std::vector<maths::CVector3> v3AxisDirectors = _pOther->GetAxisDirectors();

    maths::CVector3 v3OBBCenter = _pOther->GetCenter();
    const maths::CVector3& v3SphereCenter = GetCenter();
    UNUSED_VARIABLE(v3SphereCenter);

    maths::CVector3 v3Dir = maths::CVector3::Normalize(v3OBBCenter - v3SphereCenter);
    float fDist = maths::CVector3::Distance(v3OBBCenter, v3SphereCenter);
    UNUSED_VARIABLE(fDist);

    std::cout << "Distance: " << fDist << std::endl;

    maths::CVector3 v3HalfSize = _pOther->GetHalfSize();
    float fLenght = v3HalfSize.Length();
    UNUSED_VARIABLE(fLenght);

    float fSquareRadius = GetRadius() * GetRadius();
    UNUSED_VARIABLE(fSquareRadius);

    float fSum = fSquareRadius + fLenght;

    if (fDist <= fSum)
    {
      std::cout << "entra" << std::endl;
    }

    //float fClosestX = FLT_MAX;
    //float fClosestY = FLT_MAX;
    //float fClosestZ = FLT_MAX;

    /*for (const maths::CVector3& v3Axis : v3AxisDirectors)
    {
      UNUSED_VARIABLE(v3Axis);
      float fDot = maths::CVector3::Dot(v3Dir, v3Axis);
      std::cout << "Dot: " << fDot << std::endl;
    }*/

    maths::CVector3 v3AxisDir = maths::CVector3::Normalize(v3AxisDirectors[0] - GetCenter());
    float fDotX = maths::CVector3::Dot(v3AxisDir, v3AxisDirectors[0]);

    std::cout << "Dot X: " << fDotX << std::endl;

    float fDotY = maths::CVector3::Dot(v3Dir, v3AxisDirectors[1]);
    std::cout << "Dot Y: " << fDotY << std::endl;

    float fDotZ = maths::CVector3::Dot(v3Dir, v3AxisDirectors[2]);
    std::cout << "Dot Z: " << fDotZ << std::endl;

    //for (const maths::CVector3& v3Vertex : v3Extents)
    //{
    //  fClosestX = maths::Min(v3Vertex.X, GetCenter().X);
    //  fClosestY = maths::Min(v3Vertex.Y, GetCenter().Y);
    //  fClosestZ = maths::Min(v3Vertex.Z, GetCenter().Z);
    //}

    //// We calculate the squared distance between the center of the sphere and the nearest point.
    //fDist = (fClosestX - GetCenter().X) * (fClosestX - GetCenter().X) + 
    //  (fClosestY - GetCenter().Y) * (fClosestY - GetCenter().Y) +
    //  (fClosestZ - GetCenter().Z) * (fClosestZ - GetCenter().Z);

    //// We check if the distance is less than or equal to the radius squared of the sphere.
    //float fSquareRadius = GetRadius() * GetRadius();
    //if (fDist <= fSquareRadius)
    //{
    //  maths::CVector3 vImpactPoint(fClosestX, fClosestY, fClosestZ);
    //  _oHitEvent_.ImpactPoint = vImpactPoint;
    //  _oHitEvent_.Depth = fSquareRadius - fDist;
    //  _oHitEvent_.Normal = maths::CVector3::Normalize(vImpactPoint - GetCenter());
    //  return true;
    //}

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

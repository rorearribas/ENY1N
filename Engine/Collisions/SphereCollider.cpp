#include "SphereCollider.h"
#include "Engine/Engine.h"
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

  bool CSphereCollider::CheckCollision(const CCollider& _oCollider, THitEvent& _oHitEvent_)
  {
    const EColliderType& eColliderType = _oCollider.GetType();
#ifdef _DEBUG
    assert(eColliderType != EColliderType::INVALID);
#endif // DEBUG

    switch (eColliderType)
    {
    case EColliderType::BOX_COLLIDER:
    {
      const CBoxCollider& oBoxCollider = static_cast<const CBoxCollider&>(_oCollider);
#ifdef _DEBUG
      assert(&oBoxCollider);
#endif // DEBUG
      return oBoxCollider.IsOBB() ? CheckOBBCollision(&oBoxCollider, _oHitEvent_) : CheckAABBCollision(&oBoxCollider, _oHitEvent_);
    }
    case EColliderType::SPHERE_COLLIDER:
    {
      const CSphereCollider& oSphereCollider = static_cast<const CSphereCollider&>(_oCollider);
#ifdef _DEBUG
      assert(&oSphereCollider);
#endif // DEBUG
      return CheckSphereCollision(&oSphereCollider, _oHitEvent_);
    }
    }
    return false;
  }
  // ------------------------------------
  bool CSphereCollider::IntersectRay(const physics::CRay& _oRay, THitEvent& _oHitEvent_, const float& _fMaxDistance)
  {
    float fDist = 0.0f;
    math::CVector3 v3ClosestPoint = math::CVector3::Zero;
    if (math::ClosestPtRaySphere(_oRay, GetCenter(), GetRadius(), v3ClosestPoint, fDist) && fDist <= _fMaxDistance)
    {
      _oHitEvent_.Object = GetOwner();
      _oHitEvent_.ImpactPoint = v3ClosestPoint;
      _oHitEvent_.Normal = math::CVector3::Normalize(v3ClosestPoint - GetCenter());
      _oHitEvent_.Distance = fDist;
      return true;
    }
    return false;
  }
  // ------------------------------------
  void CSphereCollider::RecalculateCollider()
  {
    // Set world center
    m_v3Center = GetPos();
  }
  // ------------------------------------
  bool CSphereCollider::CheckSphereCollision(const CSphereCollider* _pOther, THitEvent& _oHitEvent_) const
  {
    const math::CVector3 v3Center = GetCenter();
    math::CVector3 v3Offset = v3Center - _pOther->GetCenter();
    math::CVector3 v3Dir = math::CVector3::Normalize(v3Offset);

    // Calculate values
    float fDistanceSquared = (v3Offset.x * v3Offset.x) + (v3Offset.y * v3Offset.y) + (v3Offset.z * v3Offset.z);
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
  bool CSphereCollider::CheckOBBCollision(const CBoxCollider* _pOther, THitEvent& _oHitEvent_) const
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
    float fClampedX = math::Clamp(fProjX, -v3HalfSize.x, v3HalfSize.x);
    float fClampedY = math::Clamp(fProjY, -v3HalfSize.y, v3HalfSize.y);
    float fClampedZ = math::Clamp(fProjZ, -v3HalfSize.z, v3HalfSize.z);

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
  bool CSphereCollider::CheckAABBCollision(const CBoxCollider* _pOther, THitEvent& _oHitEvent_) const
  {
    // We have to find the closest point.
    const math::CVector3& v3Center = GetCenter();
    float fClosestX = math::Max(_pOther->GetMin().x, math::Min(v3Center.x, _pOther->GetMax().x));
    float fClosestY = math::Max(_pOther->GetMin().y, math::Min(v3Center.y, _pOther->GetMax().y));
    float fClosestZ = math::Max(_pOther->GetMin().z, math::Min(v3Center.z, _pOther->GetMax().z));

    // We have to calculate the squared distance between the center of the sphere and the nearest point.
    float fSquaredDist = (fClosestX - v3Center.x) * (fClosestX - v3Center.x) +
      (fClosestY - v3Center.y) * (fClosestY - v3Center.y) +
      (fClosestZ - v3Center.z) * (fClosestZ - v3Center.z);

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
      GetPos(),
      GetRadius(),
      internal_sphere_collider::s_iMaxSubvH,
      internal_sphere_collider::s_iMaxSubvV,
      math::CVector3::One,
      render::ERenderMode::WIREFRAME
    );
  }
}

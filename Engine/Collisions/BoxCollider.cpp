#include "BoxCollider.h"
#include "Engine/Engine.h"
#include "SphereCollider.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/Math/Math.h"
#include <cassert>
#include "Libs/Math/Vector3.h"

namespace collision
{
  namespace internal_box_collider
  {
    static const float s_fDebugRadius = 0.025f;
  }
  // ------------------------------------
  CBoxCollider::CBoxCollider(void* _pOwner) : CCollider(collision::EColliderType::BOX_COLLIDER, _pOwner)
  {
    ComputeMinMax();
    ComputeExtents();
  }
  // ------------------------------------
  CBoxCollider::~CBoxCollider()
  {
    m_vctPrimitives.clear();
  }
  // ------------------------------------
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
      return IsOBBEnabled() ? CheckOBBCollision(&oBoxCollider, _oHitEvent_) : CheckAABBCollision(&oBoxCollider, _oHitEvent_);
    }
    case EColliderType::SPHERE_COLLIDER:
    {
      const CSphereCollider& oSphereCollider = static_cast<const CSphereCollider&>(_oCollider);
      assert(&oSphereCollider);
      return IsOBBEnabled() ? CheckOBBSphereCollision(&oSphereCollider, _oHitEvent_) : CheckSphereCollision(&oSphereCollider, _oHitEvent_);
    }
    }
    return false;
  }
  // ------------------------------------
  bool CBoxCollider::IntersectRay(const physics::CRay& _oRay, SHitEvent& _oHitEvent_, const float& _fMaxDistance)
  {
    const math::CVector3& v3RayOrigin = _oRay.GetOrigin();
    const math::CVector3& v3RayDir = _oRay.GetDir();
    math::CVector3 v3Delta = v3RayOrigin - GetPosition();

    const std::vector<math::CVector3> vctAxis = GetAxisDirectors();
    math::CVector3 v3HalfSize = (m_v3Max - m_v3Min) * 0.5f;

    float fMinValue = -FLT_MAX;
    float fMaxValue = _fMaxDistance;
    int iHitAxis = -1;

    // Standard slab algorithm
    for (uint32_t uIndex = 0; uIndex < 3; ++uIndex)
    {
      const math::CVector3& v3Axis = vctAxis[uIndex];
      float fDotDelta = math::CVector3::Dot(v3Axis, v3Delta);
      float fDotRayDir = math::CVector3::Dot(v3Axis, v3RayDir);

      if (fabs(fDotRayDir) > math::s_fEpsilon3)
      {
        float t1 = (-fDotDelta - v3HalfSize[uIndex]) / fDotRayDir;
        float t2 = (-fDotDelta + v3HalfSize[uIndex]) / fDotRayDir;

        if (t1 > t2)
        {
          std::swap(t1, t2);
        }

        if (t1 > fMinValue)
        {
          fMinValue = t1;
          iHitAxis = static_cast<int>(uIndex);
        }

        fMaxValue = math::Min(fMaxValue, t2);
        if (fMinValue > fMaxValue)
        {
          return false;
        }
      }
      else
      {
        // Check parallel ray
        if (fabs(fDotDelta) > v3HalfSize[uIndex])
        {
          return false;
        }
      }
    }

    // Set valid distance
    float fHitDistance = fMinValue >= 0.0f ? fMinValue : fMaxValue;
    if (fHitDistance < 0.0f || fHitDistance > _fMaxDistance)
    {
      return false;
    }

    // Apply hit data
    _oHitEvent_.Object = GetOwner();
    _oHitEvent_.Distance = fHitDistance;
    _oHitEvent_.ImpactPoint = v3RayOrigin + v3RayDir * fHitDistance;

    if (iHitAxis >= 0)
    {
      float fDot = math::CVector3::Dot(vctAxis[iHitAxis], v3RayDir);
      _oHitEvent_.Normal = (fDot < 0.0f) ? vctAxis[iHitAxis] : -vctAxis[iHitAxis];
    }

    return true;
  }
  // ------------------------------------
  void CBoxCollider::RecalculateCollider()
  {
    ComputeMinMax();
    ComputeExtents();
  }
  // ------------------------------------
  std::vector<math::CVector3> CBoxCollider::GetAxisDirectors() const
  {
    return std::vector<math::CVector3> { m_v3Right, m_v3Up, m_v3Forward };
  }
  // ------------------------------------
  void CBoxCollider::DrawDebug()
  {
    // Draw cube
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    pEngine->DrawCube(GetPosition(), GetSize(), GetRotation(), math::CVector3::One);

    // Draw extents
    float fMagnitude = m_v3Size.Magnitude();
    for (size_t tIndex = 0; tIndex < m_v3Extents.size(); tIndex++)
    {
      math::CVector3 v3Pos = m_v3Extents[tIndex];
      pEngine->DrawSphere(v3Pos, internal_box_collider::s_fDebugRadius * fMagnitude, 8, 8, math::CVector3::Right);
    }
  }
  // ------------------------------------
  bool CBoxCollider::CheckOBBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    const std::vector<math::CVector3>& v3Extents = this->GetExtents();
    const std::vector<math::CVector3>& v3OtherExtents = _pOther->GetExtents();

    std::vector<math::CVector3> vctAxis =
    {
      // Normals 
      this->m_v3Right,
      this->m_v3Up,
      this->m_v3Forward,

      // Other normals
      _pOther->m_v3Right,
      _pOther->m_v3Up,
      _pOther->m_v3Forward,

      // Cross Product
      math::CVector3::Cross(this->m_v3Right, _pOther->m_v3Right),
      math::CVector3::Cross(this->m_v3Right, _pOther->m_v3Up),
      math::CVector3::Cross(this->m_v3Right, _pOther->m_v3Forward),
      math::CVector3::Cross(this->m_v3Up, _pOther->m_v3Right),
      math::CVector3::Cross(this->m_v3Up, _pOther->m_v3Up),
      math::CVector3::Cross(this->m_v3Up, _pOther->m_v3Forward),
      math::CVector3::Cross(this->m_v3Forward, _pOther->m_v3Right),
      math::CVector3::Cross(this->m_v3Forward, _pOther->m_v3Up),
      math::CVector3::Cross(this->m_v3Forward, _pOther->m_v3Forward)
    };

    math::CVector3 v3ImpactPoint = math::CVector3::Zero;
    math::CVector3 v3Normal = math::CVector3::Zero;
    float fDepth = FLT_MAX;

    for (const math::CVector3& v3Axis : vctAxis)
    {
      if (math::SeparateAxisTheorem(v3Extents, v3OtherExtents, v3Axis, v3ImpactPoint, v3Normal, fDepth))
      {
        return false;
      }
    }

    // Compute impact point
    _oHitEvent_.ImpactPoint = v3ImpactPoint;
    _oHitEvent_.Normal = v3Normal;
    _oHitEvent_.Depth = std::abs(fDepth);

    return true;
  }
  // ------------------------------------
  bool CBoxCollider::CheckAABBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    bool bCheckCollision = (m_v3Min.X <= _pOther->m_v3Max.X && m_v3Max.X >= _pOther->m_v3Min.X) &&
      (m_v3Min.Y <= _pOther->m_v3Max.Y && m_v3Max.Y >= _pOther->m_v3Min.Y) &&
      (m_v3Min.Z <= _pOther->m_v3Max.Z && m_v3Max.Z >= _pOther->m_v3Min.Z);

    if (bCheckCollision)
    {
      math::CVector3 v3HalfSize = (m_v3Max - m_v3Min) * 0.5f;
      math::CVector3 v3OtherHalfSize = (_pOther->m_v3Max - _pOther->m_v3Min) * 0.5f;

      // Compute offset
      math::CVector3 v3Dir = _pOther->GetCenter() - GetCenter();
      math::CVector3 v3Abs = math::CVector3::Abs(v3Dir);
      v3Dir.Normalize();

      // Calculate overlap
      float fOverlapX = (v3HalfSize.X + v3OtherHalfSize.X) - v3Abs.X;
      float fOverlapY = (v3HalfSize.Y + v3OtherHalfSize.Y) - v3Abs.Y;
      float fOverlapZ = (v3HalfSize.Z + v3OtherHalfSize.Z) - v3Abs.Z;

      // Get depth and normal
      math::CVector3 v3Normal = math::CVector3::Zero;
      float fDepth = fOverlapX;

      if (fOverlapY < fDepth)
      {
        fDepth = fOverlapY;
        v3Normal = math::CVector3(0, (v3Dir.Y < 0) ? 1.0f : -1.0f, 0);
      }
      if (fOverlapZ < fDepth)
      {
        fDepth = fOverlapZ;
        v3Normal = math::CVector3(0, 0, (v3Dir.Z < 0) ? 1.0f : -1.0f);
      }
      if (fOverlapX == fDepth)
      {
        v3Normal = math::CVector3((v3Dir.X < 0) ? 1.0f : -1.0f, 0, 0);
      }

      // Fill hit event
      _oHitEvent_.ImpactPoint = _pOther->GetCenter() - (v3Normal * v3OtherHalfSize);
      _oHitEvent_.Normal = v3Normal;
      _oHitEvent_.Depth = fDepth;

      return true;
    }
    return false;
  }
  // ------------------------------------
  bool CBoxCollider::CheckOBBSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    // Get OBB data
    const math::CVector3& v3OBBCenter = GetCenter();
    const math::CVector3 v3HalfSize = GetHalfSize();
    const std::vector<math::CVector3> v3Axis = GetAxisDirectors();

    // Calculate dir
    const math::CVector3& v3SphereCenter = _pOther->GetCenter();
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
    float fRadius = _pOther->GetRadius();
    float fDistance = math::CVector3::Distance(v3ClosestPoint, v3SphereCenter);
    if (fDistance <= fRadius)
    {
      _oHitEvent_.ImpactPoint = v3ClosestPoint;
      _oHitEvent_.Depth = fRadius - fDistance;
      _oHitEvent_.Normal = math::CVector3::Normalize(v3ClosestPoint - v3SphereCenter);
      return true;
    }
    return false;
  }
  // ------------------------------------
  bool CBoxCollider::CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    // We have to find the closest point.
    const math::CVector3& v3SphereCenter = _pOther->GetCenter();
    float fClosestX = math::Max(m_v3Min.X, math::Min(v3SphereCenter.X, m_v3Max.X));
    float fClosestY = math::Max(m_v3Min.Y, math::Min(v3SphereCenter.Y, m_v3Max.Y));
    float fClosestZ = math::Max(m_v3Min.Z, math::Min(v3SphereCenter.Z, m_v3Max.Z));

    // We have to calculate the squared distance between the center of the sphere and the nearest point.
    float fSquaredDist = (fClosestX - v3SphereCenter.X) * (fClosestX - v3SphereCenter.X) +
      (fClosestY - v3SphereCenter.Y) * (fClosestY - v3SphereCenter.Y) +
      (fClosestZ - v3SphereCenter.Z) * (fClosestZ - v3SphereCenter.Z);

    // We check if the distance is less than or equal to the radius squared of the sphere.
    const float& fRadius = _pOther->GetRadius();
    float fSquareRadius = fRadius * fRadius;
    if (fSquaredDist <= fSquareRadius)
    {
      math::CVector3 v3ImpactPoint(fClosestX, fClosestY, fClosestZ);
      _oHitEvent_.ImpactPoint = v3ImpactPoint;
      _oHitEvent_.Depth = fRadius - sqrt(fSquaredDist);
      _oHitEvent_.Normal = math::CVector3::Normalize(v3ImpactPoint - v3SphereCenter);
      return true;
    }
    return false;
  }
  // ------------------------------------
  void CBoxCollider::ComputeExtents()
  {
    // Calculate matrix
    math::CMatrix4x4 mRot = math::CMatrix4x4::Rotation(GetRotation());

    // Calculate extents
    const math::CVector3& v3Center = GetCenter();
    m_v3Extents =
    {
      v3Center + mRot * (m_v3Min - v3Center),
      v3Center + mRot * (math::CVector3(m_v3Min.X, m_v3Min.Y, m_v3Max.Z) - v3Center),
      v3Center + mRot * (math::CVector3(m_v3Min.X, m_v3Max.Y, m_v3Min.Z) - v3Center),
      v3Center + mRot * (math::CVector3(m_v3Min.X, m_v3Max.Y, m_v3Max.Z) - v3Center),
      v3Center + mRot * (math::CVector3(m_v3Max.X, m_v3Min.Y, m_v3Min.Z) - v3Center),
      v3Center + mRot * (math::CVector3(m_v3Max.X, m_v3Min.Y, m_v3Max.Z) - v3Center),
      v3Center + mRot * (math::CVector3(m_v3Max.X, m_v3Max.Y, m_v3Min.Z) - v3Center),
      v3Center + mRot * (m_v3Max - v3Center)
    };

    // Set dir vectors
    m_v3Forward = mRot * math::CVector3::Forward;
    m_v3Right = mRot * math::CVector3::Right;
    m_v3Up = mRot * math::CVector3::Up;
  }
  // ------------------------------------
  void CBoxCollider::ComputeMinMax()
  {
    math::CVector3 v3HalfSize = m_v3Size * 0.5f;
    m_v3Min = GetPosition() - v3HalfSize;
    m_v3Max = GetPosition() + v3HalfSize;
  }
}
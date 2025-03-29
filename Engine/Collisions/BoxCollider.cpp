#include "BoxCollider.h"
#include "Engine/Base/Engine.h"
#include "SphereCollider.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/Maths/Maths.h"
#include <algorithm>
#include <cassert>

namespace collisions
{
  CBoxCollider::CBoxCollider(void* _pOwner) : CCollider(BOX_COLLIDER, _pOwner)
  {
    ComputeMinMax();
    ComputeExtents();
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
    bool bCheckCollision = (m_vMin.X <= _pOther->m_vMax.X && m_vMax.X >= _pOther->m_vMin.X) &&
      (m_vMin.Y <= _pOther->m_vMax.Y && m_vMax.Y >= _pOther->m_vMin.Y) &&
      (m_vMin.Z <= _pOther->m_vMax.Z && m_vMax.Z >= _pOther->m_vMin.Z);

    if (bCheckCollision)
    {
      // Half size AABB
      maths::CVector3 v3HalfSize = (m_vMax - m_vMin) * 0.5f;
      maths::CVector3 v3OtherHalfSize = (_pOther->m_vMax - _pOther->m_vMin) * 0.5f;

      // Compute offset
      maths::CVector3 v3Offset = _pOther->GetCenter() - GetCenter();
      maths::CVector3 v3AbsDelta = maths::CVector3::Abs(v3Offset);

      // Calculate overlap
      float fOverlapX = (v3HalfSize.X + v3OtherHalfSize.X) - v3AbsDelta.X;
      float fOverlapY = (v3HalfSize.Y + v3OtherHalfSize.Y) - v3AbsDelta.Y;
      float fOverlapZ = (v3HalfSize.Z + v3OtherHalfSize.Z) - v3AbsDelta.Z;

      maths::CVector3 v3Normal(0, 0, 0);
      float fDepth = 0.0f;

      if (fOverlapX < fOverlapY && fOverlapX < fOverlapZ)
      {
        v3Normal = maths::CVector3((v3Offset.X < 0) ? -1.0f : 1.0f, 0, 0);
        fDepth = fOverlapX;
      }
      else if (fOverlapY < fOverlapZ)
      {
        v3Normal = maths::CVector3(0, (v3Offset.Y < 0) ? -1.0f : 1.0f, 0);
        fDepth = fOverlapY;
      }
      else
      {
        v3Normal = maths::CVector3(0, 0, (v3Offset.Z < 0) ? -1.0f : 1.0f);
        fDepth = fOverlapZ;
      }

      // Fill values
      _oHitEvent_.ImpactPoint = (_pOther->GetCenter() - v3Normal) * v3OtherHalfSize;
      _oHitEvent_.Normal = v3Normal;
      _oHitEvent_.Depth = fDepth;

      return true;
    }

    return false;
  }
  // ------------------------------------
  bool CBoxCollider::CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    // We have to find the closest point.
    const maths::CVector3& v3Center = _pOther->GetCenter();
    float fClosestX = maths::Max<float>(m_vMin.X, maths::Min<float>(v3Center.X, m_vMax.X));
    float fClosestY = maths::Max<float>(m_vMin.Y, maths::Min<float>(v3Center.Y, m_vMax.Y));
    float fClosestZ = maths::Max<float>(m_vMin.Z, maths::Min<float>(v3Center.Z, m_vMax.Z));

    // We have to calculate the squared distance between the center of the sphere and the nearest point.
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
    // Calculate matrix
    maths::CMatrix4x4 mRot = maths::CMatrix4x4::Rotation(GetRotation());
    mRot = maths::CMatrix4x4::Transpose(mRot);

    // Calculate extents
    m_v3Extents =
    {
      GetCenter() + mRot * (maths::CVector3(m_vMin.X, m_vMin.Y, m_vMin.Z) - GetCenter()),
      GetCenter() + mRot * (maths::CVector3(m_vMin.X, m_vMin.Y, m_vMax.Z) - GetCenter()),
      GetCenter() + mRot * (maths::CVector3(m_vMin.X, m_vMax.Y, m_vMin.Z) - GetCenter()),
      GetCenter() + mRot * (maths::CVector3(m_vMin.X, m_vMax.Y, m_vMax.Z) - GetCenter()),
      GetCenter() + mRot * (maths::CVector3(m_vMax.X, m_vMin.Y, m_vMin.Z) - GetCenter()),
      GetCenter() + mRot * (maths::CVector3(m_vMax.X, m_vMin.Y, m_vMax.Z) - GetCenter()),
      GetCenter() + mRot * (maths::CVector3(m_vMax.X, m_vMax.Y, m_vMin.Z) - GetCenter()),
      GetCenter() + mRot * (maths::CVector3(m_vMax.X, m_vMax.Y, m_vMax.Z) - GetCenter())
    };

    // Create debug
    if (m_vctPrimitives.empty())
    {
      for (int iIndex = 0; iIndex < static_cast<int>(m_v3Extents.size()); iIndex++)
      {
        engine::CEngine* pEngine = engine::CEngine::GetInstance();
        auto* pPrimitive = pEngine->CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType::E3D_SPHERE);
        pPrimitive->SetColor(maths::CVector3::Right);
        pPrimitive->SetScale(maths::CVector3::One / 8.0f);
        m_vctPrimitives.emplace_back(pPrimitive);
      }
    }

    // Update position
    for (int iIndex = 0; iIndex < static_cast<int>(m_v3Extents.size()); iIndex++)
    {
      m_vctPrimitives[iIndex]->SetPosition(m_v3Extents[iIndex]);
    }
  }
  // ------------------------------------
  void CBoxCollider::ComputeMinMax()
  {
    maths::CVector3 v3HalfSize = m_vSize * 0.5f;
    m_vMin = GetPosition() - v3HalfSize;
    m_vMax = GetPosition() + v3HalfSize;
  }
}
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
    m_v3Size = _v3Size;
    RecalculateCollider();
  }
  // ------------------------------------
  bool CBoxCollider::CheckBoxCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    bool bCheckCollision = (m_v3Min.X <= _pOther->m_v3Max.X && m_v3Max.X >= _pOther->m_v3Min.X) &&
      (m_v3Min.Y <= _pOther->m_v3Max.Y && m_v3Max.Y >= _pOther->m_v3Min.Y) &&
      (m_v3Min.Z <= _pOther->m_v3Max.Z && m_v3Max.Z >= _pOther->m_v3Min.Z);

    if (bCheckCollision)
    { 
      maths::CVector3 v3HalfSize = (m_v3Max - m_v3Min) * 0.5f;
      maths::CVector3 v3OtherHalfSize = (_pOther->m_v3Max - _pOther->m_v3Min) * 0.5f;

      // Compute offset
      maths::CVector3 v3Dir = _pOther->GetCenter() - GetCenter();
      maths::CVector3 v3Abs = maths::CVector3::Abs(v3Dir);
      v3Dir.Normalize();

      // Calculate overlap
      float fOverlapX = (v3HalfSize.X + v3OtherHalfSize.X) - v3Abs.X;
      float fOverlapY = (v3HalfSize.Y + v3OtherHalfSize.Y) - v3Abs.Y;
      float fOverlapZ = (v3HalfSize.Z + v3OtherHalfSize.Z) - v3Abs.Z;

      // Get depth and normal
      maths::CVector3 v3Normal = maths::CVector3::Zero;
      float fDepth = fOverlapX;

      if (fOverlapY < fDepth) 
      {
        fDepth = fOverlapY;
        v3Normal = maths::CVector3(0, (v3Dir.Y < 0) ? -1.0f : 1.0f, 0);
      }
      if (fOverlapZ < fDepth) 
      {
        fDepth = fOverlapZ;
        v3Normal = maths::CVector3(0, 0, (v3Dir.Z < 0) ? -1.0f : 1.0f);
      }
      if (fOverlapX == fDepth) 
      {
        v3Normal = maths::CVector3((v3Dir.X < 0) ? -1.0f : 1.0f, 0, 0);
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
  bool CBoxCollider::CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const
  {
    // We have to find the closest point.
    const maths::CVector3& v3Center = _pOther->GetCenter();
    float fClosestX = maths::Max<float>(m_v3Min.X, maths::Min<float>(v3Center.X, m_v3Max.X));
    float fClosestY = maths::Max<float>(m_v3Min.Y, maths::Min<float>(v3Center.Y, m_v3Max.Y));
    float fClosestZ = maths::Max<float>(m_v3Min.Z, maths::Min<float>(v3Center.Z, m_v3Max.Z));

    // We have to calculate the squared distance between the center of the sphere and the nearest point.
    float fDist = (fClosestX - _pOther->GetCenter().X) * (fClosestX - _pOther->GetCenter().X) +
      (fClosestY - _pOther->GetCenter().Y) * (fClosestY - _pOther->GetCenter().Y) +
      (fClosestZ - _pOther->GetCenter().Z) * (fClosestZ - _pOther->GetCenter().Z);

    // We check if the distance is less than or equal to the radius squared of the sphere.
    float fSquareRadius = _pOther->GetRadius() * _pOther->GetRadius();
    if (fDist <= fSquareRadius)
    {
      maths::CVector3 v3ImpactPoint(fClosestX, fClosestY, fClosestZ);
      _oHitEvent_.ImpactPoint = v3ImpactPoint;
      _oHitEvent_.Depth = fSquareRadius - fDist;
      _oHitEvent_.Normal = maths::CVector3::Normalize(_pOther->GetCenter() - v3ImpactPoint);
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
    const maths::CVector3& v3Center = GetCenter();
    m_v3Extents =
    {
      v3Center + mRot * (m_v3Min - v3Center),
      v3Center + mRot * (maths::CVector3(m_v3Min.X, m_v3Min.Y, m_v3Max.Z) - v3Center),
      v3Center + mRot * (maths::CVector3(m_v3Min.X, m_v3Max.Y, m_v3Min.Z) - v3Center),
      v3Center + mRot * (maths::CVector3(m_v3Min.X, m_v3Max.Y, m_v3Max.Z) - v3Center),
      v3Center + mRot * (maths::CVector3(m_v3Max.X, m_v3Min.Y, m_v3Min.Z) - v3Center),
      v3Center + mRot * (maths::CVector3(m_v3Max.X, m_v3Min.Y, m_v3Max.Z) - v3Center),
      v3Center + mRot * (maths::CVector3(m_v3Max.X, m_v3Max.Y, m_v3Min.Z) - v3Center),
      v3Center + mRot * (m_v3Max - v3Center)
    };

    // Set values
    m_v3Forward = mRot * maths::CVector3::Forward;
    m_v3Right = mRot * maths::CVector3::Right;
    m_v3Up = mRot * maths::CVector3::Up;

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
    maths::CVector3 v3HalfSize = m_v3Size * 0.5f;
    m_v3Min = GetPosition() - v3HalfSize;
    m_v3Max = GetPosition() + v3HalfSize;
  }
}
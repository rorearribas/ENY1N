#include "CapsuleCollider.h"
#include "Engine/Base/Engine.h"
#include "Libs/Macros/GlobalMacros.h"

namespace collision
{
  namespace internal_capsule_collider
  {
    const int s_iMaxSubvH(12);
    const int s_iMaxSubvV(12);
  }

  bool CCapsuleCollider::CheckCollision(const CCollider& /*_pOther*/, SHitEvent& /*_oHitEvent_*/)
  {
    return false;
  }
  // ------------------------------------
  bool CCapsuleCollider::IntersectRay(const physics::CRay& /*_oRay*/, SHitEvent& /*_oHitEvent_*/, const float& /*_fMaxDistance*/)
  {
    return false;
  }
  // ------------------------------------
  void CCapsuleCollider::RecalculateCollider()
  {
    // Calculate axis directors
    math::CMatrix4x4 mRot = math::CMatrix4x4::Rotation(GetRotation());
    mRot = math::CMatrix4x4::Transpose(mRot);

    m_v3Forward = mRot * math::CVector3::Forward;
    m_v3Right = mRot * math::CVector3::Right;
    m_v3Up = mRot * math::CVector3::Up;
  }
  // ------------------------------------
  void CCapsuleCollider::SetRadius(float _fRadius)
  {
    m_fRadius = _fRadius;
  }
  // ------------------------------------
  void CCapsuleCollider::SetHeight(float _fHeight)
  {
    m_fHeight = _fHeight;
  }
  // ------------------------------------
  void CCapsuleCollider::DrawDebug()
  {
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    const math::CVector3& v3CurrentPos = GetPosition(); // Get center

    float fHeight = m_fHeight * 0.5f;
    float fDiff = m_fHeight - m_fRadius;
    UNUSED_VAR(fDiff);

    pEngine->DrawSphere(v3CurrentPos + (m_v3Up * fHeight), 0.125f, 8, 8, math::CVector3::Forward);
    pEngine->DrawSphere(v3CurrentPos - (m_v3Up * fHeight), 0.125f, 8, 8, math::CVector3::Right);

    pEngine->DrawCapsule
    (
      v3CurrentPos + m_v3LocalCenter,
      m_fRadius, // Radius
      m_fHeight, // Height
      internal_capsule_collider::s_iMaxSubvH, // Horizontal
      internal_capsule_collider::s_iMaxSubvV, // Vertical
      math::CVector3::One
    );
  }
  // ------------------------------------
  bool CCapsuleCollider::CheckOBBCollision(const CBoxCollider* /*_pOther*/, SHitEvent& /*_oHitEvent_*/) const
  {
    return false;
  }
  // ------------------------------------
  bool CCapsuleCollider::CheckBoxCollision(const CBoxCollider* /*_pOther*/, SHitEvent& /*_oHitEvent_*/) const
  {
    return false;
  }
  // ------------------------------------
  bool CCapsuleCollider::CheckCapsuleCollision(const CCapsuleCollider* /*_pOther*/, SHitEvent& /*_oHitEvent_*/) const
  {
    return false;
  }
  // ------------------------------------
  bool CCapsuleCollider::CheckSphereCollision(const CSphereCollider* /*_pOther*/, SHitEvent& /*_oHitEvent_*/) const
  {
    return false;
  }
}
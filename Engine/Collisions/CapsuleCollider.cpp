#include "CapsuleCollider.h"
#include "Engine/Base/Engine.h"

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
    // Not implemented yet!
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
    pEngine->DrawCapsule
    (
      GetPosition() + m_v3LocalCenter, 
      m_fRadius, // Radius
      m_fHeight, // Height
      internal_capsule_collider::s_iMaxSubvH, // Horizontal
      internal_capsule_collider::s_iMaxSubvV, // Vertical
      math::CVector3::One
    );
  }
}
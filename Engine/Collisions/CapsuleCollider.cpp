#include "CapsuleCollider.h"

namespace collision
{
  bool CCapsuleCollider::CheckCollision(const CCollider& _pOther, SHitEvent& _oHitEvent_)
  {
    return CCollider::CheckCollision(_pOther, _oHitEvent_);
  }
  // ------------------------------------
  bool CCapsuleCollider::IntersectRay(const physics::CRay& _oRay, SHitEvent& _oHitEvent_, const float& _fMaxDistance)
  {
    return CCollider::IntersectRay(_oRay, _oHitEvent_, _fMaxDistance);
  }
  // ------------------------------------
  void CCapsuleCollider::RecalculateCollider()
  {
    CCollider::RecalculateCollider();
  }
  // ------------------------------------
  void CCapsuleCollider::DrawDebug()
  {
    // Not implemented yet!
  }
}
#pragma once
#include "Collider.h"

namespace collision
{
  class CCapsuleCollider final : public CCollider
  {
  public:
    CCapsuleCollider(void* _pOwner) : CCollider(EColliderType::CAPSULE_COLLIDER, _pOwner) {}
    virtual ~CCapsuleCollider() {}

    virtual bool CheckCollision(const CCollider& _pOther, SHitEvent& _oHitEvent_) override;
    virtual bool IntersectRay(const physics::CRay& _oRay, SHitEvent& _oHitEvent_, const float& _fMaxDistance) override;
    virtual void RecalculateCollider() override;

    inline void SetSize(const math::CVector3& _v3Size) { m_v3Size = _v3Size; }
    inline const math::CVector3& GetSize() const { return m_v3Size; }

    virtual void DrawDebug() override;

  private:
    math::CVector3 m_v3Size = math::CVector3::One;
    math::CVector3 m_v3Center = math::CVector3::Zero;
  };
}



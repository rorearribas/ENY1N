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

    inline const math::CVector3& GetCenter() const { return m_v3Center; }
    inline const float GetRadius() const { return m_fRadius; }
    inline const float GetHeight() const { return m_fHeight; }

    void SetRadius(float _fRadius);
    void SetHeight(float _fHeight);

    virtual void DrawDebug() override;

  private:
    math::CVector3 m_v3Center = math::CVector3::Zero; // Local displacement
    float m_fRadius = 0.25f;
    float m_fHeight = 1.0f;
  };
}



#pragma once
#include "Collider.h"

namespace collision { class CSphereCollider; }

namespace collision { class CBoxCollider; }

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

    inline const math::CVector3& GetLocalCenter() const { return m_v3LocalCenter; }
    inline void SetLocalCenter(const math::CVector3& _v3LocalCenter) { m_v3LocalCenter = _v3LocalCenter; }

    inline const float GetRadius() const { return m_fRadius; }
    inline const float GetHeight() const { return m_fHeight; }

    void SetRadius(float _fRadius);
    void SetHeight(float _fHeight);

    virtual void DrawDebug() override;

  private:
    bool CheckCapsuleCollision(const CCapsuleCollider* _pOther, SHitEvent& _oHitEvent_) const;
    bool CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const;
    bool CheckOBBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const;
    bool CheckBoxCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const;

    math::CVector3 m_v3LocalCenter = math::CVector3::Zero; // Local displacement
    float m_fHeight = 2.0f;
    float m_fRadius = 0.5f;

    math::CVector3 m_v3Forward = math::CVector3::Zero;
    math::CVector3 m_v3Right = math::CVector3::Zero;
    math::CVector3 m_v3Up = math::CVector3::Zero;
  };
}



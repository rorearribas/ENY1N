#pragma once
#include "Libs/Math/Vector3.h"
#include "Collider.h"

namespace collision { class CBoxCollider; }

namespace collision
{
  class CSphereCollider : public CCollider
  {
  public:
    CSphereCollider(void* _pOwner) : CCollider(EColliderType::SPHERE_COLLIDER, _pOwner) {}
    virtual ~CSphereCollider() {}

    virtual bool CheckCollision(const CCollider&, SHitEvent& _oHitEvent_) override;
    virtual bool IntersectRay(const physics::CRay& _oRay, SHitEvent& _oHitEvent_, const float& _fMaxDistance) override;
    virtual void RecalculateCollider() override;

    void SetCenter(const math::CVector3& _v3Center) { m_v3Center = _v3Center; }
    const math::CVector3& GetCenter() const { return m_v3Center; }
    void SetRadius(float _fRadius) { m_fRadius = _fRadius; }
    const float& GetRadius() const { return m_fRadius;}

  private:
    bool CheckOBBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const;
    bool CheckBoxCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const;
    bool CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const;

    math::CVector3 m_v3Center = math::CVector3::Zero; // World position
    float m_fRadius = 0.5f; // Radius
  };
}


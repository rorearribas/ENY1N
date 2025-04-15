#pragma once
#include "Libs/Maths/Vector3.h"
#include "Collider.h"

namespace collisions { class CBoxCollider; }

namespace collisions
{
  class CSphereCollider : public CCollider
  {
  public:
    CSphereCollider(void* _pOwner) : CCollider(SPHERE_COLLIDER, _pOwner) {}
    virtual ~CSphereCollider() {}

    bool CheckCollision(const CCollider&, SHitEvent& _oHitEvent_) override;
    virtual void RecalculateCollider() override;

    void SetCenter(const maths::CVector3& _v3Center) { m_v3Center = _v3Center; }
    const maths::CVector3& GetCenter() const { return m_v3Center; }
    void SetRadius(float _fRadius) { m_fRadius = _fRadius; }
    const float& GetRadius() const { return m_fRadius;}

  private:
    bool CheckOBBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const;
    bool CheckBoxCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const;
    bool CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const;

    maths::CVector3 m_v3Center = maths::CVector3::Zero; // World position
    float m_fRadius = 0.5f; // Radius
  };
}


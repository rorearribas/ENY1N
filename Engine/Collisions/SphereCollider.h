#pragma once
#include "Libs/Maths/Vector3.h"
#include "Collider.h"

namespace collisions { class CBoxCollider; }

namespace collisions
{
  class CSphereCollider : public CCollider
  {
  public:
    CSphereCollider() : CCollider(SPHERE_COLLIDER) {}
    CSphereCollider(float _fRadius) : CCollider(SPHERE_COLLIDER), m_fRadius(_fRadius) {}
    virtual ~CSphereCollider() {}

    bool CheckCollision(const CCollider&) override;
    virtual void RecalculateCollider() override;

    void SetCenter(const maths::CVector3& _v3Center) { m_v3Center = _v3Center; }
    const maths::CVector3& GetCenter() const { return m_v3Center; }
    void SetRadius(float _fRadius) { m_fRadius = _fRadius; }
    const float& GetRadius() const { return m_fRadius;}

  private:
    bool CheckBoxCollision(const CBoxCollider* _pOther) const;
    bool CheckSphereCollision(const CSphereCollider* _pOther) const;

    maths::CVector3 m_v3Center = maths::CVector3::Zero; // World position
    float m_fRadius = 0.5f; // Radius
  };
}


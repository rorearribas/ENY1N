#pragma once
#include "Libs/Maths/Vector3.h"
#include "Collider.h"

namespace collisions
{
  class CSphereCollider : public CCollider
  {
  public:
    CSphereCollider() : CCollider(SPHERE_COLLIDER) {}
    CSphereCollider(float _fRadius) : CCollider(SPHERE_COLLIDER), m_fRadius(_fRadius) {}
    virtual ~CSphereCollider() {}

    bool CheckCollision(const CCollider&) override { return false; }
    virtual void RecalculateCollider() override {};

    const maths::CVector3& GetCenter() const { return m_v3Center; }
    void SetRadius(float _fRadius) { m_fRadius = _fRadius; }
    const float& GetRadius() const { return m_fRadius;}

  private:
    maths::CVector3 m_v3Center = maths::CVector3::Zero; // World position
    float m_fRadius = 10.0f; // Radius
  };
}


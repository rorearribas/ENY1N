#pragma once
#include "Libs/Maths/Vector3.h"
#include "Collider.h"

namespace physics
{
  class CSphereCollider : public CCollider
  {
  public:
    CSphereCollider() : CCollider(SPHERE_COLLIDER) {}
    CSphereCollider(float _fRadius) : CCollider(SPHERE_COLLIDER), m_fRadius(_fRadius) {}
    virtual ~CSphereCollider() {}

    bool CheckCollision(const CCollider&) override { return false; }
    void SetRadius(float _fRadius) { m_fRadius = _fRadius; }
    const float& GetRadius() const { return m_fRadius;}

  private:
    float m_fRadius = 10.0f; // Radius
    maths::CVector3 m_v3Center = maths::CVector3::Zero; // World position
  };
}


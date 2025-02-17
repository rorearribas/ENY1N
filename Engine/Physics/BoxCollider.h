#pragma once
#include "Collider.h"
#include "Libs/Maths/Vector3.h"

namespace physics
{
  class CBoxCollider : public CCollider
  {
  public:
    CBoxCollider() : CCollider(BOX_COLLIDER) {}
    CBoxCollider(const maths::CVector3& _v3Min, const maths::CVector3& _v3Max) 
    : CCollider(BOX_COLLIDER), m_vMax(_v3Max), m_vMin(_v3Min) {}
    virtual ~CBoxCollider() {}

    bool CheckCollision(const CCollider& /*_other*/) override { return false; }
    const maths::CVector3& GetMax() const { return m_vMax; }
    const maths::CVector3& GetMin() const { return m_vMin; }
    maths::CVector3 GetCenter() const { return maths::CVector3(m_vMax + m_vMin) * 0.5f; }

  private:
    maths::CVector3 m_vMax = maths::CVector3::Zero;
    maths::CVector3 m_vMin = maths::CVector3::Zero;
  };
}


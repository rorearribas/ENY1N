#pragma once
#include "Collider.h"
#include "Libs/Maths/Vector3.h"
#include "SphereCollider.h"

namespace physics
{
  class CBoxCollider : public CCollider
  {
  public:
    CBoxCollider() : CCollider(BOX_COLLIDER) {}
    CBoxCollider(const maths::CVector3& _v3Min, const maths::CVector3& _v3Max) 
    : CCollider(BOX_COLLIDER), m_vMax(_v3Max), m_vMin(_v3Min) {}
    virtual ~CBoxCollider() {}

    bool CheckCollision(const CCollider& _other) override;

    maths::CVector3 GetCenter() const { return maths::CVector3(m_vMax + m_vMin) * 0.5f; }
    void SetMax(const maths::CVector3& _v3Max) { m_vMax = _v3Max; }
    const maths::CVector3& GetMax() const { return m_vMax; }
    void SetMin(const maths::CVector3& _v3Min) { m_vMin = _v3Min; }
    const maths::CVector3& GetMin() const { return m_vMin; }

  private:
    bool CheckBoxCollision(const CBoxCollider& _other);
    bool CheckSphereCollision(const CSphereCollider& _other);

  private:
    maths::CVector3 m_vMax = maths::CVector3::One;
    maths::CVector3 m_vMin = maths::CVector3::Zero;
  };
}


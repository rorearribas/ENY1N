#pragma once
#include "Collider.h"
#include "Libs/Maths/Vector3.h"

namespace collisions { class CSphereCollider; }

namespace collisions
{
  class CBoxCollider : public CCollider
  {
  public:
    CBoxCollider(void* _pOwner) : CCollider(BOX_COLLIDER, _pOwner) { ComputeMinMax(); }
    virtual ~CBoxCollider() {}

    virtual bool CheckCollision(const CCollider& _other, SHitEvent& _oHitEvent_) override;
    virtual void RecalculateCollider() override;

    void SetSize(const maths::CVector3& _v3Size);
    const maths::CVector3& GetSize() const { return m_vSize;}

    maths::CVector3 GetCenter() const { return maths::CVector3(m_vMax + m_vMin) * 0.5f; }
    const std::vector<maths::CVector3>& GetExtents() const { return m_v3Extents; }
    const maths::CVector3& GetMax() const { return m_vMax; }
    const maths::CVector3& GetMin() const { return m_vMin; }

  private:
    bool CheckBoxCollision(const CBoxCollider* _other) const;
    bool CheckSphereCollision(const CSphereCollider* _other, SHitEvent& _oHitEvent_) const;

    void ComputeExtents();
    void ComputeMinMax();

  private:
    // Size
    maths::CVector3 m_vSize = maths::CVector3::One;
    // Min - Max
    maths::CVector3 m_vMax = maths::CVector3::Zero;
    maths::CVector3 m_vMin = maths::CVector3::Zero;
    // Extents
    std::vector<maths::CVector3> m_v3Extents;
  };
}


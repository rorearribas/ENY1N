#pragma once
#include "Collider.h"
#include "Libs/Maths/Vector3.h"

namespace render { namespace graphics { class CPrimitive; } }
namespace collisions { class CSphereCollider; }

namespace collisions
{
  class CBoxCollider : public CCollider
  {
  public:
    CBoxCollider(void* _pOwner);
    virtual ~CBoxCollider() {}

    virtual bool CheckCollision(const CCollider& _pOther, SHitEvent& _oHitEvent_) override;
    virtual void RecalculateCollider() override;

    void SetOBBEnabled(bool _bEnabled) { m_bOBBEnabled = _bEnabled; }
    const bool& IsOBBEnabled() const { return m_bOBBEnabled; }

    void SetSize(const maths::CVector3& _v3Size);
    const maths::CVector3& GetSize() const { return m_v3Size; }

    maths::CVector3 GetCenter() const { return maths::CVector3(m_v3Max + m_v3Min) * 0.5f; }
    maths::CVector3 GetHalfSize() const { return maths::CVector3(m_v3Max - m_v3Min) * 0.5f; }

    const maths::CVector3& GetMax() const { return m_v3Max; }
    const maths::CVector3& GetMin() const { return m_v3Min; }

    const std::vector<maths::CVector3>& GetExtents() const { return m_v3Extents; }
    std::vector<maths::CVector3> GetAxisDirectors() const;

    const maths::CVector3& GetRightAxis() const { return m_v3Right; }
    const maths::CVector3& GetUpAxis() const { return m_v3Up; }
    const maths::CVector3& GetForwardAxis() const { return m_v3Forward; }

  private:
    bool CheckOBBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const;
    bool CheckBoxCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const;

    bool CheckOBBSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const;
    bool CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const;

    void ComputeExtents();
    void ComputeMinMax();

  private:
    // Size
    maths::CVector3 m_v3Size = maths::CVector3::One;

    // Min - Max
    maths::CVector3 m_v3Max = maths::CVector3::Zero;
    maths::CVector3 m_v3Min = maths::CVector3::Zero;

    // OBB
    maths::CVector3 m_v3Right = maths::CVector3::Right;
    maths::CVector3 m_v3Up = maths::CVector3::Up;
    maths::CVector3 m_v3Forward = maths::CVector3::Forward;
    bool m_bOBBEnabled = true;

    // Extents
    std::vector<maths::CVector3> m_v3Extents;
    std::vector<render::graphics::CPrimitive*> m_vctPrimitives;
  };
}


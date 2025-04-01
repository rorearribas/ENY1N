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

    virtual bool CheckCollision(const CCollider& _other, SHitEvent& _oHitEvent_) override;
    virtual void RecalculateCollider() override;

    void SetSize(const maths::CVector3& _v3Size);
    const maths::CVector3& GetSize() const { return m_v3Size;}

    maths::CVector3 GetCenter() const { return maths::CVector3(m_v3Max + m_v3Min) * 0.5f; }
    const std::vector<maths::CVector3>& GetExtents() const { return m_v3Extents; }
    const maths::CVector3& GetMax() const { return m_v3Max; }
    const maths::CVector3& GetMin() const { return m_v3Min; }

  private:
    bool CheckBoxCollision(const CBoxCollider* _other, SHitEvent& _oHitEvent_) const;
    bool CheckSphereCollision(const CSphereCollider* _other, SHitEvent& _oHitEvent_) const;

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

    // Extents
    std::vector<maths::CVector3> m_v3Extents;
    std::vector<render::graphics::CPrimitive*> m_vctPrimitives;
  };
}


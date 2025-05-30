#pragma once
#include "Collider.h"
#include "Libs/Math/Vector3.h"

namespace render { namespace graphics { class CPrimitive; } }
namespace collision { class CSphereCollider; }

namespace collision
{
  class CBoxCollider : public CCollider
  {
  public:
    CBoxCollider(void* _pOwner);
    virtual ~CBoxCollider();

    virtual bool CheckCollision(const CCollider& _pOther, SHitEvent& _oHitEvent_) override;
    virtual bool IntersectRay(const physics::CRay& _oRay, SHitEvent& _oHitEvent_, const float& _fMaxDistance) override;
    virtual void RecalculateCollider() override;

    void SetOBBEnabled(bool _bEnabled) { m_bOBBEnabled = _bEnabled; }
    const bool& IsOBBEnabled() const { return m_bOBBEnabled; }

    void SetSize(const math::CVector3& _v3Size);
    const math::CVector3& GetSize() const { return m_v3Size; }

    math::CVector3 GetCenter() const { return math::CVector3(m_v3Max + m_v3Min) * 0.5f; }
    math::CVector3 GetHalfSize() const { return math::CVector3(m_v3Max - m_v3Min) * 0.5f; }

    const math::CVector3& GetMax() const { return m_v3Max; }
    const math::CVector3& GetMin() const { return m_v3Min; }

    const std::vector<math::CVector3>& GetExtents() const { return m_v3Extents; }
    std::vector<math::CVector3> GetAxisDirectors() const;

    const math::CVector3& GetRightAxis() const { return m_v3Right; }
    const math::CVector3& GetUpAxis() const { return m_v3Up; }
    const math::CVector3& GetForwardAxis() const { return m_v3Forward; }

    void DrawDebug();

  private:
    bool CheckOBBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const;
    bool CheckBoxCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const;

    bool CheckOBBSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const;
    bool CheckSphereCollision(const CSphereCollider* _pOther, SHitEvent& _oHitEvent_) const;

    void ComputeExtents();
    void ComputeMinMax();

  private:
    // Size
    math::CVector3 m_v3Size = math::CVector3::One;

    // Min - Max
    math::CVector3 m_v3Max = math::CVector3::Zero;
    math::CVector3 m_v3Min = math::CVector3::Zero;

    // OBB
    math::CVector3 m_v3Right = math::CVector3::Right;
    math::CVector3 m_v3Up = math::CVector3::Up;
    math::CVector3 m_v3Forward = math::CVector3::Forward;

    // Extents
    std::vector<math::CVector3> m_v3Extents;
    std::vector<render::graphics::CPrimitive*> m_vctPrimitives;

    bool m_bOBBEnabled = true;
    bool m_bDebugMode = true;
  };
}


#pragma once
#include "Collider.h"
#include "Libs/Math/Vector3.h"

namespace render { namespace graphics { class CPrimitive; } }
namespace collision { class CSphereCollider; }

namespace collision
{
  class CBoxCollider final : public CCollider
  {
  public:
    CBoxCollider(void* _pOwner);
    virtual ~CBoxCollider();

    virtual bool CheckCollision(const CCollider& _pOther, SHitEvent& _oHitEvent_) override;
    virtual bool IntersectRay(const physics::CRay& _oRay, SHitEvent& _oHitEvent_, const float& _fMaxDistance) override;
    virtual void RecalculateCollider() override;

    inline void SetOBBEnabled(bool _bEnabled) { m_bOBBEnabled = _bEnabled; }
    inline bool IsOBBEnabled() const { return m_bOBBEnabled; }

    inline const math::CVector3& GetSize() const { return m_v3Size; }
    inline void SetSize(const math::CVector3& _v3Size) { m_v3Size = _v3Size; RecalculateCollider(); }

    inline math::CVector3 GetCenter() const { return math::CVector3(m_v3Max + m_v3Min) * 0.5f; }
    inline math::CVector3 GetHalfSize() const { return math::CVector3(m_v3Max - m_v3Min) * 0.5f; }

    inline const math::CVector3& GetMax() const { return m_v3Max; }
    inline const math::CVector3& GetMin() const { return m_v3Min; }

    inline const std::vector<math::CVector3>& GetExtents() const { return m_v3Extents; }
    std::vector<math::CVector3> GetAxisDirectors() const;

    inline const math::CVector3& GetRightAxis() const { return m_v3Right; }
    inline const math::CVector3& GetUpAxis() const { return m_v3Up; }
    inline const math::CVector3& GetForwardAxis() const { return m_v3Forward; }

    virtual void DrawDebug() override;

  private:
    bool CheckOBBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const;
    bool CheckAABBCollision(const CBoxCollider* _pOther, SHitEvent& _oHitEvent_) const;

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
    bool m_bOBBEnabled = true;

    // Extents
    std::vector<math::CVector3> m_v3Extents;
    std::vector<render::graphics::CPrimitive*> m_vctPrimitives;
  };
}


#pragma once
#include "Collider.h"

namespace collision { class CSphereCollider; }
namespace collision { class CBoxCollider; }

namespace collision
{
  class CCapsuleCollider final : public CCollider
  {
  public:
    CCapsuleCollider(void* _pOwner) : CCollider(EColliderType::CAPSULE_COLLIDER, _pOwner) {}
    ~CCapsuleCollider() {}

    virtual bool CheckCollision(const CCollider& _pOther, THitEvent& _oHitEvent_) override;
    virtual bool IntersectRay(const physics::CRay& _oRay, THitEvent& _oHitEvent_, const float& _fMaxDistance) override;
    virtual void RecalculateCollider() override;

    inline math::CVector3 GetWorldPos() const { return GetPos() + GetLocalCenter(); }
    inline const math::CVector3& GetLocalCenter() const { return m_v3LocalCenter; }
    inline void SetLocalCenter(const math::CVector3& _v3LocalCenter) { m_v3LocalCenter = _v3LocalCenter; }

    inline const float GetRadius() const { return m_fRadius; }
    void SetRadius(float _fRadius);
    inline const float GetHeight() const { return m_fHeight; }
    void SetHeight(float _fHeight);

    inline const math::CVector3& GetStartSegmentPoint() const { return m_v3StartSegmentPoint; }
    inline const math::CVector3& GetEndSegmentPoint() const { return m_v3EndSegmentPoint; }
    inline const math::CVector3& GetSegmentDir() const { return m_v3SegmentDir; }

    inline const math::CVector3& GetOrientedAxis() const { return m_v3OrientedAxis; }
    inline void SetOrientedAxis(math::CVector3 _v3Axis) { m_v3OrientedAxis = _v3Axis; }

    virtual void DrawDebug() override;

  private:
    bool CheckCapsuleCollision(const CCapsuleCollider* _pOther, THitEvent& _oHitEvent_) const;
    bool CheckSphereCollision(const CSphereCollider* _pOther, THitEvent& _oHitEvent_) const;
    bool CheckOBBCollision(const CBoxCollider* _pOther, THitEvent& _oHitEvent_) const;
    bool CheckBoxCollision(const CBoxCollider* _pOther, THitEvent& _oHitEvent_) const;

  private:
    // Capsule properties
    math::CVector3 m_v3LocalCenter = math::CVector3::Zero; // Local displacement
    math::CVector3 m_v3OrientedAxis = math::CVector3::Up;
    float m_fHeight = 2.0f;
    float m_fRadius = 0.5f;

    // Segment
    math::CVector3 m_v3EndSegmentPoint = math::CVector3::Zero;
    math::CVector3 m_v3StartSegmentPoint = math::CVector3::Zero;
    math::CVector3 m_v3SegmentDir = math::CVector3::Zero;
  };
}



#pragma once
#include "Libs/Math/Transform.h"
#include "Engine/Utils/Ray.h"
#include "Libs/Utils/Delegate.h"

// Interface
namespace collision
{
  enum ECollisionMask
  {
    DEFAULT = 1 << 0,
    PHYSICS = 1 << 1,
    FLOOR = 1 << 2,
    WATER = 1 << 3,
    PLAYER = 1 << 4,
    ENEMY = 1 << 5,
    INVALID = -1
  };

  enum class EColliderType
  {
    BOX_COLLIDER,
    SPHERE_COLLIDER,
    CAPSULE_COLLIDER,
    INVALID
  };

  struct THitEvent
  {
    math::CVector3 Normal = math::CVector3::Zero;
    math::CVector3 ImpactPoint = math::CVector3::Zero;
    float Depth = 0.0f;
    float Distance = 0.0f;
    void* Object = nullptr;
  };

  class CCollider
  {
  public:
    typedef utils::CDelegate<void(const collision::THitEvent&)> TOnCollisionEvent;
    friend class CCollisionManager;

  public:
    CCollider(EColliderType _eColliderType, void* _pOwner) : m_eColliderType(_eColliderType), m_pOwner(_pOwner) {}
    virtual ~CCollider() {}

    virtual bool CheckCollision(const collision::CCollider&, collision::THitEvent&) = 0;
    virtual bool IntersectRay(const physics::CRay&, collision::THitEvent&, const float&) = 0;
    virtual void RecalculateCollider() = 0;
    virtual void DrawDebug() = 0;

    inline void* GetOwner() const { return m_pOwner; }
    inline const collision::EColliderType& GetType() const { return m_eColliderType; }

    inline void SetCollisionMask(const ECollisionMask& _eCollisionMask) { m_eCollisionMask = _eCollisionMask; }
    inline const collision::ECollisionMask& GetCollisionMask() const { return m_eCollisionMask; }

    inline math::CVector3 GetPos() const { return m_oTransform.GetPos(); }
    inline void SetPos(const math::CVector3& _v3Pos) { m_oTransform.SetPos(_v3Pos); }
    inline math::CVector3 GetRot() const { return m_oTransform.GetRot(); }
    inline void SetRot(const math::CVector3& _v3Rot) { m_oTransform.SetRot(_v3Rot); }
    inline math::CVector3 GetScl() const { return m_oTransform.GetScl(); }
    inline void SetScl(const math::CVector3& _v3Scale) { m_oTransform.SetScl(_v3Scale); }

    // Bind
    inline void SetOnCollisionEnter(const TOnCollisionEvent& _oDelegate) { m_oOnCollisionEnter = _oDelegate; }
    inline void SetOnCollisionStay(const TOnCollisionEvent& _oDelegate) { m_oOnCollisionStay = _oDelegate; }
    inline void SetOnCollisionExit(const TOnCollisionEvent& _oDelegate) { m_oOnCollisionExit = _oDelegate; }

  private:
    TOnCollisionEvent m_oOnCollisionEnter;
    TOnCollisionEvent m_oOnCollisionStay;
    TOnCollisionEvent m_oOnCollisionExit;

    math::CTransform m_oTransform = math::CTransform();
    EColliderType m_eColliderType = EColliderType::INVALID;
    ECollisionMask m_eCollisionMask = ECollisionMask::DEFAULT;

  private:
    void* m_pOwner = nullptr;
  };
}


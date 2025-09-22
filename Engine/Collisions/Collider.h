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

  struct SHitEvent
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
    typedef utils::CDelegate<void(const collision::SHitEvent&)> TOnCollisionEvent;
    friend class CCollisionManager;

  public:
    explicit CCollider(EColliderType _eColliderType, void* _pOwner)
    : m_eColliderType(_eColliderType), m_pOwner(_pOwner) {}
    virtual ~CCollider() {}

    virtual bool CheckCollision(const collision::CCollider&, collision::SHitEvent&) = 0;
    virtual bool IntersectRay(const physics::CRay&, collision::SHitEvent&, const float&) = 0;
    virtual void RecalculateCollider() = 0;
    virtual void DrawDebug() = 0;

    inline void* GetOwner() const { return m_pOwner; }
    inline const collision::EColliderType& GetType() const { return m_eColliderType; }

    inline void SetCollisionMask(const ECollisionMask& _eCollisionMask) { m_eCollisionMask = _eCollisionMask; }
    inline const collision::ECollisionMask& GetCollisionMask() const { return m_eCollisionMask; }

    inline const math::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
    inline void SetPosition(const math::CVector3& _v3Pos) { m_oTransform.SetPosition(_v3Pos); }
    inline const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
    inline void SetRotation(const math::CVector3& _v3Rot) { m_oTransform.SetRotation(_v3Rot); }
    inline const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }
    inline void SetScale(const math::CVector3& _v3Scale) { m_oTransform.SetScale(_v3Scale); }

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


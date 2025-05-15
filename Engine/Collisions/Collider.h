#pragma once
#include "Libs/Math/Transform.h"
#include "Engine/Physics/Rigidbody.h"

// Interface
namespace collisions
{
  enum EColliderType { BOX_COLLIDER, SPHERE_COLLIDER, INVALID };

  struct SHitEvent
  {
    math::CVector3 Normal = math::CVector3::Zero;
    math::CVector3 ImpactPoint = math::CVector3::Zero;
    float Depth = 0.0f;
    void* Object = nullptr;
  };

  class CCollider
  {
  public:
    typedef utils::CDelegate<void(const SHitEvent&)> TOnCollisionEvent;
    friend class CCollisionManager;

  public:
    explicit CCollider(EColliderType _eColliderType, void* _pOwner) 
    : m_eColliderType(_eColliderType), m_pOwner(_pOwner) {}
    virtual ~CCollider() {}

    virtual bool CheckCollision(const CCollider&, SHitEvent&) = 0;
    virtual void RecalculateCollider() = 0;

    const EColliderType& GetType() const { return m_eColliderType; }
    void* GetOwner() const { return m_pOwner; }

    const math::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
    void SetPosition(const math::CVector3& _v3Pos) { m_oTransform.SetPosition(_v3Pos); }
    const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
    void SetRotation(const math::CVector3& _v3Rot) { m_oTransform.SetRotation(_v3Rot); }
    const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }
    void SetScale(math::CVector3& _v3Scale) { m_oTransform.SetScale(_v3Scale); }

    // Notifications
    void SetOnCollisionEnter(const TOnCollisionEvent& _oDelegate) { m_oOnCollisionEnter = _oDelegate; }
    void SetOnCollisionStay(const TOnCollisionEvent& _oDelegate) { m_oOnCollisionStay = _oDelegate; }
    void SetOnCollisionExit(const TOnCollisionEvent& _oDelegate) { m_oOnCollisionExit = _oDelegate; }

  private:
    TOnCollisionEvent m_oOnCollisionEnter;
    TOnCollisionEvent m_oOnCollisionStay;
    TOnCollisionEvent m_oOnCollisionExit;

    math::CTransform m_oTransform = math::CTransform();
    EColliderType m_eColliderType = INVALID;

  private:
    void* m_pOwner = nullptr;
  };
}


#pragma once
#include "Libs/Maths/Transform.h"
#include "Engine/Physics/Rigidbody.h"

// Interface
namespace collisions
{
  enum EColliderType { BOX_COLLIDER, SPHERE_COLLIDER, INVALID };

  struct SHitEvent
  {
    maths::CVector3 Normal = maths::CVector3::Zero;
    maths::CVector3 ImpactPoint = maths::CVector3::Zero;
    float Depth = 0.0f;
  };

  class CCollider
  {
  public:
    typedef utils::CDelegate<void(const collisions::CCollider*, const SHitEvent&)> TOnCollisionEvent;
    friend class CCollisionManager;

  public:
    explicit CCollider(EColliderType _eColliderType, void* _pOwner) : 
    m_eColliderType(_eColliderType), m_pOwner(_pOwner) {}
    virtual ~CCollider() {}

    virtual bool CheckCollision(const CCollider&, SHitEvent&) = 0;
    virtual void RecalculateCollider() = 0;

    const void* GetOwner() const { return m_pOwner; }
    const EColliderType& GetType() const { return m_eColliderType; }

    const maths::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
    void SetPosition(const maths::CVector3& _v3Pos) { m_oTransform.SetPosition(_v3Pos); }
    const maths::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
    void SetRotation(const maths::CVector3& _v3Rot) { m_oTransform.SetRotation(_v3Rot); }
    const maths::CVector3& GetScale() const { return m_oTransform.GetScale(); }
    void SetScale(maths::CVector3& _v3Scale) { m_oTransform.SetScale(_v3Scale); }

    // Notifications
    void SetOnCollisionEnter(const TOnCollisionEvent& _oDelegate) { m_oOnCollisionEnter = _oDelegate; }
    void SetOnCollisionStay(const TOnCollisionEvent& _oDelegate) { m_oOnCollisionStay = _oDelegate; }
    void SetOnCollisionExit(const TOnCollisionEvent& _oDelegate) { m_oOnCollisionExit = _oDelegate; }

  private:
    TOnCollisionEvent m_oOnCollisionEnter;
    TOnCollisionEvent m_oOnCollisionStay;
    TOnCollisionEvent m_oOnCollisionExit;

  private:
    void* m_pOwner = nullptr;
    maths::CTransform m_oTransform = maths::CTransform();
    EColliderType m_eColliderType = INVALID;
  };
}


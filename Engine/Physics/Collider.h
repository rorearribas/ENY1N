#pragma once
#include "Libs/Maths/Transform.h"

// Interface
namespace physics
{
  enum EColliderType { BOX_COLLIDER, SPHERE_COLLIDER, INVALID };

  class CCollider
  {
  public:
    explicit CCollider(EColliderType _eColliderType) : m_eColliderType(_eColliderType) {}
    virtual ~CCollider() {}

    virtual bool CheckCollision(const CCollider&) = 0;
    virtual void RecalculateCollider() = 0;

    const EColliderType& GetType() const { return m_eColliderType; }

    const maths::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
    void SetPosition(const maths::CVector3& _v3Pos) { m_oTransform.SetPosition(_v3Pos); }
    const maths::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
    void SetRotation(const maths::CVector3& _v3Rot) { m_oTransform.SetRotation(_v3Rot); }
    const maths::CVector3& GetScale() const { return m_oTransform.GetScale(); }
    void SetScale(maths::CVector3& _v3Scale) { m_oTransform.SetScale(_v3Scale); }

  protected:
    EColliderType m_eColliderType = INVALID;
    maths::CTransform m_oTransform;

  };
}


#pragma once
#include "Libs/Maths/Vector3.h"
#include "Game/ETT/Entity.h"

namespace physics
{
  class CRigidbody
  {
  public:
    explicit CRigidbody(game::CEntity* _pOwner) : m_pTargetEntity(_pOwner) {}
    ~CRigidbody() {}

    void SetStatic(bool _bStatic) { m_bIsStatic = _bStatic; }
    const bool& IsStatic() const { return m_bIsStatic; }
    void SetKinematic(bool _bKinematic) { m_bIsKinematic = _bKinematic; }
    const bool& IsKinematic() const { return m_bIsKinematic; }

    void Clear() 
    {
      m_v3Velocity = maths::CVector3::Zero; 
      m_v3Acceleration = maths::CVector3::Zero;
    }

    const maths::CVector3& GetVelocity() { return m_v3Acceleration; }
    void AddVelocity(const maths::CVector3& _v3Velocity) { m_v3Velocity += _v3Velocity; }
    const maths::CVector3& GetAcceleration() { return m_v3Acceleration; }
    void AddAcceleration(const maths::CVector3& _v3Accelaration) { m_v3Acceleration += _v3Accelaration; }

    void SetMass(float _fValue) { m_fMass = _fValue; }
    const float& GetMass() const { return m_fMass; }
    void SetDrag(float _fValue) { m_fDrag = _fValue; }
    const float& GetDrag() const { return m_fDrag; }

    void SetImpactPoint(const maths::CVector3& _v3Point)
    {
      m_v3ImpactPoint = _v3Point;
    }

  private:
    friend class CPhysicsManager;

    game::CEntity* m_pTargetEntity = nullptr;
    maths::CVector3 m_v3Velocity = maths::CVector3::Zero;
    maths::CVector3 m_v3Acceleration = maths::CVector3::Zero;
    maths::CVector3 m_v3ImpactPoint = maths::CVector3::Zero;

    float m_fMass = 1.0f;
    float m_fDrag = 0.0f;

    bool m_bIsStatic = true;
    bool m_bIsKinematic = true; // WIP
  };
}



#pragma once
#include "Libs/Maths/Vector3.h"
#include "Libs/Utils/Delegate.h"
#include "Engine/Collisions/Collider.h"

namespace physics
{
  enum ERigidbodyType { KINEMATIC, DYNAMIC };
  enum ERigidbodyState { IN_THE_AIR, COLLIDING };

  class CRigidbody
  {
  public:
    typedef utils::CDelegate<void(const maths::CVector3&)> TOnVelocityChangedDelegate;
    typedef utils::CDelegate<void(const maths::CVector3&)> TOnRotationChangedDelegate;

  public:
    explicit CRigidbody(const ERigidbodyType _eRigidbodyType = KINEMATIC) : m_eRigidbodyType(_eRigidbodyType) {}
    ~CRigidbody() {}

    void SetRigidbodyType(ERigidbodyType _eRigidbodyType);
    const ERigidbodyType& GetRigidbodyType() const { return m_eRigidbodyType; }
    void SetCurrentState(ERigidbodyState _eRigidbodyState) { m_eRidibodyState = _eRigidbodyState; }
    const ERigidbodyState& GetRigidbodyState() const { return m_eRidibodyState; }

    void AddForce(const maths::CVector3& _v3Force);
    const maths::CVector3& GetAcceleration() { return m_v3Acceleration; }
    void AddTorque(const maths::CVector3& _v3Torque);
    const maths::CVector3& GetTorque() const { return m_v3Torque; }

    void SetAngularVelocity(const maths::CVector3& _v3Velocity) { m_v3AngularVelocity = _v3Velocity; }
    const maths::CVector3& GetAngularVelocity() const { return m_v3AngularVelocity; }
    void SetVelocity(const maths::CVector3& _v3Velocity) { m_v3Velocity = _v3Velocity; }
    const maths::CVector3& GetVelocity() { return m_v3Velocity; }

    void SetMass(float _fValue) { m_fMass = _fValue; }
    const float& GetMass() const { return m_fMass; }

    // Notifications
    void SetOnVelocityChangedDelegate(const TOnVelocityChangedDelegate& _oDelegate) { m_OnVelocityChangedDelegate = _oDelegate; }
    void SetOnRotationChangedDelegate(const TOnRotationChangedDelegate& _oDelegate) { m_OnRotationChangedDelegate = _oDelegate; }

  private:
    friend class CPhysicsManager;

  private:
    maths::CVector3 m_v3Velocity = maths::CVector3::Zero;
    maths::CVector3 m_v3AngularVelocity = maths::CVector3::Zero;

    maths::CVector3 m_v3Acceleration = maths::CVector3::Zero;
    maths::CVector3 m_v3Torque = maths::CVector3::Zero;

    TOnVelocityChangedDelegate m_OnVelocityChangedDelegate;
    TOnRotationChangedDelegate m_OnRotationChangedDelegate;

    ERigidbodyType m_eRigidbodyType = KINEMATIC;
    ERigidbodyState m_eRidibodyState = IN_THE_AIR;

    float m_fInertia = 1.0f;
    float m_fMass = 1.0f;

    float m_fLinearDrag = 0.01f;
    float m_fAngularDrag = 0.01f;
  };
}



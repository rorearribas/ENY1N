#pragma once
#include "Libs/Math/Vector3.h"
#include "Libs/Utils/Delegate.h"
#include "Engine/Collisions/Collider.h"

namespace physics
{
  enum ERigidbodyType { KINEMATIC, DYNAMIC };
  enum ERigidbodyState { IN_THE_AIR, COLLIDING };

  class CRigidbody
  {
  public:
    typedef utils::CDelegate<void(const math::CVector3&)> TOnVelocityChangedDelegate;
    typedef utils::CDelegate<void(const math::CVector3&)> TOnRotationChangedDelegate;

  public:
    explicit CRigidbody(const ERigidbodyType _eRigidbodyType = KINEMATIC) : m_eRigidbodyType(_eRigidbodyType) {}
    ~CRigidbody() {}

    void SetRigidbodyType(ERigidbodyType _eRigidbodyType);
    const ERigidbodyType& GetRigidbodyType() const { return m_eRigidbodyType; }
    void SetCurrentState(ERigidbodyState _eRigidbodyState) { m_eRidibodyState = _eRigidbodyState; }
    const ERigidbodyState& GetRigidbodyState() const { return m_eRidibodyState; }

    void AddForce(const math::CVector3& _v3Force);
    const math::CVector3& GetAcceleration() { return m_v3Acceleration; }
    void AddTorque(const math::CVector3& _v3Torque);
    const math::CVector3& GetTorque() const { return m_v3Torque; }

    void SetAngularVelocity(const math::CVector3& _v3Velocity) { m_v3AngularVelocity = _v3Velocity; }
    const math::CVector3& GetAngularVelocity() const { return m_v3AngularVelocity; }
    void SetVelocity(const math::CVector3& _v3Velocity) { m_v3Velocity = _v3Velocity; }
    const math::CVector3& GetVelocity() { return m_v3Velocity; }

    void SetMass(float _fValue) { m_fMass = _fValue; }
    const float& GetMass() const { return m_fMass; }

    // Notifications
    void SetOnVelocityChangedDelegate(const TOnVelocityChangedDelegate& _oDelegate) { m_OnVelocityChangedDelegate = _oDelegate; }
    void SetOnRotationChangedDelegate(const TOnRotationChangedDelegate& _oDelegate) { m_OnRotationChangedDelegate = _oDelegate; }

  private:
    friend class CPhysicsManager;

  private:
    math::CVector3 m_v3Velocity = math::CVector3::Zero;
    math::CVector3 m_v3AngularVelocity = math::CVector3::Zero;

    math::CVector3 m_v3Acceleration = math::CVector3::Zero;
    math::CVector3 m_v3Torque = math::CVector3::Zero;

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



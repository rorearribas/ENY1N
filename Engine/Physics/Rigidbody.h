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
  public:
    explicit CRigidbody(const ERigidbodyType _eRigidbodyType = KINEMATIC) : m_eRigidbodyType(_eRigidbodyType) {}
    ~CRigidbody() {}

    const ERigidbodyType& GetType() const { return m_eRigidbodyType; }
    void SetRigidbodyType(ERigidbodyType _eRigidbodyType);

    void SetCurrentState(ERigidbodyState _eRigidbodyState) { m_eRidibodyState = _eRigidbodyState; }
    const ERigidbodyState& GetRigidbodyState() const { return m_eRidibodyState; }

    void AddForce(const maths::CVector3& _v3Force) { m_v3AccumulatedForces += _v3Force; }
    const maths::CVector3& GetAcceleration() { return m_v3Acceleration; }

    const maths::CVector3& GetVelocity() { return m_v3Velocity; }
    void ResetVelocity() { m_v3Velocity = maths::CVector3::Zero; }

    void SetMass(float _fValue) { m_fMass = _fValue; }
    const float& GetMass() const { return m_fMass; }
    void SetDrag(float _fValue) { m_fDrag = _fValue; }
    const float& GetDrag() const { return m_fDrag; }

    // Notifications
    void SetOnVelocityChangedDelegate(const TOnVelocityChangedDelegate& _oDelegate) { OnVelocityChangedDelegate = _oDelegate; }

  private:
    friend class CPhysicsManager;

  private:
    maths::CVector3 m_v3Velocity = maths::CVector3::Zero;
    maths::CVector3 m_v3Acceleration = maths::CVector3::Zero;
    maths::CVector3 m_v3AccumulatedForces = maths::CVector3::One;

    TOnVelocityChangedDelegate OnVelocityChangedDelegate;
    ERigidbodyType m_eRigidbodyType = KINEMATIC;
    ERigidbodyState m_eRidibodyState = IN_THE_AIR;

    float m_fMass = 1.0f;
    float m_fDrag = 0.0f;

    bool m_bIsOnCollision = false;
  };
}



#pragma once
#include "Libs/Maths/Vector3.h"
#include "Libs/Utils/Delegate.h"
#include "Engine/Collisions/Collider.h"

namespace physics
{
  enum ERigidbodyType
  {
    KINEMATIC,
    DYNAMIC
  };

  class CRigidbody
  {
  public:
    typedef utils::CDelegate<void(const maths::CVector3&)> TOnVelocityChangedDelegate;
  public:
    explicit CRigidbody(const ERigidbodyType _eRigidbodyType = KINEMATIC) : m_eRigidbodyType(_eRigidbodyType) {}
    ~CRigidbody() {}

    void SetRigidbodyType(ERigidbodyType _eRigidbodyType) { m_eRigidbodyType = _eRigidbodyType; }
    const ERigidbodyType& GetType() const { return m_eRigidbodyType; }

    void AddForce(maths::CVector3& _v3Force) { m_v3AccumulatedForces += _v3Force; }
    const maths::CVector3& GetVelocity() { return m_v3Velocity; }
    const maths::CVector3& GetAcceleration() { return m_v3Acceleration; }

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

    ERigidbodyType m_eRigidbodyType = KINEMATIC;
    TOnVelocityChangedDelegate OnVelocityChangedDelegate;

    float m_fMass = 1.0f;
    float m_fDrag = 0.0f;
  };
}



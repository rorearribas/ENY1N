#include "Rigidbody.h"

namespace physics
{
  void CRigidbody::SetRigidbodyType(ERigidbodyType _eRigidbodyType)
  {
    // Reset velocity
    ResetVelocity();

    // Set new state
    m_eRigidbodyType = _eRigidbodyType;
  }
  // ------------------------------------
  void CRigidbody::AddForce(const maths::CVector3& _v3Force)
  {
    if (m_fMass > 0.0f)
    {
      m_v3Acceleration += _v3Force / m_fMass;
    }
  }
}
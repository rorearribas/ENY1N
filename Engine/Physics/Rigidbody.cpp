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
}
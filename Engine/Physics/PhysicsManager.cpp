#include "PhysicsManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Rigidbody.h"
#include "Game/ETT/Entity.h"

namespace physics
{
  const float CPhysicsManager::s_fGravityForce(-9.8f);

  void CPhysicsManager::Update(float _fDeltaTime)
  {
    for (uint32_t uIndex = 0; uIndex < m_vctRigidbodys.CurrentSize(); ++uIndex)
    {
      physics::CRigidbody* pRigidbody = m_vctRigidbodys[uIndex];
      bool bUpdate = pRigidbody->GetType() == physics::ERigidbodyType::DYNAMIC;
      if (bUpdate)
      {
        // Apply gravity force (base)
        maths::CVector3 v3Gravity(0.0f, s_fGravityForce, 0.0f);
        pRigidbody->m_v3Acceleration += v3Gravity;

        // Compute velocity
        pRigidbody->m_v3Velocity += pRigidbody->GetAcceleration() * _fDeltaTime;

        // Set new position
        maths::CVector3 vCurrentVelocity = pRigidbody->m_v3Velocity * _fDeltaTime;

        // Notify
        pRigidbody->OnVelocityChangedDelegate.Execute(vCurrentVelocity);

        // Reset acceleration
        pRigidbody->m_v3Acceleration = maths::CVector3::Zero;
      }
    }
  }
  // ------------------------------------
  CRigidbody* CPhysicsManager::CreateRigidbody(ERigidbodyType _eRigidbodyType)
  {
    if (m_vctRigidbodys.CurrentSize() >= m_vctRigidbodys.GetMaxSize())
    {
      std::cout << "You have reached maximum colliders" << std::endl;
      return nullptr;
    }
    return m_vctRigidbodys.CreateItem(_eRigidbodyType);
  }
  // ------------------------------------
  void CPhysicsManager::DestroyRigidbody(CRigidbody*& _pRigidbody)
  {
    bool bOk = m_vctRigidbodys.RemoveItem(_pRigidbody);
    if (!bOk) { std::cout << "Error: Failed to remove rigidbody" << std::endl; }
    _pRigidbody = nullptr;
  }
  // ------------------------------------
  void CPhysicsManager::Flush()
  {
    m_vctRigidbodys.ClearAll();
  }
}
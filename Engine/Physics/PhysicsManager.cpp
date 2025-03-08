#include "PhysicsManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Rigidbody.h"
#include "Game/ETT/Entity.h"

namespace physics
{
  namespace internal_physics_manager
  {
    const float s_fGravityForce(-9.8f);
  }

  // ------------------------------------
  CPhysicsManager::~CPhysicsManager()
  {
    Clear();
  }
  // ------------------------------------
  void CPhysicsManager::Update(float _fDeltaTime)
  {
    for (uint32_t uIndex = 0; uIndex < m_vctRigidbodys.CurrentSize(); ++uIndex)
    {
      physics::CRigidbody* pRigidbody = m_vctRigidbodys[uIndex];
      bool bUpdate = pRigidbody->GetType() == physics::ERigidbodyType::DYNAMIC;
      if (bUpdate)
      {
        const ERigidbodyState& eRigidbodyState = pRigidbody->GetRigidbodyState();
        if (eRigidbodyState == ERigidbodyState::IN_THE_AIR)
        {
          // Apply gravity force (base)
          maths::CVector3 v3Gravity(0.0f, internal_physics_manager::s_fGravityForce, 0.0f);
          pRigidbody->m_v3Acceleration += v3Gravity;
        }

        // Cache current velocity
        maths::CVector3 vCurrentVelocity = pRigidbody->m_v3Velocity;

        // Increase velocity
        pRigidbody->m_v3Velocity += pRigidbody->GetAcceleration() * _fDeltaTime;

        // Valid new velicity
        if (vCurrentVelocity != pRigidbody->m_v3Velocity)
        {
          // Compute new displacement
          maths::CVector3 vDisplacement = pRigidbody->m_v3Velocity * _fDeltaTime;

          // Notify
          pRigidbody->OnVelocityChangedDelegate.Execute(vDisplacement);
        }

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
  void CPhysicsManager::Clear()
  {
    m_vctRigidbodys.ClearAll();
  }
}
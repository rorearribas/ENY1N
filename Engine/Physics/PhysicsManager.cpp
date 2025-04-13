#include "PhysicsManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Rigidbody.h"
#include "Game/ETT/Entity.h"
#include "Libs/Time/TimeManager.h"

namespace physics
{
  namespace internal_physics_manager
  {
    const float s_fGravityForce(9.8f);
    static maths::CVector3 s_v3GravityForce(0.0f, -internal_physics_manager::s_fGravityForce, 0.0f);
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
      bool bDynamic = pRigidbody->GetRigidbodyType() == physics::ERigidbodyType::DYNAMIC;
      if(!bDynamic) continue;

      // Apply gravity force
      pRigidbody->m_v3Acceleration += internal_physics_manager::s_v3GravityForce;

      // Add acceleration
      if (!pRigidbody->m_v3Acceleration.Equal(maths::CVector3::Zero))
      {
        pRigidbody->m_v3Velocity += pRigidbody->m_v3Acceleration * _fDeltaTime;
      }

      // Decrease velocity
      bool bInTheAir = pRigidbody->GetRigidbodyState() == physics::ERigidbodyState::IN_THE_AIR;
      const float fExpCoefficient = bInTheAir ? 0.1f : 0.2f;
      pRigidbody->m_v3Velocity *= expf(-fExpCoefficient * _fDeltaTime);

      // Notify displacement -> i extracted this equation from the internet
      maths::CVector3 v3Displacement = (pRigidbody->m_v3Velocity * _fDeltaTime) + (pRigidbody->m_v3Acceleration * _fDeltaTime * _fDeltaTime * 0.5f);
      pRigidbody->m_OnVelocityChangedDelegate(v3Displacement);

      // Compute angular displacement
      if (!pRigidbody->m_v3Torque.Equal(maths::CVector3::Zero))
      {
        pRigidbody->m_v3AngularVelocity += (pRigidbody->m_v3Torque / pRigidbody->m_fInertia) * _fDeltaTime;
      }

      // Decrease angular velocity
      const float fAngularDrag = expf(-fExpCoefficient * _fDeltaTime);
      pRigidbody->m_v3AngularVelocity *= fAngularDrag;

      // Notify angular displacement
      pRigidbody->m_OnRotationChangedDelegate(pRigidbody->m_v3AngularVelocity * _fDeltaTime);

      // Reset acceleration + torque
      pRigidbody->m_v3Acceleration = maths::CVector3::Zero;
      pRigidbody->m_v3Torque = maths::CVector3::Zero;
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
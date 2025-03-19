#include "PhysicsManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Rigidbody.h"
#include "Game/ETT/Entity.h"

namespace physics
{
  namespace internal_physics_manager
  {
    const float s_fGravityForce(9.8f);
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

      // Apply gravity force (base)
      maths::CVector3 v3Gravity(0.0f, -internal_physics_manager::s_fGravityForce, 0.0f);
      pRigidbody->m_v3Acceleration += v3Gravity * pRigidbody->m_fMass;

      // Apply drag/deceleration (Physics - REVIEW)
      const float fLinearDrag = powf(0.99f, _fDeltaTime * 60.0f);
      pRigidbody->m_v3Velocity *= fLinearDrag;
      const float fAngularDrag = powf(0.98f, _fDeltaTime * 60.0f);
      pRigidbody->m_v3AngularVelocity *= fAngularDrag;

      // Cache current velocity
      maths::CVector3 vCurrentVelocity = pRigidbody->m_v3Velocity;

      // Increase velocity
      pRigidbody->m_v3Velocity += pRigidbody->m_v3Acceleration * _fDeltaTime;

      // Valid new velocity
      if (!vCurrentVelocity.Equal(pRigidbody->m_v3Velocity))
      {
        // Compute new displacement
        maths::CVector3 vDisplacement = pRigidbody->m_v3Velocity * _fDeltaTime;
        // Notify
        pRigidbody->m_OnVelocityChangedDelegate(vDisplacement);
      }

      // Cache current angular velocity
      maths::CVector3 v3AngularVelocity = pRigidbody->m_v3AngularVelocity;

      // Compute angular displacement
      pRigidbody->m_v3AngularVelocity += (pRigidbody->m_v3Torque / pRigidbody->m_fInertia) * _fDeltaTime;

      if (!v3AngularVelocity.Equal(pRigidbody->m_v3AngularVelocity))
      {
        // Compute angular displacement
        maths::CVector3 vAngularDisplacement = pRigidbody->m_v3AngularVelocity * _fDeltaTime;
        // Notify
        pRigidbody->m_OnRotationChangedDelegate(vAngularDisplacement);
      }
      
      // Reset torque + acceleration
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
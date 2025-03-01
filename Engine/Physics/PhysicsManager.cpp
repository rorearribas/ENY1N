#include "PhysicsManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Rigidbody.h"
#include "Game/ETT/Entity.h"

namespace physics
{
  void CPhysicsManager::Update(float _fDeltaTime)
  {
    for (uint32_t uIndex = 0; uIndex < m_vctRigidbodys.CurrentSize(); ++uIndex)
    {
      physics::CRigidbody* pRigidbody = m_vctRigidbodys[uIndex];
      if (pRigidbody->IsStatic())
        continue;

      if(pRigidbody->m_pTargetEntity->GetPosition() == pRigidbody->m_v3ImpactPoint)
      continue;

      // Compute acceleration
      pRigidbody->AddAcceleration(maths::CVector3(0.0f, (-9.8f * pRigidbody->GetMass()), 0.0f) * _fDeltaTime);
      pRigidbody->AddVelocity(pRigidbody->GetAcceleration());
      // Add force to entity
      maths::CVector3 v3CurrentPosition = pRigidbody->m_pTargetEntity->GetPosition();
      v3CurrentPosition += pRigidbody->GetVelocity() * _fDeltaTime;
      pRigidbody->m_pTargetEntity->SetPosition(v3CurrentPosition);
    }
  }
  // ------------------------------------
  physics::CRigidbody* CPhysicsManager::CreateRigidbody(game::CEntity* _pEntity)
  {
    if (m_vctRigidbodys.CurrentSize() >= m_vctRigidbodys.GetMaxSize())
    {
      std::cout << "You have reached maximum colliders" << std::endl;
      return nullptr;
    }

    return m_vctRigidbodys.CreateItem(_pEntity);
  }
  // ------------------------------------
  void CPhysicsManager::DestroyRigidbody(physics::CRigidbody*& _pRigidbody)
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
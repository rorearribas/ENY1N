#include "RigidbodyComponent.h"
#include "Engine/Physics/PhysicsManager.h"
#include "Game/ETT/Entity.h"
#include "Libs/Macros/GlobalMacros.h"

namespace game
{
  CRigidbodyComponent::CRigidbodyComponent(physics::ERigidbodyType _eRigidbodyType) : CComponent()
  {
    CreateRigidbody(_eRigidbodyType);
  }
  // ------------------------------------
  void CRigidbodyComponent::Update(float _fDeltaTime)
  {
    // Rigidbody are being updated in the physics manager!
    Super::Update(_fDeltaTime);
  }
  // ------------------------------------
  void CRigidbodyComponent::DrawDebug()
  {
    // Not implemented yet!
  }
  // ------------------------------------
  void CRigidbodyComponent::SetRigidbodyType(physics::ERigidbodyType _eRigidbodyType)
  {
    assert(m_pRigidbody);
    m_pRigidbody->SetRigidbodyType(_eRigidbodyType);
  }
  // ------------------------------------
  void CRigidbodyComponent::OnCollisionEnter(const collisions::CCollider*)
  {
    m_pRigidbody->SetCurrentState(physics::ERigidbodyState::COLLIDING);
  }
  // ------------------------------------
  void CRigidbodyComponent::OnCollisionExit(const collisions::CCollider*)
  {
    m_pRigidbody->SetCurrentState(physics::ERigidbodyState::IN_THE_AIR);
    m_pRigidbody->ResetVelocity();
  }
  // ------------------------------------
  void CRigidbodyComponent::OnVelocityChanged(const maths::CVector3& _v3Velocity)
  { 
    CEntity* pOwner = GetOwner();
    if (pOwner)
    {
      maths::CVector3 vNewPosition = pOwner->GetPosition() + _v3Velocity;
      pOwner->SetPosition(vNewPosition);
    }
  }
  // ------------------------------------
  void CRigidbodyComponent::CreateRigidbody(physics::ERigidbodyType _eRigidbodyType)
  {
    Clean();

    // Create rigidbody
    m_pRigidbody = physics::CPhysicsManager::GetInstance()->CreateRigidbody(_eRigidbodyType);
    assert(m_pRigidbody);

    // Set notifications
    m_pRigidbody->SetOnVelocityChangedDelegate(physics::CRigidbody::TOnVelocityChangedDelegate(&CRigidbodyComponent::OnVelocityChanged, this));
  }
  // ------------------------------------
  void CRigidbodyComponent::Clean()
  {
    if (m_pRigidbody)
    {
      physics::CPhysicsManager::GetInstance()->DestroyRigidbody(m_pRigidbody);
    }
  }
}
#include "RigidbodyComponent.h"
#include "Engine/Physics/PhysicsManager.h"
#include "Game/ETT/Entity.h"
#include "Libs/Macros/GlobalMacros.h"

namespace game
{
  namespace internal_rigidbody_component
  {
    const float s_fMaxForce(100.0f);
  }

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
  void CRigidbodyComponent::OnCollisionEnter(const collisions::CCollider*, const collisions::SHitEvent& /*_oHitEvent*/)
  {
    /*  maths::CVector3 v3CurrentVelocity = maths::CVector3::Abs(m_pRigidbody->GetVelocity());
      m_pRigidbody->AddForce(_oHitEvent.Normal * (v3CurrentVelocity * internal_rigidbody_component::s_fMaxForce));*/
  }
  // ------------------------------------
  void CRigidbodyComponent::OnCollisionStay(const collisions::CCollider*, const collisions::SHitEvent& _oHitEvent)
  {
    maths::CVector3 v3CurrentVelocity = maths::CVector3::Abs(m_pRigidbody->GetVelocity());
    if (v3CurrentVelocity.X > 0.25f || v3CurrentVelocity.Y > 0.25f || v3CurrentVelocity.Z > 0.25f)
    {
      m_pRigidbody->AddForce(_oHitEvent.Normal * (v3CurrentVelocity * internal_rigidbody_component::s_fMaxForce));
    }
    if (GetOwner())
    {
      CEntity* pOwner = GetOwner();
      maths::CVector3 v3Offset = _oHitEvent.Normal * _oHitEvent.Depth;
      maths::CVector3 vNewPosition = pOwner->GetPosition() + v3Offset;
      pOwner->SetPosition(vNewPosition);
    }
    m_pRigidbody->ResetVelocity();
    m_pRigidbody->SetCurrentState(physics::ERigidbodyState::COLLIDING);
  }
  // ------------------------------------
  void CRigidbodyComponent::OnCollisionExit(const collisions::CCollider*, const collisions::SHitEvent& /*_oHitEvent*/)
  {
    m_pRigidbody->SetCurrentState(physics::ERigidbodyState::IN_THE_AIR);
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
#include "CollisionComponent.h"
#include "Engine/Physics/BoxCollider.h"
#include "Engine/Physics/SphereCollider.h"
#include "Game/ETT/Entity.h"

namespace game
{
  CCollisionComponent::CCollisionComponent(physics::EColliderType _eColliderType)
  {
    CreateCollider(_eColliderType);
  }
  // ------------------------------------
  void CCollisionComponent::CreateCollider(physics::EColliderType _eColliderType)
  {
    Clean();

    switch (_eColliderType)
    {
    case physics::BOX_COLLIDER:
      m_pCollider = new physics::CBoxCollider();
      break;
    case physics::SPHERE_COLLIDER:
      m_pCollider = new physics::CSphereCollider();
      break;
    default:
      break;
    }
  }
  // ------------------------------------
  void CCollisionComponent::Update(float _fDeltaTime)
  {
    Super::Update(_fDeltaTime);
  }
  // ------------------------------------
  void CCollisionComponent::SetPosition(const maths::CVector3& _v3Position)
  {
    if (m_pCollider)
    {
      m_pCollider->SetPosition(_v3Position);
    }
  }
  // ------------------------------------
  const maths::CVector3& CCollisionComponent::GetPosition() const
  {
    return m_pCollider ? m_pCollider->GetPosition() : m_pOwner->GetPosition();
  }
  // ------------------------------------
  void CCollisionComponent::OnPositionChanged(const maths::CVector3& _v3Pos)
  {
    if (m_pCollider)
    {
      m_pCollider->SetPosition(_v3Pos);
    }
  }
  // ------------------------------------
  void CCollisionComponent::Clean()
  {
    if (m_pCollider)
    {
      delete m_pCollider;
      m_pCollider = nullptr;
    }
  }
  // ------------------------------------
  void CCollisionComponent::DrawDebug()
  {

  }
}



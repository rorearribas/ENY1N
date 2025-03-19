#include "RigidbodyComponent.h"
#include "Game/ETT/Entity.h"
#include "Engine/Physics/PhysicsManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/ImGui/imgui.h"

namespace game
{
  namespace internal_rigidbody_component
  {
    const float s_fMaxReboundForce(50.0f);
    const float s_fMaxAngularForce(1000.0f);
  }
  // ------------------------------------
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
    ImGui::Spacing();
    std::string sOwnerName = GetOwner() ? GetOwner()->GetName() : std::string();
    if (!m_pRigidbody) return;

    // Generate unique ids
    std::string sTitle = "RIGIDBODY";
    std::string sMass = "Mass" + std::string("##" + sOwnerName);
    std::string sDrag = "Drag" + std::string("##" + sOwnerName);
    std::string sKinematic = "Kinematic" + std::string("##" + sOwnerName);
    std::string sAddForce = "Add Force" + std::string("##" + sOwnerName);

    ImGui::Text(sTitle.c_str());

    float fMass = GetMass();
    float fDrag = GetDrag();
    bool bKinematic = GetRigidbodyType() == physics::ERigidbodyType::KINEMATIC;

    ImGui::InputFloat(sMass.c_str(), &fMass);
    ImGui::InputFloat(sDrag.c_str(), &fDrag);
    ImGui::Checkbox(sKinematic.c_str(), &bKinematic);

    if (ImGui::Button(sAddForce.c_str()))
    {
      if (m_pRigidbody)
      {
        m_pRigidbody->AddForce(maths::CVector3(0.0, 0.0f, 1000.0f));
      }
    }

    // Apply rigidbody cfg
    if (m_pRigidbody)
    {
      SetMass(fMass);
      SetDrag(fDrag);
      physics::ERigidbodyType eRbType = bKinematic ? physics::ERigidbodyType::KINEMATIC : physics::ERigidbodyType::DYNAMIC;
      if (eRbType != GetRigidbodyType()) { SetRigidbodyType(eRbType); }
    }

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
    m_pRigidbody->SetCurrentState(physics::ERigidbodyState::COLLIDING);
  }
  // ------------------------------------
  void CRigidbodyComponent::OnCollisionStay(const collisions::CCollider*, const collisions::SHitEvent& _oHitEvent)
  {
    maths::CVector3 v3CurrentVelocity = maths::CVector3::Abs(m_pRigidbody->GetVelocity());
    if (!v3CurrentVelocity.Equal(maths::CVector3::Zero, 0.1f))
    {
      m_pRigidbody->AddForce(_oHitEvent.Normal * (v3CurrentVelocity * internal_rigidbody_component::s_fMaxReboundForce));
      maths::CVector3 v3Cross = _oHitEvent.ImpactPoint.CrossProduct(_oHitEvent.Normal).Normalize();
      maths::CVector3 v3AngularImpulse = v3Cross * internal_rigidbody_component::s_fMaxAngularForce;
      m_pRigidbody->AddTorque(v3AngularImpulse);
    }

    // Corrige la penetración si es necesario
    CEntity* pOwner = GetOwner();
    if (pOwner)
    {
      maths::CVector3 v3Offset = _oHitEvent.Normal * _oHitEvent.Depth;
      if (!v3Offset.Equal(maths::CVector3::Zero))
      {
        maths::CVector3 vNewPosition = pOwner->GetPosition() + v3Offset;
        pOwner->SetPosition(vNewPosition);
      }
    }
    m_pRigidbody->SetCurrentState(physics::ERigidbodyState::COLLIDING);
    m_pRigidbody->ResetVelocity();
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
  void CRigidbodyComponent::OnAddedRotation(const maths::CVector3& _v3Rot)
  {
    CEntity* pOwner = GetOwner();
    if (pOwner)
    {
      maths::CVector3 vNewRot = pOwner->GetRotation() + _v3Rot;
      pOwner->SetRotation(vNewRot);
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
    m_pRigidbody->SetOnRotationChangedDelegate(physics::CRigidbody::TOnVelocityChangedDelegate(&CRigidbodyComponent::OnAddedRotation, this));
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
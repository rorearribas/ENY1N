#include "RigidbodyComponent.h"
#include "Game/ETT/Entity.h"
#include "Engine/Physics/PhysicsManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Maths/Maths.h"

namespace game
{
  namespace internal_rigidbody
  {
    const float s_fMaxAngularForce(50.0f);
    const float s_fRebound(1.75f);
  }
  // ------------------------------------
  CRigidbodyComponent::CRigidbodyComponent(CEntity* _pOwner, physics::ERigidbodyType _eRigidbodyType) : CComponent(_pOwner)
  {
    CreateRigidbody(_eRigidbodyType);
  }
  // ------------------------------------
  void CRigidbodyComponent::SetRigidbodyType(physics::ERigidbodyType _eRigidbodyType)
  {
    assert(m_pRigidbody);
    m_pRigidbody->SetRigidbodyType(_eRigidbodyType);
  }
  // ------------------------------------
  void CRigidbodyComponent::OnCollisionEnter(const collisions::SHitEvent& _oHitEvent)
  { 
    // Set new state
    m_pRigidbody->SetCurrentState(physics::ERigidbodyState::COLLIDING);

    maths::CVector3 v3CurrentVelocity = m_pRigidbody->GetVelocity();
    maths::CVector3 v3VelocityDir = maths::CVector3::Normalize(v3CurrentVelocity);

    float fVelocity = v3CurrentVelocity.Length();
    maths::CVector3 v3TorqueDir = _oHitEvent.Normal.Cross(v3VelocityDir);
    m_pRigidbody->AddTorque(v3TorqueDir * -fVelocity * (internal_rigidbody::s_fMaxAngularForce * 2.0f));
  }
  // ------------------------------------
  void CRigidbodyComponent::OnCollisionStay(const collisions::SHitEvent& _oHitEvent)
  {
    maths::CVector3 v3CurrentVelocity = m_pRigidbody->GetVelocity();
    maths::CVector3 v3VelocityDir = maths::CVector3::Normalize(v3CurrentVelocity);

    // Get impact velocity
    float fImpactVelocity = abs(v3CurrentVelocity.Dot(_oHitEvent.Normal));
    if (fImpactVelocity > maths::s_fEpsilon5)
    {
      // Apply velocity
      v3CurrentVelocity = v3CurrentVelocity - _oHitEvent.Normal * (-fImpactVelocity * internal_rigidbody::s_fRebound);
      m_pRigidbody->SetVelocity(v3CurrentVelocity);
    }

    // Apply torque
    float fVelocity = v3CurrentVelocity.Length();
    if (fVelocity > maths::s_fEpsilon5)
    {
      maths::CVector3 v3TorqueDir = _oHitEvent.Normal.Cross(v3VelocityDir);
      m_pRigidbody->AddTorque(v3TorqueDir * -fVelocity * internal_rigidbody::s_fMaxAngularForce);
    }

    // Fixed impacted position
    CEntity* pOwner = GetOwner();
    if (pOwner)
    {
      maths::CVector3 v3Offset = _oHitEvent.Normal * _oHitEvent.Depth;
      if (!v3Offset.Equal(maths::CVector3::Zero, maths::s_fEpsilon5))
      {
        maths::CVector3 vNewPosition = pOwner->GetPosition() + v3Offset;
        pOwner->SetPosition(vNewPosition);
      }
    }

    // Set state
    m_pRigidbody->SetCurrentState(physics::ERigidbodyState::COLLIDING);
  }
  // ------------------------------------
  void CRigidbodyComponent::OnCollisionExit(const collisions::SHitEvent&)
  {
    m_pRigidbody->SetCurrentState(physics::ERigidbodyState::IN_THE_AIR);
  }
  // ------------------------------------
  void CRigidbodyComponent::OnApplyVelocity(const maths::CVector3& _v3Velocity)
  {
    CEntity* pOwner = GetOwner();
    if (pOwner)
    {
      maths::CVector3 vNewPosition = pOwner->GetPosition() + _v3Velocity;
      pOwner->SetPosition(vNewPosition);
    }
  }
  // ------------------------------------
  void CRigidbodyComponent::OnApplyRotation(const maths::CVector3& _v3Rot)
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
    m_pRigidbody->SetOnVelocityChangedDelegate(physics::CRigidbody::TOnVelocityChangedDelegate(&CRigidbodyComponent::OnApplyVelocity, this));
    m_pRigidbody->SetOnRotationChangedDelegate(physics::CRigidbody::TOnVelocityChangedDelegate(&CRigidbodyComponent::OnApplyRotation, this));
  }
  // ------------------------------------
  void CRigidbodyComponent::Clean()
  {
    if (m_pRigidbody)
    {
      physics::CPhysicsManager::GetInstance()->DestroyRigidbody(m_pRigidbody);
    }
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
    std::string sKinematic = "Kinematic" + std::string("##" + sOwnerName);
    std::string sAddForce = "Add Force" + std::string("##" + sOwnerName);

    ImGui::Text(sTitle.c_str());

    float fMass = GetMass();
    bool bKinematic = GetRigidbodyType() == physics::ERigidbodyType::KINEMATIC;

    ImGui::InputFloat(sMass.c_str(), &fMass);
    ImGui::Checkbox(sKinematic.c_str(), &bKinematic);

    if (ImGui::Button(sAddForce.c_str()))
    {
      if (m_pRigidbody)
      {
        m_pRigidbody->AddForce(maths::CVector3(500.0f, 0.0f, 0.0f));
      }
    }

    // Apply rigidbody cfg
    if (m_pRigidbody)
    {
      SetMass(fMass);
      physics::ERigidbodyType eRbType = bKinematic ? physics::ERigidbodyType::KINEMATIC : physics::ERigidbodyType::DYNAMIC;
      if (eRbType != GetRigidbodyType()) { SetRigidbodyType(eRbType); }
    }
  }
}
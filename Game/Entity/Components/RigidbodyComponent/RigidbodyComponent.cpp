#include "RigidbodyComponent.h"
#include "Game/Entity/Entity.h"
#include "Engine/Physics/PhysicsManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Math/Math.h"

namespace game
{
  namespace internal_rigidbody
  {
    const float s_fMaxAngularForce(50.0f);
    const float s_fRebound(1.75f);
  }
  // ------------------------------------
  void CRigidbodyComponent::CreateRigidbody(physics::ERigidbodyType _eRigidbodyType)
  {
    Clean();

    // Create rigidbody
    m_pRigidbody = physics::CPhysicsManager::GetInstance()->CreateRigidbody(_eRigidbodyType);
#ifdef _DEBUG
    assert(m_pRigidbody.IsValid());
#endif

    // Set notifications
    m_pRigidbody->SetOnVelocityChangedDelegate(physics::CRigidbody::TOnVelocityChangedDelegate(&CRigidbodyComponent::OnApplyVelocity, this));
    m_pRigidbody->SetOnRotationChangedDelegate(physics::CRigidbody::TOnVelocityChangedDelegate(&CRigidbodyComponent::OnApplyRotation, this));
  }
  // ------------------------------------
  void CRigidbodyComponent::SetRigidbodyType(physics::ERigidbodyType _eRigidbodyType)
  {
    m_pRigidbody->SetRigidbodyType(_eRigidbodyType);
  }
  // ------------------------------------
  void CRigidbodyComponent::OnCollisionEnter(const collision::THitEvent& _oHitEvent)
  {
    // Set new state
    m_pRigidbody->SetCurrentState(physics::ERigidbodyState::COLLIDING);

    math::CVector3 v3CurrentVelocity = m_pRigidbody->GetVelocity();
    math::CVector3 v3VelocityDir = math::CVector3::Normalize(v3CurrentVelocity);

    // Get impact velocity
    float fImpactVelocity = abs(v3CurrentVelocity.Dot(_oHitEvent.Normal));
    if (fImpactVelocity > math::s_fEpsilon5)
    {
      // Apply velocity
      v3CurrentVelocity = v3CurrentVelocity - _oHitEvent.Normal * (-fImpactVelocity * internal_rigidbody::s_fRebound);
      m_pRigidbody->SetVelocity(v3CurrentVelocity);
    }

    float fMagnitude = v3CurrentVelocity.Magnitude();
    math::CVector3 v3TorqueDir = _oHitEvent.Normal.Cross(v3VelocityDir);
    m_pRigidbody->AddTorque(v3TorqueDir * -fMagnitude * (internal_rigidbody::s_fMaxAngularForce * 2.0f));

    // Fixed impacted position
    CEntity* pOwner = GetOwner();
    if (pOwner)
    {
      math::CVector3 v3Offset = _oHitEvent.Normal * _oHitEvent.Depth;
      if (!v3Offset.Equal(math::CVector3::Zero, math::s_fEpsilon5))
      {
        math::CVector3 vNewPosition = pOwner->GetPos() + v3Offset;
        pOwner->SetPos(vNewPosition);
      }
    }
  }
  // ------------------------------------
  void CRigidbodyComponent::OnCollisionStay(const collision::THitEvent& _oHitEvent)
  {
    math::CVector3 v3CurrentVelocity = m_pRigidbody->GetVelocity();
    math::CVector3 v3VelocityDir = math::CVector3::Normalize(v3CurrentVelocity);

    // Get impact velocity
    float fImpactVelocity = abs(v3CurrentVelocity.Dot(_oHitEvent.Normal));
    if (fImpactVelocity > math::s_fEpsilon5)
    {
      // Apply velocity
      v3CurrentVelocity = v3CurrentVelocity - _oHitEvent.Normal * (-fImpactVelocity * internal_rigidbody::s_fRebound);
      m_pRigidbody->SetVelocity(v3CurrentVelocity);
    }

    // Apply torque
    float fMagnitude = v3CurrentVelocity.Magnitude();
    if (fMagnitude > math::s_fEpsilon5)
    {
      math::CVector3 v3TorqueDir = _oHitEvent.Normal.Cross(v3VelocityDir);
      m_pRigidbody->AddTorque(v3TorqueDir * -fMagnitude * internal_rigidbody::s_fMaxAngularForce);
    }

    // Fixed impacted position
    CEntity* pOwner = GetOwner();
    if (pOwner)
    {
      math::CVector3 v3Offset = _oHitEvent.Normal * _oHitEvent.Depth;
      if (!v3Offset.Equal(math::CVector3::Zero, math::s_fEpsilon5))
      {
        math::CVector3 vNewPosition = pOwner->GetPos() + v3Offset;
        pOwner->SetPos(vNewPosition);
      }
    }

    // Set state
    m_pRigidbody->SetCurrentState(physics::ERigidbodyState::COLLIDING);
  }
  // ------------------------------------
  void CRigidbodyComponent::OnCollisionExit(const collision::THitEvent&)
  {
    m_pRigidbody->SetCurrentState(physics::ERigidbodyState::IN_THE_AIR);
  }
  // ------------------------------------
  void CRigidbodyComponent::OnApplyVelocity(const math::CVector3& _v3Velocity)
  {
    CEntity* pOwner = GetOwner();
    if (pOwner)
    {
      math::CVector3 vNewPosition = pOwner->GetPos() + _v3Velocity;
      pOwner->SetPos(vNewPosition);
    }
  }
  // ------------------------------------
  void CRigidbodyComponent::OnApplyRotation(const math::CVector3& _v3Rot)
  {
    CEntity* pOwner = GetOwner();
    if (pOwner)
    {
      math::CVector3 vNewRot = pOwner->GetRot() + _v3Rot;
      pOwner->SetRot(vNewRot);
    }
  }
  // ------------------------------------
  void CRigidbodyComponent::Clean()
  {
    if (m_pRigidbody.IsValid())
    {
      physics::CPhysicsManager::GetInstance()->DestroyRigidbody(m_pRigidbody);
    }
  }
  // ------------------------------------
#ifdef _DEBUG
  void CRigidbodyComponent::DrawDebug()
  {
    ImGui::Spacing();
    std::string sOwnerName = GetOwner() ? GetOwner()->GetName() : std::string();
    if (!m_pRigidbody.IsValid())
    {
      return;
    }

    ImGui::Text("RIGIDBODY");
    float fMass = GetMass();
    ImGui::InputFloat("Mass", &fMass);
    bool bKinematic = GetRigidbodyType() == physics::ERigidbodyType::KINEMATIC;
    ImGui::Checkbox("Kinematic", &bKinematic);

    float fSetForce[3] = { m_v3DebugForce.x, m_v3DebugForce.y, m_v3DebugForce.z };
    ImGui::InputFloat3("Set Force", fSetForce);
    m_v3DebugForce = math::CVector3(fSetForce[0], fSetForce[1], fSetForce[2]);

    ImGui::SameLine();
    if (ImGui::Button("Apply Force"))
    {
      m_pRigidbody->AddForce(m_v3DebugForce);
    }

    // Apply
    physics::ERigidbodyType eRbType = bKinematic ? physics::ERigidbodyType::KINEMATIC : physics::ERigidbodyType::DYNAMIC;
    if (eRbType != GetRigidbodyType())
    {
      SetRigidbodyType(eRbType);
    }
    if (fMass != GetMass())
    {
      SetMass(fMass);
    }
  }
#endif
}
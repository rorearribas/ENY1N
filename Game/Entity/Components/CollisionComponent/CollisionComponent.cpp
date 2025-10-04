#include "CollisionComponent.h"
#include "Libs/ImGui/imgui.h"
#include "Game/Entity/Entity.h"
#include "Engine/Engine.h"
#include "Engine/Collisions/CollisionManager.h"
#include "Engine/Collisions/BoxCollider.h"
#include "Engine/Collisions/SphereCollider.h"
#include "Engine/Collisions/CapsuleCollider.h"
#include <cassert>

namespace game
{
  CCollisionComponent::CCollisionComponent(CEntity* _pOwner, collision::EColliderType _eColliderType) : CComponent(_pOwner)
  {
    CreateCollider(_eColliderType);
  }
  // ------------------------------------
  void CCollisionComponent::CreateCollider(collision::EColliderType _eColliderType)
  {
    // Flush
    Clean();

    // Get owner
    CEntity* pOwner = GetOwner();

    // Create collider usin coll manager
    collision::CCollisionManager* pCollisionManager = collision::CCollisionManager::GetInstance();
    if (pCollisionManager)
    {
      // Create collider
      m_pCollider = pCollisionManager->CreateCollider(_eColliderType, pOwner);
#ifdef _DEBUG
      assert(m_pCollider);
#endif

      // Assign notifications
      m_pCollider->SetOnCollisionEnter(collision::CCollider::TOnCollisionEvent(&CEntity::OnCollisionEnter, pOwner));
      m_pCollider->SetOnCollisionStay(collision::CCollider::TOnCollisionEvent(&CEntity::OnCollisionStay, pOwner));
      m_pCollider->SetOnCollisionExit(collision::CCollider::TOnCollisionEvent(&CEntity::OnCollisionExit, pOwner));

      // Force to update
      m_pCollider->SetPosition(pOwner->GetPosition());
      m_pCollider->SetRotation(pOwner->GetRotation());
      m_pCollider->RecalculateCollider();
    }
  }
  // ------------------------------------
  void CCollisionComponent::SetCollisionMask(collision::ECollisionMask _eCollisionMask)
  {
    m_pCollider->SetCollisionMask(_eCollisionMask);
  }
  // ------------------------------------
  collision::ECollisionMask CCollisionComponent::GetCollisionMask() const
  {
    return m_pCollider ? m_pCollider->GetCollisionMask() : collision::ECollisionMask::INVALID;
  }
  // ------------------------------------
  void CCollisionComponent::SetPosition(const math::CVector3& _v3Pos)
  {
    m_pCollider->SetPosition(_v3Pos);
    m_pCollider->RecalculateCollider();
  }
  // ------------------------------------
  const math::CVector3& CCollisionComponent::GetPosition() const
  {
    return m_pCollider->GetPosition();
  }
  // ------------------------------------
  void CCollisionComponent::SetRotation(const math::CVector3& _v3Rot)
  {
    m_pCollider->SetRotation(_v3Rot);
    m_pCollider->RecalculateCollider();
  }
  // ------------------------------------
  const math::CVector3& CCollisionComponent::GetRotation() const
  {
    return m_pCollider->GetRotation();
  }
  // ------------------------------------
  void CCollisionComponent::OnPositionChanged(const math::CVector3& _v3Pos)
  {
    SetPosition(_v3Pos);
  }
  // ------------------------------------
  void CCollisionComponent::OnRotationChanged(const math::CVector3& _v3Rot)
  {
    SetRotation(_v3Rot);
  }
  // ------------------------------------
  void CCollisionComponent::Clean()
  {
    if (m_pCollider)
    {
      collision::CCollisionManager::GetInstance()->DestroyCollider(m_pCollider);
    }
  }
  // ------------------------------------
  void CCollisionComponent::DrawDebug()
  {
    ImGui::Spacing();
    switch (m_pCollider->GetType())
    {
    case collision::EColliderType::BOX_COLLIDER:
    {
      collision::CBoxCollider* pBoxCollider = static_cast<collision::CBoxCollider*>(m_pCollider);
      bool bOBBEnabled = pBoxCollider->IsOBBEnabled();

      float v3Size[3] = { pBoxCollider->GetSize().X, pBoxCollider->GetSize().Y, pBoxCollider->GetSize().Z };
      float v3Max[3] = { pBoxCollider->GetMax().X, pBoxCollider->GetMax().Y, pBoxCollider->GetMax().Z };
      float v3Min[3] = { pBoxCollider->GetMin().X, pBoxCollider->GetMin().Y, pBoxCollider->GetMin().Z };

      ImGui::Text("BOX COLLIDER");
      ImGui::InputFloat3("Size", v3Size);
      ImGui::InputFloat3("Max", v3Max);
      ImGui::InputFloat3("Min", v3Min);
      ImGui::Checkbox("OBB Enabled", &bOBBEnabled);
      ImGui::Checkbox("Debug Mode", &m_bDebugMode);
      ImGui::Separator();

      // Apply values
      pBoxCollider->SetOBBEnabled(bOBBEnabled);
      math::CVector3 v3CurrentSize(v3Size[0], v3Size[1], v3Size[2]);
      if (!v3CurrentSize.Equal(pBoxCollider->GetSize()))
      {
        pBoxCollider->SetSize(v3CurrentSize);
      }
    }
    break;
    case collision::EColliderType::SPHERE_COLLIDER:
    {
      collision::CSphereCollider* pSphereCollider = static_cast<collision::CSphereCollider*>(m_pCollider);
      float v3Center[3] = { pSphereCollider->GetCenter().X, pSphereCollider->GetCenter().Y, pSphereCollider->GetCenter().Z };
      float fRadius = pSphereCollider->GetRadius();

      ImGui::Text("SPHERE COLLIDER");
      ImGui::InputFloat("Radius", &fRadius);

      // Read only
      ImGui::BeginDisabled();
      ImGui::InputFloat3("Center", v3Center);
      ImGui::EndDisabled();

      // Debug mode
      ImGui::Checkbox("Debug Mode", &m_bDebugMode);
      ImGui::Separator();

      // Apply values
      if (fRadius != pSphereCollider->GetRadius())
      {
        pSphereCollider->SetRadius(fRadius >= 0.0f ? fRadius : 0.0f);
      }
    }
    break;
    case collision::EColliderType::CAPSULE_COLLIDER:
    {
      collision::CCapsuleCollider* pCapsuleCollider = static_cast<collision::CCapsuleCollider*>(m_pCollider);
      float v3OrientedAxis[3] = { pCapsuleCollider->GetOrientedAxis().X, pCapsuleCollider->GetOrientedAxis().Y, pCapsuleCollider->GetOrientedAxis().Z };
      float v3LocalCenter[3] = { pCapsuleCollider->GetLocalCenter().X, pCapsuleCollider->GetLocalCenter().Y, pCapsuleCollider->GetLocalCenter().Z };
      float fHeight = pCapsuleCollider->GetHeight();
      float fRadius = pCapsuleCollider->GetRadius();

      ImGui::Text("CAPSULE COLLIDER");
      ImGui::InputFloat("Height", &fHeight);
      ImGui::InputFloat("Radius", &fRadius);
      ImGui::InputFloat3("Oriented Axis", v3OrientedAxis);
      ImGui::InputFloat3("Local Center", v3LocalCenter);

      // Debug mode
      ImGui::Checkbox("Debug Mode", &m_bDebugMode);
      ImGui::Separator();

      // Apply values
      if (fHeight != pCapsuleCollider->GetHeight())
      {
        pCapsuleCollider->SetHeight(fHeight);
      }
      if (fRadius != pCapsuleCollider->GetRadius())
      {
        pCapsuleCollider->SetRadius(fRadius >= 0.0f ? fRadius : 0.0f);
      }
      math::CVector3 v3CurrentOrientedAxis(v3OrientedAxis[0], v3OrientedAxis[1], v3OrientedAxis[2]);
      if (!v3CurrentOrientedAxis.Equal(pCapsuleCollider->GetLocalCenter()))
      {
        pCapsuleCollider->SetOrientedAxis(v3CurrentOrientedAxis);
      }
      math::CVector3 v3CurrentLocalCenter(v3LocalCenter[0], v3LocalCenter[1], v3LocalCenter[2]);
      if (!v3CurrentLocalCenter.Equal(pCapsuleCollider->GetLocalCenter()))
      {
        pCapsuleCollider->SetLocalCenter(v3CurrentLocalCenter);
      }
    }
    break;
    default:
      break;
    }

    if (m_bDebugMode)
    {
      m_pCollider->DrawDebug();
    }
  }
}



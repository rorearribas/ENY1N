#include "CollisionComponent.h"
#include "Libs/ImGui/imgui.h"
#include "Game/ETT/Entity.h"
#include "Engine/Base/Engine.h"
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
    std::string sOwnerName = GetOwner() ? GetOwner()->GetName() : std::string();
    std::string sDebugMode = std::string("Debug Mode") + std::string("##" + sOwnerName);

    switch (m_pCollider->GetType())
    {
    case collision::EColliderType::BOX_COLLIDER:
    {
      // Generate unique ids
      std::string sColliderType = "BOX COLLIDER";
      std::string sSize = std::string("Size") + std::string("##" + sOwnerName);
      std::string sMax = std::string("Max") + std::string("##" + sOwnerName);
      std::string sMin = std::string("Min") + std::string("##" + sOwnerName);
      std::string sOBB = std::string("OBB Enabled") + std::string("##" + sOwnerName);

      collision::CBoxCollider* pBoxCollider = static_cast<collision::CBoxCollider*>(m_pCollider);
      bool bOBBEnabled = pBoxCollider->IsOBBEnabled();

      float v3Size[3] = { pBoxCollider->GetSize().X, pBoxCollider->GetSize().Y, pBoxCollider->GetSize().Z };
      float v3Max[3] = { pBoxCollider->GetMax().X, pBoxCollider->GetMax().Y, pBoxCollider->GetMax().Z };
      float v3Min[3] = { pBoxCollider->GetMin().X, pBoxCollider->GetMin().Y, pBoxCollider->GetMin().Z };

      ImGui::Text(sColliderType.c_str());
      ImGui::InputFloat3(sSize.c_str(), v3Size);
      ImGui::InputFloat3(sMax.c_str(), v3Max);
      ImGui::InputFloat3(sMin.c_str(), v3Min);
      ImGui::Checkbox(sOBB.c_str(), &bOBBEnabled);
      ImGui::Checkbox(sDebugMode.c_str(), &m_bDebugMode);
      ImGui::Separator();

      // Apply values
      pBoxCollider->SetOBBEnabled(bOBBEnabled);
      math::CVector3 v3CurrentSize(v3Size[0], v3Size[1], v3Size[2]);
      if (!v3CurrentSize.Equal(pBoxCollider->GetSize()))
      {
        pBoxCollider->SetSize(v3CurrentSize);
      }
      if (m_bDebugMode)
      {
        pBoxCollider->DrawDebug();
      }
    }
    break;
    case collision::EColliderType::SPHERE_COLLIDER:
    {
      // Generate unique ids
      std::string sTitle = "SPHERE COLLIDER";
      std::string sCenter = "Center" + std::string("##" + sOwnerName);
      std::string sRadius = "Radius" + std::string("##" + sOwnerName);

      collision::CSphereCollider* pSphereCollider = static_cast<collision::CSphereCollider*>(m_pCollider);
      float v3Center[3] = { pSphereCollider->GetCenter().X, pSphereCollider->GetCenter().Y, pSphereCollider->GetCenter().Z };
      float fRadius = pSphereCollider->GetRadius();

      ImGui::Text(sTitle.c_str());
      ImGui::InputFloat(sRadius.c_str(), &fRadius);

      // Read only
      ImGui::BeginDisabled();
      ImGui::InputFloat3(sCenter.c_str(), v3Center);
      ImGui::EndDisabled();

      // Debug mode
      ImGui::Checkbox(sDebugMode.c_str(), &m_bDebugMode);
      ImGui::Separator();

      // Apply values
      if (fRadius != pSphereCollider->GetRadius())
      {
        pSphereCollider->SetRadius(fRadius >= 0.0f ? fRadius : 0.0f);
      }
      if (m_bDebugMode)
      {
        pSphereCollider->DrawDebug();
      }
    }
    break;
    case collision::EColliderType::CAPSULE_COLLIDER:
    {
      // Generate unique ids
      std::string sTitle = "CAPSULE COLLIDER";
      std::string sHeight = "Height" + std::string("##" + sOwnerName);
      std::string sRadius = "Radius" + std::string("##" + sOwnerName);
      std::string sLocalCenter = "Local Center" + std::string("##" + sOwnerName);

      collision::CCapsuleCollider* pCapsuleCollider = static_cast<collision::CCapsuleCollider*>(m_pCollider);
      float v3LocalCenter[3] = { pCapsuleCollider->GetLocalCenter().X, pCapsuleCollider->GetLocalCenter().Y, pCapsuleCollider->GetLocalCenter().Z };
      float fHeight = pCapsuleCollider->GetHeight();
      float fRadius = pCapsuleCollider->GetRadius();

      ImGui::Text(sTitle.c_str());

      ImGui::InputFloat(sHeight.c_str(), &fHeight);
      ImGui::InputFloat(sRadius.c_str(), &fRadius);
      ImGui::InputFloat3(sLocalCenter.c_str(), v3LocalCenter);

      // Debug mode
      ImGui::Checkbox(sDebugMode.c_str(), &m_bDebugMode);
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
      math::CVector3 v3CurrentLocalCenter(v3LocalCenter[0], v3LocalCenter[1], v3LocalCenter[2]);
      if (!v3CurrentLocalCenter.Equal(pCapsuleCollider->GetLocalCenter()))
      {
        pCapsuleCollider->SetLocalCenter(v3CurrentLocalCenter);
      }
      if (m_bDebugMode)
      {
        pCapsuleCollider->DrawDebug();
      }
    }
    break;
    default:
      break;
    }
  }
}



#include "CollisionComponent.h"
#include "Libs/ImGui/imgui.h"
#include "Engine/Collisions/CollisionManager.h"
#include "Engine/Collisions/BoxCollider.h"
#include "Engine/Collisions/SphereCollider.h"
#include "Game/ETT/Entity.h"
#include "Engine/Base/Engine.h"
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

    // Create primitive
    switch (_eColliderType)
    {
    case collision::EColliderType::BOX_COLLIDER:
    {
      m_pPrimitive = engine::CEngine::GetInstance()->CreatePrimitive
      (
        render::graphics::CPrimitive::EPrimitiveType::E3D_CUBE,
        render::ERenderMode::WIREFRAME
      );
      assert(m_pPrimitive);
    }
    break;
    case collision::EColliderType::SPHERE_COLLIDER:
    {
      m_pPrimitive = engine::CEngine::GetInstance()->CreatePrimitive
      (
        render::graphics::CPrimitive::EPrimitiveType::E3D_SPHERE,
        render::ERenderMode::WIREFRAME
      );
      assert(m_pPrimitive);
    }
    break;
    default:
      break;
    }

    // Update primtive
    if (m_pPrimitive)
    {
      m_pPrimitive->SetPosition(pOwner->GetPosition());
      m_pPrimitive->SetRotation(pOwner->GetRotation());
      m_pPrimitive->SetScale(pOwner->GetScale());

      m_pPrimitive->UseGlobalLighting(false);
      m_pPrimitive->SetColor(math::CVector3::Forward);
    }
  }
  // ------------------------------------
  void CCollisionComponent::SetPosition(const math::CVector3& _v3Pos)
  {
    if (m_pCollider)
    {
      m_pCollider->SetPosition(_v3Pos);
      m_pCollider->RecalculateCollider();
    }
    if (m_pPrimitive)
    {
      m_pPrimitive->SetPosition(_v3Pos);
    }
  }
  // ------------------------------------
  const math::CVector3& CCollisionComponent::GetPosition() const
  {
    return m_pCollider->GetPosition();
  }
  // ------------------------------------
  void CCollisionComponent::SetRotation(const math::CVector3& _v3Rot)
  {
    if (m_pCollider)
    {
      m_pCollider->SetRotation(_v3Rot);
      m_pCollider->RecalculateCollider();
    }
    if (m_pPrimitive)
    {
      m_pPrimitive->SetRotation(_v3Rot);
    }
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
    if (m_pPrimitive)
    {
      engine::CEngine::GetInstance()->DestroyPrimitive(m_pPrimitive);
    }
  }
  // ------------------------------------
  void CCollisionComponent::DrawDebug()
  {
    ImGui::Spacing();
    std::string sOwnerName = GetOwner() ? GetOwner()->GetName() : std::string();

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
      std::string sDebugMode = std::string("Debug Mode") + std::string("##" + sOwnerName);

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

      // Apply values
      pBoxCollider->SetOBBEnabled(bOBBEnabled);
      math::CVector3 v3CurrentSize(v3Size[0], v3Size[1], v3Size[2]);
      if (m_pPrimitive && !v3CurrentSize.Equal(m_pPrimitive->GetScale()))
      {
        m_pPrimitive->SetScale(math::CVector3(v3Size[0], v3Size[1], v3Size[2]));
      }
      if (m_pPrimitive && !v3CurrentSize.Equal(pBoxCollider->GetSize()))
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

      // Update radius
      if (fRadius != pSphereCollider->GetRadius())
      {
        pSphereCollider->SetRadius(fRadius);
        if (m_pPrimitive)
        {
          m_pPrimitive->SetScale(math::CVector3(fRadius * 2.0f, fRadius * 2.0f, fRadius * 2.0f));
        }
      }
    }
    break;
    default:
      break;
    }
  }
}



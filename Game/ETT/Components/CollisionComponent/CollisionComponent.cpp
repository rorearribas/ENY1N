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
  CCollisionComponent::CCollisionComponent(CEntity* _pOwner, collisions::EColliderType _eColliderType) : CComponent(_pOwner)
  {
    CreateCollider(_eColliderType);
  }
  // ------------------------------------
  void CCollisionComponent::CreateCollider(collisions::EColliderType _eColliderType)
  {
    // Flush
    Clean();

    // Get owner
    CEntity* pOwner = GetOwner();
    assert(pOwner);

    // Create collider usin coll manager
    collisions::CCollisionManager* pCollisionManager = collisions::CCollisionManager::GetInstance();
    if (pCollisionManager)
    { 
      // Create collider
      m_pCollider = pCollisionManager->CreateCollider(_eColliderType, pOwner);
      assert(m_pCollider);

      // Assign notifications
      m_pCollider->SetOnCollisionEnter(collisions::CCollider::TOnCollisionEvent(&CEntity::OnCollisionEnter, pOwner));
      m_pCollider->SetOnCollisionStay(collisions::CCollider::TOnCollisionEvent(&CEntity::OnCollisionStay, pOwner));
      m_pCollider->SetOnCollisionExit(collisions::CCollider::TOnCollisionEvent(&CEntity::OnCollisionExit, pOwner));
    }

    // Create primitive
    switch (_eColliderType)
    {
    case collisions::BOX_COLLIDER:
    {
      m_pPrimitive = engine::CEngine::GetInstance()->CreatePrimitive
      (
        render::graphics::CPrimitive::EPrimitiveType::E3D_CUBE, 
        render::ERenderMode::WIREFRAME
      );
      assert(m_pPrimitive);
    }
    break;
    case collisions::SPHERE_COLLIDER:
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

    if (m_pPrimitive)
    {
      m_pPrimitive->SetColor(maths::CVector3::One);
      m_pPrimitive->SetScale(pOwner->GetScale());
    }
  }
  // ------------------------------------
  void CCollisionComponent::SetPosition(const maths::CVector3& _v3Pos)
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
  const maths::CVector3& CCollisionComponent::GetPosition() const
  {
    return m_pCollider->GetPosition();
  }
  // ------------------------------------
  void CCollisionComponent::SetRotation(const maths::CVector3& _v3Rot)
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
  const maths::CVector3& CCollisionComponent::GetRotation() const
  {
    return m_pCollider->GetRotation();
  }
  // ------------------------------------
  void CCollisionComponent::OnPositionChanged(const maths::CVector3& _v3Pos)
  {
    SetPosition(_v3Pos);
  }
  // ------------------------------------
  void CCollisionComponent::OnRotationChanged(const maths::CVector3& _v3Rot)
  {
    SetRotation(_v3Rot);
  }
  // ------------------------------------
  void CCollisionComponent::Clean()
  {
    if (m_pCollider)
    {
      collisions::CCollisionManager::GetInstance()->DestroyCollider(m_pCollider);
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
    case collisions::EColliderType::BOX_COLLIDER:
    {
      // Generate unique ids
      std::string sTitle = "BOX COLLIDER";
      std::string sSize = "Size" + std::string("##" + sOwnerName);
      std::string sMax = "Max" + std::string("##" + sOwnerName);
      std::string sMin = "Min" + std::string("##" + sOwnerName);

      collisions::CBoxCollider* pBoxCollider = static_cast<collisions::CBoxCollider*>(m_pCollider);
      float v3Size[3] = { pBoxCollider->GetSize().X, pBoxCollider->GetSize().Y, pBoxCollider->GetSize().Z };
      float v3Max[3] = { pBoxCollider->GetMax().X, pBoxCollider->GetMax().Y, pBoxCollider->GetMax().Z };
      float v3Min[3] = { pBoxCollider->GetMin().X, pBoxCollider->GetMin().Y, pBoxCollider->GetMin().Z };

      ImGui::Text(sTitle.c_str());
      ImGui::InputFloat3(sSize.c_str(), v3Size);
      ImGui::InputFloat3(sMax.c_str(), v3Max);
      ImGui::InputFloat3(sMin.c_str(), v3Min);

      // Apply box collider size
      if (m_pPrimitive) 
      { 
        m_pPrimitive->SetScale(maths::CVector3(v3Size[0], v3Size[1], v3Size[2])); 
      }
      pBoxCollider->SetSize(maths::CVector3(v3Size[0], v3Size[1], v3Size[2]));
    }
    break;
    case collisions::EColliderType::SPHERE_COLLIDER:
    {
      // Generate unique ids
      std::string sTitle = "SPHERE COLLIDER";
      std::string sCenter = "Center" + std::string("##" + sOwnerName);
      std::string sRadius = "Radius" + std::string("##" + sOwnerName);

      collisions::CSphereCollider* pSphereCollider = static_cast<collisions::CSphereCollider*>(m_pCollider);
      float v3Center[3] = { pSphereCollider->GetCenter().X, pSphereCollider->GetCenter().Y, pSphereCollider->GetCenter().Z };
      float fRadius = pSphereCollider->GetRadius();

      ImGui::Text(sTitle.c_str());
      ImGui::InputFloat3(sCenter.c_str(), v3Center);
      ImGui::InputFloat(sRadius.c_str(), &fRadius);

      pSphereCollider->SetCenter(maths::CVector3(GetPosition().X, GetPosition().Y, GetPosition().Z));
      pSphereCollider->SetRadius(fRadius);
      if (m_pPrimitive) { m_pPrimitive->SetScale(maths::CVector3(fRadius * 2.0f, fRadius * 2.0f, fRadius * 2.0f)); }
    }
    break;
    default:
      break;
    }
  }
}



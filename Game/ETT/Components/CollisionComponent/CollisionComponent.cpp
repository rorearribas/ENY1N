#include "CollisionComponent.h"
#include "Libs/ImGui/imgui.h"
#include "Engine/Physics/BoxCollider.h"
#include "Engine/Physics/SphereCollider.h"
#include "Engine/Managers/CollisionManager.h"
#include "Game/ETT/Entity.h"
#include <cassert>
#include "Engine/Base/Engine.h"

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
    physics::CCollisionManager* pCollisionManager = physics::CCollisionManager::GetInstance();
    if (pCollisionManager)
    {
      m_pCollider = pCollisionManager->CreateCollider(_eColliderType);
    }
  }
  // ------------------------------------
  void CCollisionComponent::Update(float _fDeltaTime)
  {
    // Colliders are being updated in the collision manager!
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
    if (m_pPrimitive)
    {
      m_pPrimitive->SetPosition(_v3Pos);
    }
  }
  // ------------------------------------
  void CCollisionComponent::Clean()
  {
    if (m_pCollider)
    {
      physics::CCollisionManager::GetInstance()->DestroyCollider(m_pCollider);
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
    case physics::EColliderType::BOX_COLLIDER:
    {
      if (!m_pPrimitive)
      {
        m_pPrimitive = engine::CEngine::GetInstance()->CreatePrimitive
        (
          render::graphics::CPrimitive::EPrimitiveType::E3D_CUBE,
          render::graphics::CPrimitive::ERenderMode::WIREFRAME
        );
        m_pPrimitive->SetColor(maths::CVector3::One);
      }
      
      // Generate unique ids
      std::string sTitle = "BOX COLLIDER";
      std::string sMax = "Max" + std::string("##" + sOwnerName);
      std::string sMin = "Min" + std::string("##" + sOwnerName);

      physics::CBoxCollider* pBoxCollider = static_cast<physics::CBoxCollider*>(m_pCollider);
      float v3Max[3] = { pBoxCollider->GetMax().X, pBoxCollider->GetMax().Y, pBoxCollider->GetMax().Z };
      float v3Min[3] = { pBoxCollider->GetMin().X, pBoxCollider->GetMin().Y, pBoxCollider->GetMin().Z };

      ImGui::Text(sTitle.c_str());
      ImGui::InputFloat3(sMax.c_str(), v3Max);
      ImGui::InputFloat3(sMin.c_str(), v3Min);

      pBoxCollider->SetMax(maths::CVector3(v3Max[0], v3Max[1], v3Max[2]));
      pBoxCollider->SetMin(maths::CVector3(v3Min[0], v3Min[1], v3Min[2]));
      m_pPrimitive->SetScale(maths::CVector3(v3Max[0], v3Max[1], v3Max[2]));
    }
    break;
    case physics::EColliderType::SPHERE_COLLIDER:
    {
      // Generate unique ids
      std::string sTitle = "SPHERE COLLIDER";
      std::string sCenter = "Center" + std::string("##" + sOwnerName);
      std::string sRadius = "Radius" + std::string("##" + sOwnerName);

      physics::CSphereCollider* pSphereCollider = static_cast<physics::CSphereCollider*>(m_pCollider);
      float v3Center[3] = { pSphereCollider->GetCenter().X, pSphereCollider->GetCenter().Y, pSphereCollider->GetCenter().Z };
      float fRadius = pSphereCollider->GetRadius();

      ImGui::Text(sTitle.c_str());
      ImGui::InputFloat3(sCenter.c_str(), v3Center);
      ImGui::InputFloat(sRadius.c_str(), &fRadius);

      pSphereCollider->SetPosition(maths::CVector3(v3Center[0], v3Center[1], v3Center[2]));
      pSphereCollider->SetRadius(fRadius);
    }
    break;
    default:
      break;
    }
  }
}



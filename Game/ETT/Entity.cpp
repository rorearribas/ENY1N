#include "Entity.h"
#include "Components/Component.h"
#include <algorithm>
#include "Libs/ImGui/imgui.h"
#include <string>
#include <typeinfo>

namespace game
{
  // ------------------------------------
  CEntity::~CEntity()
  {
    DestroyAllComponents();
  }
  // ------------------------------------
  void CEntity::Update(float _fDeltaTime)
  {
    if (!m_bTickEnabled) return;

    // Update components
    for (CComponent* pComponent : m_vctComponents)
    {
      if (pComponent)
      {
        pComponent->Update(_fDeltaTime);
      }
    }
  }
  // ------------------------------------
  void CEntity::DrawDebug()
  {
    ImGui::Begin("Entity");

    ImGui::Text("ID: ");
    ImGui::SameLine();
    ImGui::Text(m_sEntityName.c_str());

    ImGui::Text("Pos: ");
    ImGui::SameLine();
    ImGui::SameLine();
    float vPos[3] = { m_oTransform.GetPosition().X, m_oTransform.GetPosition().Y, m_oTransform.GetPosition().Z };
    std::string sUniquePosValue = std::string("##Pos") + m_sEntityName + std::string("_unique");
    ImGui::InputFloat3(sUniquePosValue.c_str(), vPos);

    ImGui::Text("Rot: ");
    ImGui::SameLine();
    ImGui::SameLine();
    float vRot[3] = { m_oTransform.GetRotation().X, m_oTransform.GetRotation().Y, m_oTransform.GetRotation().Z };
    std::string sUniqueRotValue = std::string("##Rot") + m_sEntityName + std::string("_unique");
    ImGui::InputFloat3(sUniqueRotValue.c_str(), vRot);

    ImGui::Text("Scl: ");
    ImGui::SameLine();
    ImGui::SameLine();
    float vScale[3] = { m_oTransform.GetScale().X, m_oTransform.GetScale().Y, m_oTransform.GetScale().Z };
    std::string sUniqueSclValue = std::string("##Scl") + m_sEntityName + std::string("_unique");
    ImGui::InputFloat3(sUniqueSclValue.c_str(), vScale);

    ImGui::End();

    // Draw debug
    for (CComponent* pComponent : m_vctComponents)
    {
      if (pComponent)
      {
        pComponent->DrawDebug();
      }
    }
  }
  // ------------------------------------
  void CEntity::SetPosition(const maths::CVector3& _v3Position)
  {
    m_oTransform.SetPosition(_v3Position);

    // Notify to components
    for (game::CComponent* pComponent : m_vctComponents)
    {
      if (pComponent)
      {
        pComponent->OnPositionChanged(_v3Position);
      }
    }
  }
  // ------------------------------------
  void CEntity::SetRotation(const maths::CVector3& _v3Rot)
  {
    m_oTransform.SetRotation(_v3Rot);

    // Notify to components
    for (game::CComponent* pComponent : m_vctComponents)
    {
      if (pComponent)
      {
        pComponent->OnRotationChanged(_v3Rot);
      }
    }
  }
  // ------------------------------------
  void CEntity::SetScale(const maths::CVector3& _v3Scale)
  {
    m_oTransform.SetScale(_v3Scale);

    // Notify to components
    for (game::CComponent* pComponent : m_vctComponents)
    {
      if (pComponent)
      {
        pComponent->OnScaleChanged(_v3Scale);
      }
    }
  }
  // ------------------------------------
  void CEntity::DestroyAllComponents()
  {
    std::for_each(m_vctComponents.begin(), m_vctComponents.end(), [](CComponent*& _pComponent)
    {
      if (_pComponent)
      {
        delete _pComponent;
        _pComponent = nullptr;
      }
    });
  }
}
#include "Entity.h"
#include "Components/Component.h"
#include <algorithm>
#include "Libs/ImGui/imgui.h"
#include "Libs/ImGui/ImGuizmo.h"
#include <string>
#include <typeinfo>
#include "Engine/Engine.h"
#include <iostream>
#include "Libs/Math/Math.h"

namespace game
{
  // ------------------------------------
  CEntity::~CEntity()
  {
    Clear();
  }
  // ------------------------------------
  void CEntity::Update(float _fDeltaTime)
  {
    if (m_bTickEnabled)
    {
      // Update components
      for (CComponent* pComponent : m_lstComponents)
      {
        if (pComponent)
        {
          pComponent->Update(_fDeltaTime);
        }
      }
    }
  }
  // ------------------------------------
  void CEntity::DrawDebug()
  {
    // Begin
    ImGui::Begin(m_sEntityName.c_str());

    // Gizmo config
    static ImGuizmo::OPERATION s_eGizmoOperation = ImGuizmo::TRANSLATE;
    static ImGuizmo::MODE s_eGizmoMode = ImGuizmo::WORLD;

    if (ImGui::IsKeyPressed(ImGuiKey_W)) s_eGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_E)) s_eGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R)) s_eGizmoOperation = ImGuizmo::SCALE;

    // Operation
    if (ImGui::RadioButton("Translate", s_eGizmoOperation == ImGuizmo::TRANSLATE))
      s_eGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", s_eGizmoOperation == ImGuizmo::ROTATE))
      s_eGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", s_eGizmoOperation == ImGuizmo::SCALE))
      s_eGizmoOperation = ImGuizmo::SCALE;

    // Mode
    if (ImGui::RadioButton("World", s_eGizmoMode == ImGuizmo::WORLD))
      s_eGizmoMode = ImGuizmo::WORLD;
    ImGui::SameLine();
    if (ImGui::RadioButton("Local", s_eGizmoMode == ImGuizmo::LOCAL))
      s_eGizmoMode = ImGuizmo::LOCAL;

    ImGui::Separator();
    ImGui::Spacing();

    // Get view matrix and projection matrix
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    const render::CCamera* pCamera = pEngine->GetCamera();

    // Get matrix
    float fTranslation[3] = { m_oTransform.GetPosition().x, m_oTransform.GetPosition().y, m_oTransform.GetPosition().z };
    float fRotation[3] = { m_oTransform.GetRotation().x, m_oTransform.GetRotation().y, m_oTransform.GetRotation().z };
    float fScale[3] = { m_oTransform.GetScale().x, m_oTransform.GetScale().y, m_oTransform.GetScale().z };

    // Set rect
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    // get view matrix
    const math::CMatrix4x4& viewMatrix = pCamera->GetViewMatrix();
    const math::CMatrix4x4& projectionMatrix = pCamera->GetProjectionMatrix();

    // Manipulate gizmo
    math::CMatrix4x4 mMatrix = m_oTransform.GetMatrix();
    if (ImGuizmo::Manipulate(viewMatrix, projectionMatrix, s_eGizmoOperation, s_eGizmoMode, mMatrix))
    {
      // Apply modifications
      switch (s_eGizmoOperation)
      {
        case ImGuizmo::TRANSLATE:
        {
          SetPosition(mMatrix.GetTranslate());
        }
        break;
        case ImGuizmo::ROTATE:
        {
          SetRotation(mMatrix.GetRotation());
        }
        break;
        case ImGuizmo::SCALE:
        {
          SetScale(mMatrix.GetScale());
        }
        break;
      }
    }

    ImGui::InputFloat3("Translation", fTranslation);
    ImGui::InputFloat3("Rotation", fRotation);
    ImGui::InputFloat3("Scaling", fScale);

    math::CVector3 v3Pos(fTranslation[0], fTranslation[1], fTranslation[2]);
    math::CVector3 v3Rot(fRotation[0], fRotation[1], fRotation[2]);
    math::CVector3 v3Scale(fScale[0], fScale[1], fScale[2]);

    if (!ImGuizmo::IsUsing())
    {
      // Update the entity
      if (!GetPosition().Equal(v3Pos))
      {
        SetPosition(v3Pos);
      }
      if (!GetRotation().Equal(v3Rot))
      {
        SetRotation(v3Rot);
      }
      if (!GetScale().Equal(v3Scale))
      {
        SetScale(v3Scale);
      }
    }

    // Draw debug on components
    for (CComponent* pComponent : m_lstComponents)
    {
      if (pComponent)
      {
        ImGui::Spacing();
        pComponent->DrawDebug();
      }
    }

    ImGui::End();
  }
  // ------------------------------------
  void CEntity::SetPosition(const math::CVector3& _v3Position)
  {
    m_oTransform.SetPosition(_v3Position);

    // Notify to components
    for (game::CComponent* pComponent : m_lstComponents)
    {
      if (pComponent)
      {
        pComponent->OnPositionChanged(_v3Position);
      }
    }
  }
  // ------------------------------------
  void CEntity::SetRotation(const math::CVector3& _v3Rot)
  {
    m_oTransform.SetRotation(_v3Rot);

    // Notify to components
    for (game::CComponent* pComponent : m_lstComponents)
    {
      if (pComponent)
      {
        pComponent->OnRotationChanged(_v3Rot);
      }
    }
  }
  // ------------------------------------
  void CEntity::SetScale(const math::CVector3& _v3Scale)
  {
    m_oTransform.SetScale(_v3Scale);

    // Notify to components
    for (game::CComponent* pComponent : m_lstComponents)
    {
      if (pComponent)
      {
        pComponent->OnScaleChanged(_v3Scale);
      }
    }
  }
  // ------------------------------------
  void CEntity::OnCollisionEnter(const collision::THitEvent& _oHitEvent)
  {
    // Notify to components
    for (game::CComponent* pComponent : m_lstComponents)
    {
      if (pComponent)
      {
        pComponent->OnCollisionEnter(_oHitEvent);
      }
    }
  }
  // ------------------------------------
  void CEntity::OnCollisionStay(const collision::THitEvent& _oHitEvent)
  {
    // Notify to components
    for (game::CComponent* pComponent : m_lstComponents)
    {
      if (pComponent)
      {
        pComponent->OnCollisionStay(_oHitEvent);
      }
    }
  }
  // ------------------------------------
  void CEntity::OnCollisionExit(const collision::THitEvent& _oHitEvent)
  {
    // Notify to components
    for (game::CComponent* pComponent : m_lstComponents)
    {
      if (pComponent)
      {
        pComponent->OnCollisionExit(_oHitEvent);
      }
    }
  }
  // ------------------------------------
  void CEntity::Clear()
  {
    std::for_each(m_lstComponents.begin(), m_lstComponents.end(), [](CComponent*& _pComponent)
    {
      global::ReleaseObject(_pComponent);
    });
  }
}
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
    DestroyAllComponents();
  }
  // ------------------------------------
  void CEntity::Update(float _fDeltaTime)
  {
    if (m_bTickEnabled)
    {
      // Update components
      for (CComponent* pComponent : m_vctComponents)
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
    static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::ROTATE;
    static ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::MODE::WORLD;

    if (ImGui::IsKeyPressed(ImGuiKey_W)) mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_E)) mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R)) mCurrentGizmoOperation = ImGuizmo::SCALE;

    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
      mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
      mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
      mCurrentGizmoOperation = ImGuizmo::SCALE;

    // Get view matrix and projection matrix
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    const render::CCamera* pCamera = pEngine->GetCamera();

    float matrix[16];
    math::CMatrix4x4 mTransform = m_oTransform.ComputeModelMatrix();
    memcpy(matrix, mTransform(), sizeof(matrix));

    // Get matrix
    float fTranslation[3] = { m_oTransform.GetPosition().X, m_oTransform.GetPosition().Y, m_oTransform.GetPosition().Z };
    float fRotation[3] = { m_oTransform.GetRotation().X, m_oTransform.GetRotation().Y, m_oTransform.GetRotation().Z };
    float fScale[3] = { m_oTransform.GetScale().X, m_oTransform.GetScale().Y, m_oTransform.GetScale().Z };

    // Set rect
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    // get view matrix
    const math::CMatrix4x4& viewMatrix = pCamera->GetViewMatrix();
    const math::CMatrix4x4& projectionMatrix = pCamera->GetProjectionMatrix();

    // Recompose matrix
    ImGuizmo::RecomposeMatrixFromComponents(fTranslation, fRotation, fScale, matrix);

    // Manipulate gizmo
    if (ImGuizmo::Manipulate(viewMatrix(), projectionMatrix(), mCurrentGizmoOperation, mCurrentGizmoMode, matrix))
    {
      // Decompose matrix
      ImGuizmo::DecomposeMatrixToComponents(matrix, fTranslation, fRotation, fScale);

      // Apply modifications
      switch (mCurrentGizmoOperation)
      {
      case ImGuizmo::TRANSLATE:
      {
        SetPosition(math::CVector3(fTranslation[0], fTranslation[1], fTranslation[2]));
      }
      break;
      case ImGuizmo::ROTATE:
      {
        math::CVector3 vNewRot = m_oTransform.GetRotation();
        vNewRot.X += (fRotation[0] - vNewRot.X);
        vNewRot.Y += (fRotation[1] - vNewRot.Y);
        vNewRot.Z += (fRotation[2] - vNewRot.Z);
        SetRotation(vNewRot);
      }
      break;
      case ImGuizmo::SCALE:
      {
        SetScale(math::CVector3(fScale[0], fScale[1], fScale[2]));
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
    for (CComponent* pComponent : m_vctComponents)
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
    for (game::CComponent* pComponent : m_vctComponents)
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
    for (game::CComponent* pComponent : m_vctComponents)
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
    for (game::CComponent* pComponent : m_vctComponents)
    {
      if (pComponent)
      {
        pComponent->OnScaleChanged(_v3Scale);
      }
    }
  }
  // ------------------------------------
  void CEntity::OnCollisionEnter(const collision::SHitEvent& _oHitEvent)
  {
    // Notify to components
    for (game::CComponent* pComponent : m_vctComponents)
    {
      if (pComponent)
      {
        pComponent->OnCollisionEnter(_oHitEvent);
      }
    }
  }
  // ------------------------------------
  void CEntity::OnCollisionStay(const collision::SHitEvent& _oHitEvent)
  {
    // Notify to components
    for (game::CComponent* pComponent : m_vctComponents)
    {
      if (pComponent)
      {
        pComponent->OnCollisionStay(_oHitEvent);
      }
    }
  }
  // ------------------------------------
  void CEntity::OnCollisionExit(const collision::SHitEvent& _oHitEvent)
  {
    // Notify to components
    for (game::CComponent* pComponent : m_vctComponents)
    {
      if (pComponent)
      {
        pComponent->OnCollisionExit(_oHitEvent);
      }
    }
  }
  // ------------------------------------
  void CEntity::DestroyAllComponents()
  {
    std::for_each(m_vctComponents.begin(), m_vctComponents.end(), [](CComponent*& _pComponent)
      {
        global::ReleaseObject(_pComponent);
      });
  }
}
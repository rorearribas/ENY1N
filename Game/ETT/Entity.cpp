#include "Entity.h"
#include "Components/Component.h"
#include <algorithm>
#include "Libs/ImGui/imgui.h"
#include "Libs/ImGui/ImGuizmo.h"
#include <string>
#include <typeinfo>
#include "Engine/Base/Engine.h"
#include <iostream>
#include "Libs/Maths/Maths.h"

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
    std::string sDebugEntity = m_sEntityName + "##Entity";
    ImGui::Begin(sDebugEntity.c_str());

    // Gizmo config
    static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::ROTATE;
    static ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::WORLD;

    if (ImGui::IsKeyPressed(ImGuiKey_W)) mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_E)) mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R)) mCurrentGizmoOperation = ImGuizmo::SCALE;

    // Generate unique ids
    std::string sTranslateId = "Translate##" + m_sEntityName;
    std::string sRotateId = "Rotate##" + m_sEntityName;
    std::string sScaleId = "Scale##" + m_sEntityName;

    if (ImGui::RadioButton(sTranslateId.c_str(), mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
      mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton(sRotateId.c_str(), mCurrentGizmoOperation == ImGuizmo::ROTATE))
      mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton(sScaleId.c_str(), mCurrentGizmoOperation == ImGuizmo::SCALE))
      mCurrentGizmoOperation = ImGuizmo::SCALE;

    // Get view matrix and projection matrix
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    const render::CCamera* pCamera = pEngine->GetCamera();

    // Transpose matrix
    float matrix[16];
    maths::CMatrix4x4 mTranspose = maths::CMatrix4x4::Transpose(m_oTransform.ComputeModelMatrix());
    memcpy(matrix, mTranspose(), sizeof(matrix));

    // Get matrix
    float fTranslation[3] = { m_oTransform.GetPosition().X, m_oTransform.GetPosition().Y, m_oTransform.GetPosition().Z };
    float fRotation[3] = { m_oTransform.GetRotation().X, m_oTransform.GetRotation().Y, m_oTransform.GetRotation().Z };
    float fScale[3] = { m_oTransform.GetScale().X, m_oTransform.GetScale().Y, m_oTransform.GetScale().Z };

    // Set rect
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    // get view matrix
    const maths::CMatrix4x4& viewMatrix = pCamera->GetViewMatrix();
    const maths::CMatrix4x4& projectionMatrix = pCamera->GetProjectionMatrix();

    // Recompose matrix
    ImGuizmo::RecomposeMatrixFromComponents(fTranslation, fRotation, fScale, matrix);

    // Manipulate gizmo
    ImGuizmo::Manipulate(viewMatrix(), projectionMatrix(), mCurrentGizmoOperation, mCurrentGizmoMode, matrix);

    if (ImGuizmo::IsUsing())
    {
      // Decompose matrix
      ImGuizmo::DecomposeMatrixToComponents(matrix, fTranslation, fRotation, fScale);

      // Apply modifications
      switch (mCurrentGizmoOperation) 
      {
      case ImGuizmo::TRANSLATE:
      {
        SetPosition(maths::CVector3(fTranslation[0], fTranslation[1], fTranslation[2]));
      }
      break;
      case ImGuizmo::ROTATE:
      {
        maths::CVector3 vNewRot = m_oTransform.GetRotation();
        vNewRot.X += (fRotation[0] - vNewRot.X) * -1.0f;
        vNewRot.Y += (fRotation[1] - vNewRot.Y) * -1.0f;
        vNewRot.Z += (fRotation[2] - vNewRot.Z) * -1.0f;
        SetRotation(vNewRot);
      }
      break;
      case ImGuizmo::SCALE:
      {
        SetScale(maths::CVector3(fScale[0], fScale[1], fScale[2]));
      }
      break;
      }
    }

    // Generate unique ids
    std::string sTranslateInputId = "Translation##" + m_sEntityName;
    std::string sRotateInputId = "Rotation##" + m_sEntityName;
    std::string sScaleInputId = "Scaling##" + m_sEntityName;

    ImGui::InputFloat3(sTranslateInputId.c_str(), fTranslation);
    ImGui::InputFloat3(sRotateInputId.c_str(), fRotation);
    ImGui::InputFloat3(sScaleInputId.c_str(), fScale);

    if (!ImGuizmo::IsUsing())
    {
      // Update the entity
      SetPosition(maths::CVector3(fTranslation[0], fTranslation[1], fTranslation[2]));
      SetRotation(maths::CVector3(fRotation[0], fRotation[1], fRotation[2]));
      SetScale(maths::CVector3(fScale[0], fScale[1], fScale[2]));
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
  void CEntity::OnCollisionEnter(const collisions::SHitEvent& _oHitEvent)
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
  void CEntity::OnCollisionStay(const collisions::SHitEvent& _oHitEvent)
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
  void CEntity::OnCollisionExit(const collisions::SHitEvent& _oHitEvent)
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
      if (_pComponent)
      {
        delete _pComponent;
        _pComponent = nullptr;
      }
    });
  }
}
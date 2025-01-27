#include "Entity.h"
#include "Components/Component.h"
#include <algorithm>
#include "Libs/ImGui/imgui.h"
#include "Libs/ImGui/ImGuizmo.h"
#include <string>
#include <typeinfo>
#include "Engine/Base/Engine.h"

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

    const maths::CMatrix4x4& viewMatrix = pCamera->GetViewMatrix();
    const float* pViewMatrix = reinterpret_cast<const float*>(&viewMatrix.GetValue());
    const maths::CMatrix4x4& projectionMatrix = pCamera->GetProjectionMatrix();
    const float* pProjectionMatrix = reinterpret_cast<const float*>(&projectionMatrix.GetValue());

    // Get current model matrix
    maths::CMatrix4x4 mModelMatrix = m_oTransform.ComputeModelMatrix();
    const float* pModelMatrix = reinterpret_cast<const float*>(&mModelMatrix.GetValue());

    // Set rect
    /*ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);*/

    ImGuizmo::Manipulate(pViewMatrix, pProjectionMatrix, mCurrentGizmoOperation, mCurrentGizmoMode, const_cast<float*>(pModelMatrix));

    // Descomponer la matriz de modelo transformada para obtener los componentes
    float matrixTranslation[3] = { m_oTransform.GetPosition().X, m_oTransform.GetPosition().Y, m_oTransform.GetPosition().Z };
    float matrixRotation[3] = { m_oTransform.GetRotation().X, m_oTransform.GetRotation().Y, m_oTransform.GetRotation().Z };
    float matrixScale[3] = { m_oTransform.GetScale().X, m_oTransform.GetScale().Y, m_oTransform.GetScale().Z };

    // Si el gizmo está siendo usado, actualizamos la transformación del objeto
    if (ImGuizmo::IsUsing())
    {
      // Descomponer la matriz de modelo resultante en sus componentes
      ImGuizmo::DecomposeMatrixToComponents(pModelMatrix, matrixTranslation, matrixRotation, matrixScale);

      // Actualizar la entidad con los nuevos valores modificados
      SetPosition(maths::CVector3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));
      SetRotation(maths::CVector3(matrixRotation[0], matrixRotation[1], matrixRotation[2]));
      SetScale(maths::CVector3(matrixScale[0], matrixScale[1], matrixScale[2]));
    }

    // Generate unique ids
    std::string sTranslateInputId = "Translation##" + m_sEntityName;
    std::string sRotateInputId = "Rotation##" + m_sEntityName;
    std::string sScaleInputId = "Scaling##" + m_sEntityName;

    ImGui::InputFloat3(sTranslateInputId.c_str(), matrixTranslation);
    ImGui::InputFloat3(sRotateInputId.c_str(), matrixRotation);
    ImGui::InputFloat3(sScaleInputId.c_str(), matrixScale);

    // Update pos rot and scale
    SetPosition(maths::CVector3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));
    SetRotation(maths::CVector3(matrixRotation[0], matrixRotation[1], matrixRotation[2]));
    SetScale(maths::CVector3(matrixScale[0], matrixScale[1], matrixScale[2]));

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
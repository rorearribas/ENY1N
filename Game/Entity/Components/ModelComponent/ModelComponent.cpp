#include "ModelComponent.h"

#include "Engine/Engine.h"
#include "Game/Entity/Entity.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"

#include "Libs/ImGui/imgui.h"
#include <cassert>

namespace game
{
  // ------------------------------------
  CModelComponent::~CModelComponent()
  {
    Clean();
  }
  // ------------------------------------
  void CModelComponent::Clean()
  {
    // Flush primitive
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    if (m_wpPrimitive.IsValid())
    {
      pEngine->DestroyPrimitive(m_wpPrimitive);
    }

    // Handle model and instances
    if (m_wpModel.IsValid() && m_wpModelInstance.IsValid())
    {
      m_wpModel->RemoveInstance(m_wpModelInstance);
    }
    else if (m_wpModel.IsValid())
    {
      m_wpModel->SetVisible(false);
    }

    // Remove model
    if (m_wpModel.IsValid() && m_wpModel->GetInstances().IsEmpty())
    {
      pEngine->DestroyModel(m_wpModel);
    }
  }
  // ------------------------------------
  void CModelComponent::LoadModel(const char* _sModelPath)
  {
    // Flush
    Clean();

    // Create model
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    m_wpModel = pEngine->LoadModel(_sModelPath);
#ifdef _DEBUG
    assert(m_wpModel.IsValid());
#endif
    if (m_wpModel->HasInstances())
    {
      // Set instance ID
      const render::gfx::TInstances& rInstances = m_wpModel->GetInstances();
      m_wpModelInstance = rInstances[rInstances.GetSize() - 1];

      // Update transform
      m_wpModelInstance->SetPos(GetPosition());
      m_wpModelInstance->SetRot(GetRotation());
      m_wpModelInstance->SetScl(GetScale());
    }
    else
    {
      // Update transform
      m_wpModel->SetPos(GetPosition());
      m_wpModel->SetRot(GetRotation());
      m_wpModel->SetScl(GetScale());
    }
  }
  // ------------------------------------
  void CModelComponent::CreatePrimitive(render::EPrimitive _eType, render::ERenderMode _eRenderMode)
  {
    // Flush
    Clean();

    // Create primitive
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    m_wpPrimitive = pEngine->CreatePrimitive(_eType, _eRenderMode);
#ifdef _DEBUG
    assert(m_wpPrimitive.IsValid());
#endif

    // Update transform
    CEntity* pOwner = GetOwner();
    if (pOwner)
    {
      m_wpPrimitive->SetPos(pOwner->GetPos());
      m_wpPrimitive->SetRot(pOwner->GetRot());
    }
  }
  // ------------------------------------
  void CModelComponent::SetCullingEnabled(bool _bCull)
  {
    if (m_wpPrimitive.IsValid())
    {
      m_wpPrimitive->SetCullEnabled(_bCull);
    }

    if (m_wpModel.IsValid() && !m_wpModelInstance.IsValid())
    {
      m_wpModel->SetCullEnabled(_bCull);
    }
    else if (m_wpModelInstance.IsValid())
    {
      m_wpModelInstance->SetCullEnabled(_bCull);
    }
  }
  // ------------------------------------
  void CModelComponent::SetColor(const math::CVector3& _v3Color)
  {
    if (m_wpPrimitive.IsValid())
    {
      m_wpPrimitive->SetColor(_v3Color);
    }
  }
  // ------------------------------------
  void CModelComponent::OnPositionChanged(const math::CVector3& _v3Pos)
  {
    SetPos(_v3Pos);
  }
  // ------------------------------------
  void CModelComponent::OnRotationChanged(const math::CVector3& _v3Rot)
  {
    SetRotation(_v3Rot);
  }
  // ------------------------------------
  void CModelComponent::OnScaleChanged(const math::CVector3& _v3Scale)
  {
    SetScl(_v3Scale);
  }
  // ------------------------------------
  void CModelComponent::SetPos(const math::CVector3& _v3Pos)
  {
    if (m_wpPrimitive.IsValid())
    {
      m_wpPrimitive->SetPos(_v3Pos);
    }

    if (m_wpModel.IsValid() && !m_wpModelInstance.IsValid())
    {
      m_wpModel->SetPos(_v3Pos);
    }
    else if (m_wpModelInstance.IsValid())
    {
      m_wpModelInstance->SetPos(_v3Pos);
    }
  }
  // ------------------------------------
  math::CVector3 CModelComponent::GetPosition() const
  {
    return m_pOwner->GetPos();
  }
  // ------------------------------------
  void CModelComponent::SetRotation(const math::CVector3& _v3Rot)
  {
    if (m_wpPrimitive.IsValid())
    {
      m_wpPrimitive->SetRot(_v3Rot);
    }

    if (m_wpModel.IsValid() && !m_wpModelInstance.IsValid())
    {
      m_wpModel->SetRot(_v3Rot);
    }
    else if (m_wpModelInstance.IsValid())
    {
      m_wpModelInstance->SetRot(_v3Rot);
    }
  }
  // ------------------------------------
  math::CVector3 CModelComponent::GetRotation() const
  {
    return m_pOwner->GetRot();
  }
  // ------------------------------------
  void CModelComponent::SetScl(const math::CVector3& _v3Scl)
  {
    if (m_wpPrimitive.IsValid())
    {
      m_wpPrimitive->SetScl(_v3Scl);
    }

    if (m_wpModel.IsValid() && !m_wpModelInstance.IsValid())
    {
      m_wpModel->SetScl(_v3Scl);
    }
    else if (m_wpModelInstance.IsValid())
    {
      m_wpModelInstance->SetScl(_v3Scl);
    }
  }
  // ------------------------------------
  math::CVector3 CModelComponent::GetScale() const
  {
    return m_pOwner->GetScl();
  }
#ifdef _DEBUG
  // ------------------------------------
  void CModelComponent::DrawDebug()
  {
    ImGui::Spacing();

    CEntity* pEntity = GetOwner();
    std::string sOwnerName = pEntity->GetName();

    if (m_wpPrimitive.IsValid())
    {
      ImGui::Text("PRIMITIVE");
      const math::CVector3& v3Color = m_wpPrimitive->GetColor();
      float fColor[3] = { v3Color.x, v3Color.y, v3Color.z };
      ImGui::InputFloat3("Color", fColor);
      ImGui::Separator();
      m_wpPrimitive->SetColor(math::CVector3(fColor[0], fColor[1], fColor[2]));
    }
  }
#endif
}
#include "ModelComponent.h"

#include "Engine/Engine.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"

#include "Game/Entity/Entity.h"
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
    if (m_pPrimitive)
    {
      pEngine->DestroyPrimitive(m_pPrimitive);
    }

    // Handle model and instances
    if (m_pRenderInstance)
    {
      m_wpModel->RemoveInstance(m_pRenderInstance->GetInstanceID());
      m_pRenderInstance = nullptr;
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
      render::gfx::TInstances& rInstances = m_wpModel->GetInstances();
      m_pRenderInstance = rInstances.last();

      // Update transform
      m_pRenderInstance->SetPos(GetPosition());
      m_pRenderInstance->SetRot(GetRotation());
      m_pRenderInstance->SetScl(GetScale());
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
    m_pPrimitive = pEngine->CreatePrimitive(_eType, _eRenderMode);
#ifdef _DEBUG
    assert(m_pPrimitive);
#endif

    // Update transform
    CEntity* pOwner = GetOwner();
    if (pOwner)
    {
      m_pPrimitive->SetPos(pOwner->GetPos());
      m_pPrimitive->SetRot(pOwner->GetRot());
    }
  }
  // ------------------------------------
  void CModelComponent::SetCullingEnabled(bool _bCull)
  {
    if (m_pPrimitive)
    {
      m_pPrimitive->SetCullEnabled(_bCull);
    }

    if (m_wpModel.IsValid() && !m_pRenderInstance)
    {
      m_wpModel->SetCullEnabled(_bCull);
    }
    else if (m_pRenderInstance)
    {
      m_pRenderInstance->SetCullEnabled(_bCull);
    }
  }
  // ------------------------------------
  void CModelComponent::SetRenderMode(render::ERenderMode _eRenderMode)
  {
    if (m_pPrimitive)
    {
      m_pPrimitive->SetRenderMode(_eRenderMode);
    }
  }
  // ------------------------------------
  void CModelComponent::SetColor(const math::CVector3& _v3Color)
  {
    if (m_pPrimitive)
    {
      m_pPrimitive->SetColor(_v3Color);
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
    if (m_pPrimitive)
    {
      m_pPrimitive->SetPos(_v3Pos);
    }

    if (m_wpModel.IsValid() && !m_pRenderInstance)
    {
      m_wpModel->SetPos(_v3Pos);
    }
    else if (m_pRenderInstance)
    {
      m_pRenderInstance->SetPos(_v3Pos);
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
    if (m_pPrimitive)
    {
      m_pPrimitive->SetRot(_v3Rot);
      return;
    }

    if (m_wpModel.IsValid() && !m_pRenderInstance)
    {
      m_wpModel->SetRot(_v3Rot);
    }
    else if (m_pRenderInstance)
    {
      m_pRenderInstance->SetRot(_v3Rot);
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
    if (m_pPrimitive)
    {
      m_pPrimitive->SetScl(_v3Scl);
    }

    if (m_wpModel.IsValid() && !m_pRenderInstance)
    {
      m_wpModel->SetScl(_v3Scl);
    }
    else if (m_pRenderInstance)
    {
      m_pRenderInstance->SetScl(_v3Scl);
    }
  }
  // ------------------------------------
  math::CVector3 CModelComponent::GetScale() const
  {
    return m_pOwner->GetScl();
  }
}
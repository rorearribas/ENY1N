#include "ModelComponent.h"

#include "Engine/Engine.h"
#include "Engine/Render/Graphics/Primitive.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Render/Graphics/RenderInstance.h"

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
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    if (m_pPrimitive)
    {
      pEngine->DestroyPrimitive(m_pPrimitive);
    }
    if (m_pModel)
    {
      pEngine->DestroyModel(m_pModel);
    }
  }
  // ------------------------------------
  void CModelComponent::LoadModel(const char* _sModelPath)
  {
    // Flush
    Clean();

    // Create model
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    render::gfx::CModel* pModel = pEngine->LoadModel(_sModelPath);
#ifdef _DEBUG
    assert(pModel);
#endif
    if (pModel->HasInstances())
    {
      // Assign last created!
      render::gfx::TInstances& rInstances = pModel->GetInstances();
      m_pInstance = (rInstances.end() - 1);
      m_pModel = nullptr;

      // Update transform
      m_pInstance->SetPosition(GetPosition());
      m_pInstance->SetRotation(GetRotation());
      m_pInstance->SetScale(GetScale());
    }
    else
    {
      // Set model
      m_pModel = pModel;
      m_pInstance = nullptr;

      // Update transform
      m_pModel->SetPosition(GetPosition());
      m_pModel->SetRotation(GetRotation());
      m_pModel->SetScale(GetScale());
    }
  }
  // ------------------------------------
  void CModelComponent::CreatePrimitive(render::EPrimitiveType _eType, render::ERenderMode _eRenderMode)
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
      m_pPrimitive->SetPosition(pOwner->GetPosition());
      m_pPrimitive->SetRotation(pOwner->GetRotation());
    }
  }
  // ------------------------------------
  void CModelComponent::SetCullingEnabled(bool _bCull)
  {
    if (m_pPrimitive)
    {
      m_pPrimitive->SetCullingEnabled(_bCull);
    }
    if (m_pModel)
    {
      m_pModel->SetCullingEnabled(_bCull);
    }
    if (m_pInstance)
    {
      m_pInstance->SetCullingEnabled(_bCull);
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
    SetPosition(_v3Pos);
  }
  // ------------------------------------
  void CModelComponent::OnRotationChanged(const math::CVector3& _v3Rot)
  {
    SetRotation(_v3Rot);
  }
  // ------------------------------------
  void CModelComponent::OnScaleChanged(const math::CVector3& _v3Scale)
  {
    SetScale(_v3Scale);
  }
  // ------------------------------------
  void CModelComponent::SetPosition(const math::CVector3& _v3Position)
  {
    if (m_pPrimitive)
    {
      m_pPrimitive->SetPosition(_v3Position);
    }
    if (m_pModel)
    {
      m_pModel->SetPosition(_v3Position);
    }
    if (m_pInstance)
    {
      m_pInstance->SetPosition(_v3Position);
    }
  }
  // ------------------------------------
  const math::CVector3& CModelComponent::GetPosition() const
  {
    return m_pOwner->GetPosition();
  }
  // ------------------------------------
  void CModelComponent::SetRotation(const math::CVector3& _v3Rot)
  {
    if (m_pPrimitive)
    {
      m_pPrimitive->SetRotation(_v3Rot);
    }
    if (m_pModel)
    {
      m_pModel->SetRotation(_v3Rot);
    }
    if (m_pInstance)
    {
      m_pInstance->SetRotation(_v3Rot);
    }
  }
  // ------------------------------------
  const math::CVector3& CModelComponent::GetRotation() const
  {
    return m_pOwner->GetRotation();
  }
  // ------------------------------------
  void CModelComponent::SetScale(const math::CVector3& _v3Scl)
  {
    if (m_pPrimitive)
    {
      m_pPrimitive->SetScale(_v3Scl);
    }
    if (m_pModel)
    {
      m_pModel->SetScale(_v3Scl);
    }
    if (m_pInstance)
    {
      m_pInstance->SetScale(_v3Scl);
    }
  }
  // ------------------------------------
  const math::CVector3& CModelComponent::GetScale() const
  {
    return m_pOwner->GetScale();
  }
  // ------------------------------------
}
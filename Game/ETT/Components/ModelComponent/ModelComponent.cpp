#include "ModelComponent.h"
#include "Engine/Base/Engine.h"
#include "Game/ETT/Entity.h"
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
  void CModelComponent::Update(float _fDeltaTime)
  {
    Super::Update(_fDeltaTime);
  }
  // ------------------------------------
  void CModelComponent::LoadModel(const char* _sModelPath, const char* _sBaseMltDir)
  {
    // Flush
    Clean();

    // Create model
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    m_pModel = pEngine->CreateModel(_sModelPath, _sBaseMltDir);
#ifdef _DEBUG
    assert(m_pModel);
#endif
  }
  // ------------------------------------
  void CModelComponent::CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType _ePrimitiveType, render::ERenderMode _eRenderMode)
  {
    // Flush
    Clean();

    // Create primitive
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    m_pPrimitive = pEngine->CreatePrimitive(_ePrimitiveType, _eRenderMode);
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
  void CModelComponent::SetPrimitiveRenderMode(render::ERenderMode _eRenderMode)
  {
    if (m_pPrimitive)
    {
      m_pPrimitive->SetRenderMode(_eRenderMode);
    }
  }
  // ------------------------------------
  void CModelComponent::SetPrimitiveColor(const math::CVector3& _v3Color)
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
  }
  // ------------------------------------
  const math::CVector3& CModelComponent::GetRotation() const
  {
    return m_pOwner->GetRotation();
  }
  // ------------------------------------
  void CModelComponent::SetScale(const math::CVector3& _v3Scale)
  {
    if (m_pPrimitive)
    {
      m_pPrimitive->SetScale(_v3Scale);
    }
    if (m_pModel)
    {
      m_pModel->SetScale(_v3Scale);
    }
  }
  // ------------------------------------
  const math::CVector3& CModelComponent::GetScale() const
  {
    return m_pOwner->GetScale();
  }
  // ------------------------------------
}
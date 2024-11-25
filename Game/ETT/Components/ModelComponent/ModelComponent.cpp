#include "ModelComponent.h"
#include "Engine/Base/Engine.h"
#include "Game/ETT/Entity.h"
#include <cassert>

namespace game
{
  CModelComponent::CModelComponent(CEntity* _pEntity) : CComponent(_pEntity)
  {
    if (engine::CEngine::HasSingleton())
    {
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      m_pPrimitive = pEngine->CreatePrimitive(render::graphics::CPrimitive::CUBE);
      assert(m_pPrimitive);
    }
  }
  // ------------------------------------
  CModelComponent::~CModelComponent()
  {
    if (m_pPrimitive && engine::CEngine::HasSingleton())
    {
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      pEngine->DestroyPrimitive(m_pPrimitive);
    }
  }
  // ------------------------------------
  void CModelComponent::Update(float _fDeltaTime)
  {
    Super::Update(_fDeltaTime);
  }
  // ------------------------------------
  void CModelComponent::OnPositionChanged(const maths::CVector3& _v3Pos)
  {
    SetPosition(_v3Pos);
  }
  // ------------------------------------
  void CModelComponent::OnRotationChanged(const maths::CVector3& _v3Rot)
  {
    SetRotation(_v3Rot);
  }
  // ------------------------------------
  void CModelComponent::OnScaleChanged(const maths::CVector3& _v3Scale)
  {
    SetRotation(_v3Scale);
  }
  // ------------------------------------
  void CModelComponent::SetPosition(const maths::CVector3& _v3Position)
  {
    if (m_pPrimitive)
    {
      m_pPrimitive->SetPosition(_v3Position);
    }
  }
  // ------------------------------------
  const maths::CVector3& CModelComponent::GetPosition() const
  {
    return m_pOwner->GetPosition();
  }
  // ------------------------------------
  void CModelComponent::SetRotation(const maths::CVector3& _v3Rot)
  {
    if (m_pPrimitive)
    {
      m_pPrimitive->SetRotation(_v3Rot);
    }
  }
  // ------------------------------------
  const maths::CVector3& CModelComponent::GetRotation() const
  {
    return m_pOwner->GetRotation();
  }
  // ------------------------------------
  void CModelComponent::SetScale(const maths::CVector3& _v3Scale)
  {
    if (m_pPrimitive)
    {
      m_pPrimitive->SetScale(_v3Scale);
    }
  }
  // ------------------------------------
  const maths::CVector3& CModelComponent::GetScale() const
  {
    return m_pOwner->GetScale();
  }
  // ------------------------------------
}
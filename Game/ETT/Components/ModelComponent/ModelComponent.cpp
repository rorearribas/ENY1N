#include "ModelComponent.h"
#include "Engine/Base/Engine.h"
#include "Game/ETT/Entity.h"
#include <cassert>

namespace game
{
  // ------------------------------------
  CModelComponent::~CModelComponent()
  {
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    if (m_pPrimitive && pEngine) { pEngine->DestroyPrimitive(m_pPrimitive); }
    if (m_pModel && pEngine) { pEngine->DestroyModel(m_pModel); }
  }
  // ------------------------------------
  void CModelComponent::UpdateComponent(float _fDeltaTime)
  {
    Super::UpdateComponent(_fDeltaTime);
  }
  // ------------------------------------
  void CModelComponent::LoadModel(const char* _sModelPath)
  {
    if (engine::CEngine::HasSingleton())
    {
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      m_pModel = pEngine->CreateModel(_sModelPath);
      assert(m_pModel);

      /*if (m_pPrimitive)
      {
        pEngine->DestroyPrimitive(m_pPrimitive);
      }*/
    }
  }
  // ------------------------------------
  void CModelComponent::CreatePrimitive(render::graphics::CPrimitive::EPrimitiveType _ePrimitiveType)
  {
    if (engine::CEngine::HasSingleton())
    {
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      m_pPrimitive = pEngine->CreatePrimitive(_ePrimitiveType);
      assert(m_pPrimitive);

      if (m_pModel)
      {
        pEngine->DestroyModel(m_pModel);
      }
    }
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
    SetScale(_v3Scale);
  }
  // ------------------------------------
  void CModelComponent::SetPosition(const maths::CVector3& _v3Position)
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
    if (m_pModel)
    {
      m_pModel->SetRotation(_v3Rot);
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
    if (m_pModel)
    {
      m_pModel->SetScale(_v3Scale);
    }
  }
  // ------------------------------------
  const maths::CVector3& CModelComponent::GetScale() const
  {
    return m_pOwner->GetScale();
  }
  // ------------------------------------
}
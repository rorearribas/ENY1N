#include "ModelComponent.h"
#include "Engine/Base/Engine.h"

namespace game
{
  CModelComponent::CModelComponent(CEntity* _pEntity) : CComponent(_pEntity)
  {
    if (engine::CEngine::HasSingleton())
    {
      engine::CEngine* pEngine = engine::CEngine::GetInstance();
      m_pPrimitive = pEngine->CreatePrimitive(render::primitive::CPrimitive::CUBE);
    }
  }
  // ------------------------------------
  void CModelComponent::Update(float /*_fDeltaTime*/)
  {
    SetPosition(m_pOwner->GetPosition());
    SetRotation(m_pOwner->GetRotation());
  }
  // ------------------------------------
  void CModelComponent::SetPosition(const maths::CVector3& _v3Position)
  {
    m_pPrimitive->SetPosition(_v3Position);
  }
  // ------------------------------------
  void CModelComponent::SetRotation(const maths::CVector3& _v3Rot)
  {
    m_pPrimitive->SetRotation(_v3Rot);
  }
  // ------------------------------------
  void CModelComponent::SetScale(const maths::CVector3& _v3Scale)
  {
    m_pPrimitive->SetScale(_v3Scale);
  }
}
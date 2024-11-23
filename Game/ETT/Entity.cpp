#include "Entity.h"
#include "Components/Component.h"
#include <algorithm>

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
    if (!m_bCanTickActor) return;



    // Update components
    for (game::CComponent* pComponent : m_vctComponents)
    {
      if (pComponent)
      {
        pComponent->Update(_fDeltaTime);
      }
    }
  }
  // ------------------------------------
  void CEntity::SetPosition(const maths::CVector3& _v3Position)
  {
    m_oTransform.SetPosition(_v3Position);
  }
  // ------------------------------------
  void CEntity::MovePosition(const maths::CVector3& _v3Delta)
  {
    m_oTransform.MovePosition(_v3Delta);
  }
  // ------------------------------------
  void CEntity::SetRotation(const maths::CVector3& _v3Rot)
  {
    m_oTransform.SetRotation(_v3Rot);
  }
  // ------------------------------------
  void CEntity::AddRotation(const maths::CVector3 _v3Delta)
  {
    m_oTransform.AddRotation(_v3Delta);
  }
  // ------------------------------------
  void CEntity::SetScale(const maths::CVector3& _v3Scale)
  {
    m_oTransform.SetScale(_v3Scale);
  }
  // ------------------------------------
  void CEntity::DestroyAllComponents()
  {
    std::for_each(m_vctComponents.begin(), m_vctComponents.end(), [](CComponent* _pComponent)
    {
      if (_pComponent)
      {
        delete _pComponent;
        _pComponent = nullptr;
      }
    });
  }
}
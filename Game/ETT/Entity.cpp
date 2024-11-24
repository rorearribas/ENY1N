#include "Entity.h"
#include "Components/Component.h"
#include <algorithm>

namespace game
{
  CEntity::CEntity(const char* _sEntityName) : m_sEntityName(_sEntityName)
  {
  }
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
    // Draw debug
    for (CComponent* pComponent : m_vctComponents)
    {
      if (pComponent)
      {
        pComponent->DrawDebug();
      }
    }
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
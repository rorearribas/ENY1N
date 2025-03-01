#include "CollisionManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Engine/Collisions/BoxCollider.h"
#include "Engine/Collisions/SphereCollider.h"
#include <iostream>
#include <cassert>

namespace collisions
{
  void CCollisionManager::Update(float /*_fDeltaTime*/)
  {
    // Update collisions
    for (uint32_t uI = 0; uI < m_vctColliders.CurrentSize(); ++uI)
    {
      for (uint32_t uJ = uI + 1; uJ < m_vctColliders.CurrentSize(); ++uJ)
      {
        if (m_vctColliders[uI]->CheckCollision(*m_vctColliders[uJ]))
        {
          ComputeCollision(m_vctColliders[uI], m_vctColliders[uJ]);
        }
      }
    }
  }
  // ------------------------------------
  void CCollisionManager::ComputeCollision(collisions::CCollider* /*_pColliderA*/, collisions::CCollider* _pColliderB)
  {
    /*_pColliderA->GetRigigbody()->Clear();*/
    _pColliderB->GetRigigbody()->Clear();
    _pColliderB->GetRigigbody()->SetImpactPoint(_pColliderB->GetPosition());
  }
  // ------------------------------------
  collisions::CCollider* CCollisionManager::CreateCollider(collisions::EColliderType _eColliderType)
  {
    if (m_vctColliders.CurrentSize() >= m_vctColliders.GetMaxSize())
    {
      std::cout << "You have reached maximum colliders" << std::endl;
      return nullptr;
    }

    switch (_eColliderType)
    {
      case collisions::BOX_COLLIDER: return m_vctColliders.CreateItem<collisions::CBoxCollider>();
      case collisions::SPHERE_COLLIDER: return m_vctColliders.CreateItem<collisions::CSphereCollider>();
      default: return nullptr;
    }
  }
  // ------------------------------------
  void CCollisionManager::DestroyCollider(collisions::CCollider*& _pCollider_)
  {
    bool bOk = m_vctColliders.RemoveItem(_pCollider_);
    if (!bOk) { std::cout << "Error: Failed to remove collider" << std::endl; }
    _pCollider_ = nullptr;
  }
  // ------------------------------------
  void CCollisionManager::Flush()
  {
    m_vctColliders.ClearAll();
  }
}
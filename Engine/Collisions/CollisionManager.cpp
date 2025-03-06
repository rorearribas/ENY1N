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
    // Check collisions
    for (uint32_t uI = 0; uI < m_vctColliders.CurrentSize(); ++uI)
    {
      collisions::CCollider* pCurrentCollider = m_vctColliders[uI];
      for (uint32_t uJ = uI + 1; uJ < m_vctColliders.CurrentSize(); ++uJ)
      {
        collisions::CCollider* pTargetCollider = m_vctColliders[uJ];
        if (pCurrentCollider->CheckCollision(*pTargetCollider))
        {
          ComputeCollision(pCurrentCollider, pTargetCollider);
        }
      }
    }
  }
  // ------------------------------------
  void CCollisionManager::ComputeCollision(collisions::CCollider* _pColliderA, collisions::CCollider* _pColliderB)
  {
     SHitEvent oHitEvent;

    _pColliderA->m_oOnCollisionEvent.Execute(_pColliderA);
    _pColliderB->m_oOnCollisionEvent.Execute(_pColliderA);
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
  void CCollisionManager::Clean()
  {
    m_vctColliders.ClearAll();
  }
}
#include "CollisionManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Engine/Physics/BoxCollider.h"
#include "Engine/Physics/SphereCollider.h"
#include <iostream>
#include <cassert>

namespace physics
{
  void CCollisionManager::Update(float /*_fDeltaTime*/)
  {
    for (uint32_t uI = 0; uI < m_vctColliders.CurrentSize(); ++uI)
    {
      for (uint32_t uJ = uI + 1; uJ < m_vctColliders.CurrentSize(); ++uJ)
      {
        bool bCollision = m_vctColliders[uI]->CheckCollision(*m_vctColliders[uJ]);
        if (bCollision)
        {
          std::cout << "WORKS" << std::endl;
        }
      }
    }
  }
  // ------------------------------------
  physics::CCollider* CCollisionManager::CreateCollider(physics::EColliderType _eColliderType)
  {
    if (m_vctColliders.CurrentSize() >= m_vctColliders.GetMaxSize())
    {
      std::cout << "You have reached maximum colliders" << std::endl;
      return nullptr;
    }

    switch (_eColliderType)
    {
      case physics::BOX_COLLIDER: return m_vctColliders.CreateItem<physics::CBoxCollider>();
      case physics::SPHERE_COLLIDER: return m_vctColliders.CreateItem<physics::CSphereCollider>();
      default: return nullptr;
    }
  }
  // ------------------------------------
  void CCollisionManager::DestroyCollider(physics::CCollider*& _pCollider_)
  {
    bool bOk = m_vctColliders.RemoveItem(_pCollider_);
    if (!bOk) { std::cout << "Error: Failed to remove collider" << std::endl; }
    _pCollider_ = nullptr;
  }
}
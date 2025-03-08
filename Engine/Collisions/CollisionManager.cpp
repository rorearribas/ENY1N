#include "CollisionManager.h"
#include "Libs/Macros/GlobalMacros.h"
#include "Engine/Collisions/BoxCollider.h"
#include "Engine/Collisions/SphereCollider.h"
#include <iostream>
#include <cassert>
#include <unordered_map>
#include <unordered_set>

namespace collisions
{
  static std::unordered_map<collisions::CCollider*, std::unordered_set<collisions::CCollider*>> s_dctCollisions;

  void CCollisionManager::Update(float /*_fDeltaTime*/)
  {
    // Check collisions
    for (uint32_t uI = 0; uI < m_vctColliders.CurrentSize(); ++uI)
    {
      collisions::CCollider* pCurrentCollider = m_vctColliders[uI];

      // Initialize the set of previous colliders for this collider if not already done
      if (s_dctCollisions.find(pCurrentCollider) == s_dctCollisions.end())
      {
        s_dctCollisions[pCurrentCollider] = std::unordered_set<collisions::CCollider*>();
      }

      for (uint32_t uJ = uI + 1; uJ < m_vctColliders.CurrentSize(); ++uJ)
      {
        // Check collision
        collisions::CCollider* pTargetCollider = m_vctColliders[uJ];
        if (pCurrentCollider->CheckCollision(*pTargetCollider))
        {
          // Collision Enter
          bool bCollisionEnter = s_dctCollisions[pCurrentCollider].find(pTargetCollider) != s_dctCollisions[pCurrentCollider].end();
          if (!bCollisionEnter)
          {
            pCurrentCollider->m_oOnCollisionEnter(pTargetCollider);
            pTargetCollider->m_oOnCollisionEnter(pCurrentCollider);
            s_dctCollisions[pCurrentCollider].insert(pTargetCollider);
          }
          // Collision Stay
          else
          {
            pCurrentCollider->m_oOnCollisionStay(pTargetCollider);
            pTargetCollider->m_oOnCollisionStay(pCurrentCollider);
          }
        }
        else
        {
          // Collision Exit
          if (s_dctCollisions[pCurrentCollider].find(pTargetCollider) != s_dctCollisions[pCurrentCollider].end())
          {
            pCurrentCollider->m_oOnCollisionExit(pTargetCollider);
            pTargetCollider->m_oOnCollisionExit(pCurrentCollider);

            // Remove from the previous collisions set as they are no longer colliding
            s_dctCollisions[pCurrentCollider].erase(pTargetCollider);
          }
        }
      }
    }
  }
  // ------------------------------------
  void CCollisionManager::ComputeCollision(collisions::CCollider* _pColliderA, collisions::CCollider* _pColliderB)
  {
    _pColliderA->m_oOnCollisionEnter(_pColliderA);
    _pColliderB->m_oOnCollisionEnter(_pColliderA);
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
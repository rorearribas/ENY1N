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
  static std::unordered_map<collisions::CCollider*, std::unordered_set<collisions::CCollider*>> s_dctHandleCollisions;

  CCollisionManager::~CCollisionManager()
  {
    Clean();
  }
  // ------------------------------------
  void CCollisionManager::Update(float _fDeltaTime)
  {
    UNUSED_VARIABLE(_fDeltaTime);

    // Check collisions
    for (uint32_t uI = 0; uI < m_vctColliders.CurrentSize(); ++uI)
    {
      // Get current collider
      collisions::CCollider* pCollider = m_vctColliders[uI];

      // Initialize the set of previous colliders for this collider if not already done
      if (s_dctHandleCollisions.find(pCollider) == s_dctHandleCollisions.end())
      {
        s_dctHandleCollisions[pCollider] = std::unordered_set<collisions::CCollider*>();
      }

      for (uint32_t uJ = uI + 1; uJ < m_vctColliders.CurrentSize(); ++uJ)
      {
        // Get target collider
        collisions::CCollider* pTargetCollider = m_vctColliders[uJ];

        // Create hit event data
        collisions::SHitEvent oHitEvent = collisions::SHitEvent();

        if (pCollider->CheckCollision(*pTargetCollider, oHitEvent))
        {
          // Collision Enter
          bool bCollisionEnter = s_dctHandleCollisions[pCollider].find(pTargetCollider) != s_dctHandleCollisions[pCollider].end();
          if (!bCollisionEnter)
          {
            // Notify to current collider
            oHitEvent.Object = pTargetCollider->GetOwner();
            pCollider->m_oOnCollisionEnter(oHitEvent);

            // Notification to target collider
            oHitEvent.Normal *= -1.0f;
            oHitEvent.Object = pCollider->GetOwner();
            pTargetCollider->m_oOnCollisionEnter(oHitEvent);

            // Register collision
            s_dctHandleCollisions[pCollider].insert(pTargetCollider);
          }
          else // Collision Stay
          {
            // Notify to current collider
            oHitEvent.Object = pTargetCollider->GetOwner();
            pCollider->m_oOnCollisionStay(oHitEvent);

            // Notification to target collider
            oHitEvent.Normal *= -1.0f;
            oHitEvent.Object = pCollider->GetOwner();
            pTargetCollider->m_oOnCollisionStay(oHitEvent);
          }
        }
        else // Collision Exit
        {
          if (s_dctHandleCollisions[pCollider].find(pTargetCollider) != s_dctHandleCollisions[pCollider].end())
          {
            // Notify to current collider
            oHitEvent.Object = pTargetCollider->GetOwner();
            pCollider->m_oOnCollisionExit(oHitEvent);

            // Notification to target collider
            oHitEvent.Object = pCollider->GetOwner();
            pTargetCollider->m_oOnCollisionExit(oHitEvent);

            // Remove from the previous collisions
            s_dctHandleCollisions[pCollider].erase(pTargetCollider);
          }
        }
      }
    }
  }
  // ------------------------------------
  collisions::CCollider* CCollisionManager::CreateCollider(collisions::EColliderType _eColliderType, void* _pOwner)
  {
    if (m_vctColliders.CurrentSize() >= m_vctColliders.GetMaxSize())
    {
      std::cout << "You have reached maximum colliders" << std::endl;
      return nullptr;
    }
    switch (_eColliderType)
    {
      case collisions::BOX_COLLIDER: return m_vctColliders.CreateItem<collisions::CBoxCollider>(_pOwner);
      case collisions::SPHERE_COLLIDER: return m_vctColliders.CreateItem<collisions::CSphereCollider>(_pOwner);
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
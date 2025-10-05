#include "CollisionManager.h"

#include "Engine/Collisions/BoxCollider.h"
#include "Engine/Collisions/SphereCollider.h"
#include "Engine/Collisions/CapsuleCollider.h"

#include "Libs/Macros/GlobalMacros.h"

namespace collision
{
  // ------------------------------------
  void CCollisionManager::Update(float /*_fDeltaTime*/)
  {
    // Check collisions
    for (uint32_t uI = 0; uI < m_vctColliders.CurrentSize(); ++uI)
    {
      // Get current collider
      collision::CCollider* pCollider = m_vctColliders[uI];

      // Initialize the set of previous colliders for this collider if not already done
      if (m_dctHandleCollisions.find(pCollider) == m_dctHandleCollisions.end())
      {
        m_dctHandleCollisions[pCollider] = std::unordered_set<collision::CCollider*>();
      }

      for (uint32_t uJ = uI + 1; uJ < m_vctColliders.CurrentSize(); ++uJ)
      {
        // Get target collider
        collision::CCollider* pTargetCollider = m_vctColliders[uJ];

        // Create hit event data
        collision::SHitEvent oHitEvent = collision::SHitEvent();
        if (pCollider->CheckCollision(*pTargetCollider, oHitEvent))
        {
          // Collision Enter
          bool bCollisionEnter = m_dctHandleCollisions[pCollider].find(pTargetCollider) != m_dctHandleCollisions[pCollider].end();
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
            m_dctHandleCollisions[pCollider].insert(pTargetCollider);
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
          if (m_dctHandleCollisions[pCollider].find(pTargetCollider) != m_dctHandleCollisions[pCollider].end())
          {
            // Notify to current collider
            oHitEvent.Object = pTargetCollider->GetOwner();
            pCollider->m_oOnCollisionExit(oHitEvent);

            // Notification to target collider
            oHitEvent.Object = pCollider->GetOwner();
            pTargetCollider->m_oOnCollisionExit(oHitEvent);

            // Remove from the previous collisions
            m_dctHandleCollisions[pCollider].erase(pTargetCollider);
          }
        }
      }
    }
  }
  // ------------------------------------
  collision::CCollider* CCollisionManager::CreateCollider(collision::EColliderType _eColliderType, void* _pOwner)
  {
    if (m_vctColliders.CurrentSize() >= m_vctColliders.GetMaxSize())
    {
      std::cout << "You have reached maximum colliders!" << std::endl;
      return nullptr;
    }
    switch (_eColliderType)
    {
      case collision::EColliderType::BOX_COLLIDER: return m_vctColliders.RegisterItem<collision::CBoxCollider>(_pOwner);
      case collision::EColliderType::SPHERE_COLLIDER: return m_vctColliders.RegisterItem<collision::CSphereCollider>(_pOwner);
      case collision::EColliderType::CAPSULE_COLLIDER: return m_vctColliders.RegisterItem<collision::CCapsuleCollider>(_pOwner);
      default: return nullptr;
    }
  }
  // ------------------------------------
  void CCollisionManager::DestroyCollider(collision::CCollider*& _pCollider_)
  {
    bool bOk = m_vctColliders.RemoveItem(_pCollider_);
    if (!bOk) { std::cout << "Error removing collider!" << std::endl; }
    _pCollider_ = nullptr;
  }
  // ------------------------------------
  bool CCollisionManager::Raycast(const physics::CRay& _oRaycast, float _fMaxDistance, SHitEvent& oHitEvent_, ECollisionMask _eMask)
  {
    bool bHit = false;
    float fClosestDistance = _fMaxDistance;
    for (uint32_t uI = 0; uI < m_vctColliders.CurrentSize(); ++uI)
    {
      collision::CCollider* pCollider = m_vctColliders[uI];
      const collision::ECollisionMask& eCollMask = pCollider->GetCollisionMask();
      if ((eCollMask & _eMask) == 0)
      {
        continue;
      }

      collision::SHitEvent oHitEvent = collision::SHitEvent();
      bool bIntersect = pCollider->IntersectRay(_oRaycast, oHitEvent, _fMaxDistance);
      bool bCorrectDist = oHitEvent.Distance <= fClosestDistance;
      if (bIntersect && bCorrectDist)
      {
        bHit = true;
        oHitEvent_ = oHitEvent;
        fClosestDistance = oHitEvent.Distance;
      }
    }
    return bHit;
  }
  // ------------------------------------
  bool CCollisionManager::RaycastAll(const physics::CRay& _oRaycast, float _fMaxDistance, std::vector<SHitEvent>& _vctOutHits_, ECollisionMask _eMask)
  {
    _vctOutHits_.clear();
    for (uint32_t uI = 0; uI < m_vctColliders.CurrentSize(); ++uI)
    {
      collision::CCollider* pCollider = m_vctColliders[uI];
      if ((pCollider->GetCollisionMask() & _eMask) == 0)
      {
        continue;
      }

      collision::SHitEvent oHitEvent = collision::SHitEvent();
      bool bIntersect = pCollider->IntersectRay(_oRaycast, oHitEvent, _fMaxDistance);
      bool bCorrectDist = oHitEvent.Distance <= _fMaxDistance;
      if (bIntersect && bCorrectDist)
      {
        _vctOutHits_.emplace_back(oHitEvent);
      }
    }
    return _vctOutHits_.size() > 0;
  }
  // ------------------------------------
  void CCollisionManager::Clean()
  {
    m_vctColliders.ClearAll();
  }
}
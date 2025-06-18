#include "CollisionManager.h"

#include "Engine/Collisions/BoxCollider.h"
#include "Engine/Collisions/SphereCollider.h"
#include "Engine/Collisions/CapsuleCollider.h"

#include "Libs/Macros/GlobalMacros.h"
#include <unordered_map>
#include <unordered_set>

namespace collision
{
  static std::unordered_map<collision::CCollider*, std::unordered_set<collision::CCollider*>> s_dctHandleCollisions;

  CCollisionManager::~CCollisionManager()
  {
    Clean();
  }
  // ------------------------------------
  void CCollisionManager::Update(float _fDeltaTime)
  {
    UNUSED_VAR(_fDeltaTime);

    // Check collisions
    for (uint32_t uI = 0; uI < m_vctColliders.CurrentSize(); ++uI)
    {
      // Get current collider
      collision::CCollider* pCollider = m_vctColliders[uI];

      // Initialize the set of previous colliders for this collider if not already done
      if (s_dctHandleCollisions.find(pCollider) == s_dctHandleCollisions.end())
      {
        s_dctHandleCollisions[pCollider] = std::unordered_set<collision::CCollider*>();
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
  collision::CCollider* CCollisionManager::CreateCollider(collision::EColliderType _eColliderType, void* _pOwner)
  {
    if (m_vctColliders.CurrentSize() >= m_vctColliders.GetMaxSize())
    {
      std::cout << "You have reached maximum colliders" << std::endl;
      return nullptr;
    }
    switch (_eColliderType)
    {
      case collision::EColliderType::BOX_COLLIDER: return m_vctColliders.CreateItem<collision::CBoxCollider>(_pOwner);
      case collision::EColliderType::SPHERE_COLLIDER: return m_vctColliders.CreateItem<collision::CSphereCollider>(_pOwner);
      case collision::EColliderType::CAPSULE_COLLIDER: return m_vctColliders.CreateItem<collision::CCapsuleCollider>(_pOwner);
      default: return nullptr;
    }
  }
  // ------------------------------------
  void CCollisionManager::DestroyCollider(collision::CCollider*& _pCollider_)
  {
    bool bOk = m_vctColliders.RemoveItem(_pCollider_);
    if (!bOk) { std::cout << "Error: Failed to remove collider" << std::endl; }
    _pCollider_ = nullptr;
  }
  // ------------------------------------
  bool CCollisionManager::Raycast(const physics::CRay& _oRaycast, SHitEvent& _oHitEvent_, float _fMaxDistance, ECollisionMask _eCollisionMask)
  {
    bool bHit = false;
    float fClosestDistance = _fMaxDistance;

    for (uint32_t uI = 0; uI < m_vctColliders.CurrentSize(); ++uI)
    {
      collision::CCollider* pCollider = m_vctColliders[uI];
      const collision::ECollisionMask& eCollMask = pCollider->GetCollisionMask();
      if((eCollMask & _eCollisionMask) == 0) continue;

      collision::SHitEvent oHitEvent = collision::SHitEvent();
      bool bIntersect = pCollider->IntersectRay(_oRaycast, oHitEvent, _fMaxDistance);
      bool bCorrectDist = _oHitEvent_.Distance < fClosestDistance;
      if (bIntersect && bCorrectDist)
      {
        _oHitEvent_ = oHitEvent;
        fClosestDistance = oHitEvent.Distance; // Update closest dista
        bHit = true;
      }
    }
    return bHit;
  }
  // ------------------------------------
  bool CCollisionManager::RaycastAll(const physics::CRay& _oRaycast, std::vector<SHitEvent>& _vctHits_, float _fMaxDistance, ECollisionMask _eCollisionMask)
  {
    _vctHits_.clear();
    for (uint32_t uI = 0; uI < m_vctColliders.CurrentSize(); ++uI)
    {
      collision::CCollider* pCollider = m_vctColliders[uI];
      if ((pCollider->GetCollisionMask() & _eCollisionMask) == 0) continue;

      collision::SHitEvent oHitEvent = collision::SHitEvent();
      bool bIntersect = pCollider->IntersectRay(_oRaycast, oHitEvent, _fMaxDistance);
      bool bCorrectDist = oHitEvent.Distance < _fMaxDistance;

      if (bIntersect && bCorrectDist)
      {
        _vctHits_.emplace_back(oHitEvent);
      }
    }
    return _vctHits_.size() > 0;
  }
  // ------------------------------------
  void CCollisionManager::Clean()
  {
    m_vctColliders.ClearAll();
  }
}
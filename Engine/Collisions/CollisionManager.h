#pragma once
#include "Engine/Collisions/Collider.h"
#include "Engine/Utils/Ray.h"
#include "Libs/Utils/Singleton.h"
#include "Libs/Utils/FixedList.h"
#include "Libs/Utils/Delegate.h"
#include <unordered_map>
#include <unordered_set>

namespace collision
{
  class CCollisionManager : public utils::CSingleton<CCollisionManager>
  {
  public:
    static const uint32_t s_uMaxColliders = 250;
    typedef utils::CFixedList<collision::CCollider, s_uMaxColliders> TColliderList;

  public:
    CCollisionManager() {}
    ~CCollisionManager() { Clean(); }

    void Update(float _fDeltaTime);

    const TColliderList& GetColliderList() { return m_lstColliders; }
    collision::CCollider* CreateCollider(collision::EColliderType _eColliderType, void* _pOwner);
    void DestroyCollider(collision::CCollider*& _pCollider_);

    bool Raycast(const physics::CRay& _oRaycast, float _fMaxDistance, SHitEvent& _oHitEvent_, ECollisionMask _eMask = ECollisionMask::DEFAULT);
    bool RaycastAll(const physics::CRay& _oRaycast, float _fMaxDistance, std::vector<SHitEvent>& _lstHits_, ECollisionMask _eMask = ECollisionMask::DEFAULT);

  private:
    void Clean();

  private:
    TColliderList m_lstColliders;
    std::unordered_map<collision::CCollider*, std::unordered_set<collision::CCollider*>> m_dctHandleCollisions;
  };
}


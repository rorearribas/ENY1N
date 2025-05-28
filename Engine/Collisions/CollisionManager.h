#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Collisions/Collider.h"
#include "Libs/Utils/FixedList.h"
#include "Libs/Utils/Delegate.h"
#include "../Physics/Ray.h"

namespace collision
{
  class CCollisionManager : public utils::CSingleton<CCollisionManager> 
  {
  public:
    static const uint32_t s_uMaxColliders = 250;
    typedef utils::CFixedList<collision::CCollider, s_uMaxColliders> TColliderList;

  public:
    CCollisionManager() {}
    ~CCollisionManager();

    void Update(float _fDeltaTime);

    const TColliderList& GetColliderList() { return m_vctColliders; }
    collision::CCollider* CreateCollider(collision::EColliderType _eColliderType, void* _pOwner);
    void DestroyCollider(collision::CCollider*& _pCollider_);

    bool Raycast(const CRay& _oRaycast, SHitEvent& _oHitEvent_, float _fMaxDistance, ECollisionMask _eCollisionMask = ECollisionMask::DEFAULT);
    bool RaycastAll(const CRay& _oRaycast, std::vector<SHitEvent>& _vctHits_, float _fMaxDistance, ECollisionMask _eCollisionMask = ECollisionMask::DEFAULT);

  private:
    void Clean();

  private:
    TColliderList m_vctColliders;
  };
}


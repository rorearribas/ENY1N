#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Collisions/Collider.h"
#include "Libs/Utils/FixedList.h"
#include "Libs/Utils/Delegate.h"

namespace collisions
{
  struct SHitEvent 
  {
    CCollider* Object;
    maths::CVector3 Normal;
    maths::CVector3 ImpactPoint;
  };

  class CCollisionManager : public utils::CSingleton<CCollisionManager>
  {
  public:
    static const uint32_t s_uMaxColliders = 250;
    typedef utils::CFixedList<collisions::CCollider, s_uMaxColliders> TColliderList;
    typedef utils::CDelegate<void(collisions::CCollider*, collisions::CCollider*)> TOnNotifyCollisionDelegate;

  public:
    CCollisionManager() {}
    ~CCollisionManager() { Clean(); }

    void Update(float _fDeltaTime);

    collisions::CCollider* CreateCollider(collisions::EColliderType _eColliderType);
    void DestroyCollider(collisions::CCollider*& _pCollider_);

  private:
    void ComputeCollision(collisions::CCollider* _pColliderA, collisions::CCollider* _pColliderB);
    void Clean();

  private:
    TColliderList m_vctColliders;
  };
}


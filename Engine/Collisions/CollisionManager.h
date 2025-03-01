#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Collisions/Collider.h"
#include "Libs/Utils/FixedList.h"

namespace collisions
{
  class CCollisionManager : public utils::CSingleton<CCollisionManager>
  {
  public:
    static const uint32_t s_uMaxColliders = 250;
    typedef utils::CFixedList<collisions::CCollider, s_uMaxColliders> TColliderList;

  public:
    CCollisionManager() {}
    ~CCollisionManager() { Flush(); }

    void Update(float _fDeltaTime);

    collisions::CCollider* CreateCollider(collisions::EColliderType _eColliderType);
    void DestroyCollider(collisions::CCollider*& _pCollider_);

  private:
    void ComputeCollision(collisions::CCollider* _pCollisionA, collisions::CCollider* _CollisionB);

    void Flush();
    TColliderList m_vctColliders;
  };
}


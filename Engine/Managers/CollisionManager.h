#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Physics/Collider.h"
#include "Libs/Utils/FixedList.h"

namespace physics
{
  class CCollisionManager : public utils::CSingleton<CCollisionManager>
  {
  public:
    static const uint32_t s_uMaxCollisions = 100;
    typedef utils::CFixedList<physics::CCollider, s_uMaxCollisions> TColliderList;

  public:
    void Update(float _fDeltaTime);

    physics::CCollider* CreateCollider(physics::EColliderType _eColliderType);
    void DestroyCollider(physics::CCollider* _pCollider);

  private:
    TColliderList m_vctColliders;
  };
}


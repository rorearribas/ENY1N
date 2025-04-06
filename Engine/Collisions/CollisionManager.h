#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Collisions/Collider.h"
#include "Libs/Utils/FixedList.h"
#include "Libs/Utils/Delegate.h"

namespace collisions
{
  class CCollisionManager : public utils::CSingleton<CCollisionManager> 
  {
  public:
    static const uint32_t s_uMaxColliders = 250;
    typedef utils::CFixedList<collisions::CCollider, s_uMaxColliders> TColliderList;

  public:
    CCollisionManager() {}
    ~CCollisionManager();

    void Update(float _fDeltaTime);

    const TColliderList& GetColliderList() { return m_vctColliders; }
    collisions::CCollider* CreateCollider(collisions::EColliderType _eColliderType, void* _pOwner);
    void DestroyCollider(collisions::CCollider*& _pCollider_);

  private:
    void Clean();

  private:
    TColliderList m_vctColliders;
  };
}


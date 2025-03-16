#pragma once
#include "Game/ETT/Entity.h"
#include "Libs/Utils/Singleton.h"
#include "Engine/Collisions/CollisionManager.h"
#include <array>

namespace collisions { class CCollider; }

namespace game
{
  class CGameManager : public utils::CSingleton<CGameManager>
  {
  public:
    static int constexpr s_iMaxEntities = 1000;
    typedef std::array<CEntity*, s_iMaxEntities> TEntitiesList;

  public:
    ~CGameManager();

    void Update(float _fDeltaTime);
    void SetDebugMode(bool _bStatus) { m_bDebugMode = _bStatus; }

    CEntity* CreateEntity(const char* _sEntityName);
    bool DestroyEntity(const char* _sEntityName);

  protected:
    void OnNotifyCollisions(collisions::CCollider*, collisions::CCollider*);

  private:
    void DestroyAllEntities();
    bool m_bDebugMode = true;

    TEntitiesList m_vctEntitiesList = {};
    uint32_t m_uRegisteredEntities = 0;
  };
}



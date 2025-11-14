#pragma once
#include "Game/Entity/Entity.h"
#include "Libs/Utils/Singleton.h"
#include "Engine/Collisions/CollisionManager.h"
#include "Libs/Utils/FixedPool.h"
#include <unordered_set>

namespace collision { class CCollider; }

namespace game
{
  class CGameManager : public utils::CSingleton<CGameManager>
  {
  public:
    static const uint32_t s_uMaxEntities = 100000u;
    typedef utils::CFixedPool<CEntity, s_uMaxEntities> TEntitiesList;

  public:
    CGameManager() {};
    ~CGameManager();

    void Update(float _fDeltaTime);

    CEntity* CreateEntity(const char* _sEntityName);
    bool DestroyEntity(const char* _sEntityName);

  private:
    void DestroyAll();

    std::unordered_set<std::string> m_uSetNames;
    TEntitiesList m_lstEntitiesList = TEntitiesList();
    uint32_t m_uRegisteredEntities = 0;
  };
}



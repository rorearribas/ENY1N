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

    TEntitiesList m_lstEntitiesList = TEntitiesList();
    std::unordered_map<std::string, uint32_t> m_uMapNextSuffix;
    std::unordered_set<std::string> m_uSetNames;
    uint32_t m_uRegisteredEntities = 0;
  };
}



#pragma once
#include "Game/ETT/Entity.h"
#include "Libs/Utils/Singleton.h"
#include "Engine/Collisions/CollisionManager.h"
#include "Libs/Utils/FixedList.h"

namespace collisions { class CCollider; }

namespace game
{
  class CGameManager : public utils::CSingleton<CGameManager>
  {
  public:
    static const uint32_t s_uMaxEntities = 1000;
    typedef utils::CFixedList<CEntity, s_uMaxEntities> TEntitiesList;

  public:
    CGameManager() {};
    ~CGameManager();

    void Update(float _fDeltaTime);

    CEntity* CreateEntity(const char* _sEntityName);
    bool DestroyEntity(const char* _sEntityName);

  private:
    void DestroyAll();

    TEntitiesList m_vctEntitiesList = TEntitiesList();
    uint32_t m_uRegisteredEntities = 0;
  };
}



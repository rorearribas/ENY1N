#pragma once
#include "Game/ETT/Entity.h"
#include "Libs/Utils/Singleton.h"
#include <array>

namespace game
{
  class CGameManager : public utils::CSingleton<CGameManager>
  {
  public:
    static int constexpr s_iMaxEntities = 1000;
    typedef std::array<CEntity*, s_iMaxEntities> TEntitiesList;

  public:
    CGameManager() {}
    ~CGameManager();

    void Update(float _fDeltaTime);
    void SetDebugMode(bool _bStatus) { m_bDebugMode = _bStatus; }

    CEntity* CreateEntity(const char* _sEntityName);
    bool DestroyEntity(const char* _sEntityName);

  private:
    void DestroyAllEntities();

    bool m_bDebugMode = true;
    int m_iRegisteredEntities = 0;
    TEntitiesList m_vctEntitiesList = {};
  };
}



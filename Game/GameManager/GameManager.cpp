#include "GameManager.h"
#include <algorithm>
#include <sstream>

namespace game
{
  // ------------------------------------
  CGameManager::~CGameManager()
  {
    DestroyAllEntities();
  }
  // ------------------------------------
  void CGameManager::Update(float _fDeltaTime)
  {
    for (CEntity* pEntity : m_vctEntitiesList)
    {
      if (pEntity)
      {
        pEntity->Update(_fDeltaTime);
        pEntity->DrawDebug();
      }
    }
  }
  // ------------------------------------
  void CGameManager::DestroyAllEntities()
  {
    std::for_each(m_vctEntitiesList.begin(), m_vctEntitiesList.end(), [&](CEntity*& pEntity_)
    {
      if (pEntity_)
      {
        delete pEntity_;
        pEntity_ = nullptr;
      }
    });
  }
  // ------------------------------------
  CEntity* CGameManager::CreateEntity(const char* _sEntityName)
  {
    if (m_iRegisteredEntities >= s_iMaxEntities) return nullptr;

    // Check id collision
    std::string sTargetEntityID = _sEntityName;
    int iNameSuffix = 1;
    for (const auto& entity : m_vctEntitiesList) 
    {
      if (entity && entity->GetName() == sTargetEntityID) 
      {
        std::ostringstream oss;
        oss << _sEntityName << "_" << iNameSuffix++;
        sTargetEntityID = oss.str();
      }
    }

    // Register entity
    CEntity*& pEntity = m_vctEntitiesList[m_iRegisteredEntities++];
    pEntity = new CEntity(sTargetEntityID.c_str());
    return pEntity;
  }
  // ------------------------------------
  bool CGameManager::DestroyEntity(const char* _sEntityName)
  {
    bool bFound = false;
    for (CEntity*& pEntity : m_vctEntitiesList)
    {
      if (pEntity && pEntity->GetName() == _sEntityName)
      {
        delete pEntity;
        pEntity = nullptr;

        m_iRegisteredEntities--;
        bFound = true;
      }
    }
    if (bFound)
    {
      auto oReorderFunc = std::remove_if(m_vctEntitiesList.begin(), m_vctEntitiesList.end(),
      [](CEntity* _pEntity) { return _pEntity == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_vctEntitiesList.end(), nullptr); // Set nullptr
      return true;
    }
    return false;
  }
}
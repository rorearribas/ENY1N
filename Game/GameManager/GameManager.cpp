#include "GameManager.h"
#include <algorithm>
#include <sstream>
#include "Libs/ImGui/imgui.h"

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
    static int selectedEntityIndex = -1;
    if (m_bDebugMode)
    {
      ImGui::Begin("Entity Selector");
      // Show the list of entities
      for (int i = 0; i < m_vctEntitiesList.size(); ++i)
      {
        CEntity* pEntity = m_vctEntitiesList[i];
        if (pEntity)
        {
          std::string entityLabel = pEntity->GetName() + "##" + std::to_string(i);
          if (ImGui::Selectable(entityLabel.c_str(), selectedEntityIndex == (int)i))
            selectedEntityIndex = i;
        }
      }

      ImGui::End();
    }

    // Update
    for (size_t i = 0; i < m_vctEntitiesList.size(); ++i)
    {
      CEntity* pEntity = m_vctEntitiesList[i];
      if (pEntity)
      {
        pEntity->Update(_fDeltaTime);
        if (selectedEntityIndex == i && m_bDebugMode)
        {
          pEntity->DrawDebug();
        }
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
    if (m_iRegisteredEntities >= s_iMaxEntities) 
      return nullptr;

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
        break;
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
    bool bFoundEntity = false;
    for (CEntity*& pEntity : m_vctEntitiesList)
    {
      if (pEntity && pEntity->GetName() == _sEntityName)
      {
        delete pEntity;
        pEntity = nullptr;

        m_iRegisteredEntities--;
        bFoundEntity = true;
      }
    }
    if (bFoundEntity)
    {
      auto oReorderFunc = std::remove_if(m_vctEntitiesList.begin(), m_vctEntitiesList.end(),
      [](CEntity* _pEntity) { return _pEntity == nullptr; }); // Reorder fixed list
      std::fill(oReorderFunc, m_vctEntitiesList.end(), nullptr); // Set nullptr
      return true;
    }
    return false;
  }
}
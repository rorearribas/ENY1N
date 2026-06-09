#include "GameManager.h"
#include <algorithm>
#include <sstream>
#include "Libs/ImGui/imgui.h"
#include "Libs/Macros/GlobalMacros.h"

namespace game
{
  // ------------------------------------
  CGameManager::~CGameManager()
  {
    DestroyAll();
  }
  // ------------------------------------
  void CGameManager::Update(float _fDeltaTime)
  {
    static int iSelectedIdx = -1;
#ifdef DEBUG_MODE
    static std::vector<std::string> lstDeleteActors = {};
    ImGui::Begin("Entity Selector");

    // Show the list of entities
    for (uint32_t uIndex = 0; uIndex < m_lstEntitiesList.size(); ++uIndex)
    {
      const CEntity& rEntity = m_lstEntitiesList.at(uIndex);
      std::string sLabel = rEntity.GetName() + "##" + std::to_string(uIndex);
      if (ImGui::Selectable(sLabel.c_str(), iSelectedIdx == static_cast<int>(uIndex)))
      {
        iSelectedIdx = uIndex;
      }

      ImGui::SameLine();
      ImGui::PushID(uIndex);
      if (ImGui::Button("Destroy"))
      {
        lstDeleteActors.emplace_back(rEntity.GetName());
      }
      ImGui::PopID();
    }
    ImGui::End();

    for (std::string sActorID : lstDeleteActors)
    {
      DestroyEntity(sActorID.c_str());
    }
    lstDeleteActors.clear();

#endif
    // Update
    for (uint32_t uIndex = 0; uIndex < m_lstEntitiesList.size(); ++uIndex)
    {
      CEntity& rEntity = m_lstEntitiesList.at(uIndex);
      rEntity.Update(_fDeltaTime);
#ifdef DEBUG_MODE
      if (uIndex == static_cast<uint32_t>(iSelectedIdx))
      {
        ImGui::PushID(uIndex);
        rEntity.DrawDebug();
        ImGui::PopID();
      }
#endif
    }
  }
  // ------------------------------------
  void CGameManager::DestroyAll()
  {
    m_lstEntitiesList.clear();
  }
  // ------------------------------------
  CEntity* CGameManager::CreateEntity(const char* _sEntityName)
  {
    if (m_uRegisteredEntities >= m_lstEntitiesList.max_size())
    {
      return nullptr;
    }

    // Check collision name
    std::string sTarget = _sEntityName;
    if (m_uSetNames.count(sTarget))
    {
      uint32_t& uNextSuffix = m_uMapNextSuffix[_sEntityName];
      if (uNextSuffix == 0)
      {
        uNextSuffix = 1;
      }
      while (m_uSetNames.count(sTarget))
      {
        std::ostringstream oStringStream;
        oStringStream << _sEntityName << "_" << uNextSuffix;
        sTarget = oStringStream.str();
        uNextSuffix++;
      }
    }
    m_uSetNames.emplace(sTarget);

    // Register entity
    m_uRegisteredEntities++;
    return &m_lstEntitiesList.emplace_back(sTarget.c_str());
  }
  // ------------------------------------
  bool CGameManager::DestroyEntity(const char* _sEntityName)
  {
    auto it = m_lstEntitiesList.begin();
    while (it != m_lstEntitiesList.end())
    {
      if (strcmp(it->GetName().c_str(), _sEntityName) == 0)
      {
        m_lstEntitiesList.erase(it);
        return true;
      }
      it++;
    }
    return false;
  }
}